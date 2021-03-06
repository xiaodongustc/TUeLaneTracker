#ifndef TRACKINGLANESTATE_H
#define TRACKINGLANESTATE_H

/******************************************************************************
* NXP Confidential Proprietary
* 
* Copyright (c) 2017 NXP Semiconductor;
* All Rights Reserved
*
* AUTHOR : Rameez Ismail
*
* THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
* ****************************************************************************/ 

#include <thread>
#include "State.h"
#include "ScalingFactors.h"


#ifdef   S32V2XX
 #include "TrackingLaneDAG_s32v.h"
#else
 #include "TrackingLaneDAG_generic.h"
#endif


template<typename GRAPH>
class TrackingLaneState: public State
{
	
private:
	uint_fast8_t mRetryGrab;
	std::thread mSideExecutor;

public:
	GRAPH 	mGraph;


public:	
	void run();
	void setupDAG(LaneFilter* laneFilter, VanishingPtFilter* vpFilter);
	
	template<typename GRAPH_BASE>
	TrackingLaneState(GRAPH_BASE&& lBaseGraph);

	~TrackingLaneState();
};





//******************************************************************/*
// Class Definitions
//Templated class thus declarations and definitions in a single file
//******************************************************************/

template<typename GRAPH>
template<typename GRAPH_BASE>
TrackingLaneState<GRAPH>::TrackingLaneState(GRAPH_BASE&& lBaseGraph)
:  mRetryGrab(0),
   mGraph(std::move(lBaseGraph))
{	
		
}


template<typename GRAPH>
void TrackingLaneState<GRAPH>::setupDAG(LaneFilter* laneFilter, VanishingPtFilter* vpFilter)
{
	//Setting Up observing Filters for the Graph	
	 mGraph.mLaneFilter = laneFilter;
	 mGraph.mVpFilter   = vpFilter;
	 
	mGraph.mLOWER_LIMIT_IntBase = 
		SCALE_INTSEC*laneFilter->HISTOGRAM_BINS(0) - (SCALE_INTSEC*laneFilter->STEP)/2;
	
 
	mGraph.mUPPER_LIMIT_IntBase = 
	   	SCALE_INTSEC*laneFilter->HISTOGRAM_BINS(laneFilter->mNb_HISTOGRAM_BINS-1) +
		(SCALE_INTSEC*laneFilter->STEP)/2;
	
 
	mGraph.mLOWER_LIMIT_IntPurview = 
	   	-SCALE_INTSEC*vpFilter->VP_RANGE_H - (SCALE_INTSEC*vpFilter->STEP)/2;
	 

	mGraph.mUPPER_LIMIT_IntPurview = 
	   	SCALE_INTSEC*vpFilter->VP_RANGE_H  +  (SCALE_INTSEC*vpFilter->STEP)/2;


	mGraph.mSCALED_STEP_LANE_FILTER		= laneFilter->STEP*SCALE_INTSEC;
	mGraph.mSCALED_STEP_VP_FILTER      	= vpFilter->STEP*SCALE_INTSEC;
	mGraph.mSCALED_START_LANE_FILTER   	= laneFilter->HISTOGRAM_BINS(0)*SCALE_INTSEC;
	mGraph.mSCALED_START_VP_FILTER		= vpFilter->HISTOGRAM_BINS(0)*SCALE_INTSEC;
	
	if (0== mGraph.init_DAG())
	 this->currentStatus= StateStatus::ACTIVE;	
}


template<typename GRAPH>
void TrackingLaneState<GRAPH>::run()
{

#ifdef PROFILER_ENABLED
mProfiler.start("SingleRun_TRACK");
#endif
		
	if (0==mGraph.grabFrame())
	{

	  if (mSideExecutor.joinable())
	     mSideExecutor.join();
		 
	  mSideExecutor = std::thread(&GRAPH::runAuxillaryTasks, std::ref(mGraph));
	  mGraph.buffer();
	  mGraph.extractLanes();
	  this->StateCounter++;
	}
			
	else
	{
	   mRetryGrab ++;
	   if(mRetryGrab >3)
	    currentStatus = StateStatus::ERROR;
	}

		
#ifdef PROFILER_ENABLED
mProfiler.end();
LOG_INFO_(LDTLog::TIMING_PROFILE)<<endl
				<<"******************************"<<endl
				<<  "Completing a TrackingLanes run." <<endl
				<<  "Complete run Time: " << mProfiler.getAvgTime("SingleRun_TRACK")<<endl
				<<"******************************"<<endl<<endl;	
				#endif	
}

template<typename GRAPH>
TrackingLaneState<GRAPH>::~TrackingLaneState()
{
	if (mSideExecutor.joinable())
	   mSideExecutor.join();	
}


#endif // TRACKINGLANESTATE_H
