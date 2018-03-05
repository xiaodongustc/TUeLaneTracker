#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include "../include/Config.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
  int lReturn =0;

  if(argc >= 2)
  {
    FileStorage fs (argv[1], FileStorage::WRITE);
    time_t rawtime; time(&rawtime);
    fs<< "callibrationDate" << asctime(localtime(&rawtime));

    

    // 3x4 Intrinsic Parameters

    //Calculating the values based on camera parameters
    float X_DIM = CAMERA_RES_H / 2; //[Pixels]
    float Y_DIM = CAMERA_RES_V / 2; //[Pixels]
    float AXIS_SKEW = 0;
    // Assuming the given focal length is correct:
    float FOCAL_X = CAMERA_FOCAL_LENGTH;
    float FOCAL_Y = CAMERA_FOCAL_LENGTH; 
    //Alternative is calculating it based on the FOV and dimension of the image: (Not tested for correct results!)
    //float FOCAL_X = X_DIM / tan(CAMERA_FOV_h/2);
    //float FOCAL_Y = Y_DIM / tan(CAMERA_FOV_V/2);

    Mat CameraMatrixIntrinsic = (Mat_<float>(3,4) << FOCAL_X,  AXIS_SKEW,  X_DIM,  0,
						     0,  FOCAL_Y,  Y_DIM,  0,
						     0,  0,  1,  0);

    // 3x3 rotation Matrix
    float ANGLE_X = 0; //Angle around x axis [radians]
    float ANGLE_Y = 0; //Angle around y axis [radians]
    float ANGLE_Z = 0; //Angle around z axis [radians]

    //Use simple rotation matrices and then combine
    //It is possible to put it all in a single rotation matrix, but that will reduce understandability and troubleshooting
    Mat RotationX = (Mat_<float>(3,3) << 1, 0, 0,
                0, cos(ANGLE_X), -sin(ANGLE_X),
                0, sin(ANGLE_X), cos(ANGLE_X));
    Mat RotationY = (Mat_<float>(3,3) << cos(ANGLE_Y), 0, -sin(ANGLE_Y),
                0, 1, 0,
                sin(ANGLE_Y), 0, cos(ANGLE_Y));
    Mat RotationZ = (Mat_<float>(3,3) << cos(ANGLE_Z), -sin(ANGLE_Z), 0,
                sin(ANGLE_Z), cos(ANGLE_Z), 0,
                0, 0, 1);
    Mat RotationCombined = RotationX * RotationY * RotationZ;

    // 3x1 Translation Matrix
    float TRANSL_X = CAMERA_OFFSET_X;
    float TRANSL_Y = 0;
    float TRANSL_Z = CAMERA_HEIGHT;
    Mat TranslationMatrix = (Mat_<float>(3,1)<< TRANSL_X,
                TRANSL_Y,
                TRANSL_Z);

    Mat BottomRowExentric = (Mat_<float>(1,4) << 0, 0, 0, 1);

    // 4x4  Extrinsic Parameters
    // For extrinsic, concatenate [Rotation (3x3) , Translation(3x1); 0(1x3), 1(1x1)]
    /*Mat CameraMatrixExtrinsic = (Mat_<float>(4,4)<< RotationCombined, TranslationMatrix,
                EmptyRow, 1);
   	*/
      Mat CameraMatrixExtrinsic;
      hconcat(RotationCombined,TranslationMatrix,CameraMatrixExtrinsic);
      vconcat(CameraMatrixExtrinsic,BottomRowExentric,CameraMatrixExtrinsic);
    fs << "CAMERA_MATRIX_INTRINSIC" << CameraMatrixIntrinsic;
    fs << "CAMERA_MATRIX_EXTRINSIC" << CameraMatrixExtrinsic;
   
    fs.release();
  }

  else
  {
    cout<< "Missing Command Line Options" <<endl<<endl;

    cout<< "Usage:"<<endl;
    cout<< "	cameraCallibrator <char*> NAME_OF_THE_CALLIBRAION FILE " <<endl;
    
    lReturn = -1;

  }   
    cout << "Program exited with return value: "<< lReturn <<endl;
    return lReturn;
}
