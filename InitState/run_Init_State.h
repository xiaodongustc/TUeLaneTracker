//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: run_Init_State.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 01-Feb-2017 19:25:15
//
#ifndef RUN_INIT_STATE_H
#define RUN_INIT_STATE_H

// Include Files
#include <cmath>
#include <float.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "omp.h"
#include "run_Init_State_types.h"

// Function Declarations
extern void run_Init_State(const int RES_VH[2], int NBUFFER, const
  MatlabStruct_laneFilter *laneFilter, const MatlabStruct_vpFilter *vpFilter,
  MatlabStruct_likelihoods *likelihoods, MatlabStruct_templates *templates,
  MatlabStruct_vanishingPt *vanishingPt, MatlabStruct_focusMask *masks);

#endif

//
// File trailer for run_Init_State.h
//
// [EOF]
//