/**********************************************************************
Copyright ©2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

/**
*******************************************************************************
* @file <OpenCV-CLIntro.cpp>
* @brief Very basic program,
*        exercising functionality of the OCL module of OpenCV.
*******************************************************************************
*/

#include <CL/cl.h>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

// for OPENCV-CL
#include "opencv2/ocl/ocl.hpp"

#define SDK_SUCCESS 0
#define SDK_FAILURE 1

using namespace cv;
using namespace cv::ocl;
using namespace std;

/**
*******************************************************************************
* @fn printMaxDiff
* @brief Print the max difference between cpuCalcResult obtained from OpenCV
*        function and destGPU obtained from corresponding OpenCV-CL function.
*
* @param cpuCalcResult The CPU calculated result.
* @param destGPU The GPU calculated result.
*******************************************************************************
*/
void printMaxDiff(cv::Mat cpuCalcResult, cv::ocl::oclMat destGPU)
{
  cv::Mat dstCPU;  /*!<GPU result copied to CPU, for display */
  /****************************************************************************
  * Copy from GPU to CPU.                                                     *
  ****************************************************************************/
  dstCPU = destGPU;

  if(cpuCalcResult.empty() == false)
  {
    double diff = cv::norm(dstCPU, cpuCalcResult, cv::NORM_INF );
    std::cout << "Maximum difference, all pixels = " << diff << std::endl;
  }
}

/**
*******************************************************************************
* @fn main
* @brief Run the filters - Morphology, Box Filter and Laplacian.
*
* @return int Success or failure status.
*******************************************************************************
*/
int main( int argc, const char** argv )
{
  std::cout << "=================================" << std::endl;
  std::cout << "Introducing Filters in OPENCV-CL" << std::endl;
  std::cout << "=================================" << std::endl;


  cv::ocl::DevicesInfo oclinfo;

//CVCL_DEVICE_TYPE_CPU should be changed to CVCL_DEVICE_TYPE_GPU
  int devnums = cv::ocl::getOpenCLDevices(oclinfo, CVCL_DEVICE_TYPE_CPU); 

  if(devnums < 1)
  {
    std::cout << "Device not found!" << std::endl;
    return SDK_FAILURE;
  }

  cv::Mat frameCPU, frameCopyCPU, imageWrite;

  cv::ocl::oclMat frameCopyGPU;
  cv::ocl::oclMat destGPU;  /*!<The gpu result.*/

  cv::ocl::oclMat destGPU1, destGPU2;

  cv::Mat cpuCalcResult;

  IplImage* iplImg = cvLoadImage( "OpenCV-CLIntro_Input.jpg");
  frameCPU = iplImg;
  if( frameCPU.empty() )
  {
    std::cout << "Failed to load image!" << iplImg << std::endl;
    return SDK_FAILURE;
  }

  /****************************************************************************
  * Load image                                                                *
  ****************************************************************************/
  switch (frameCPU.type())
  {
    case CV_8UC1:
      cv::cvtColor(frameCPU, frameCopyCPU, CV_BGR2GRAY);
      break;
    case CV_8UC4:
    default:
      cv::cvtColor(frameCPU, frameCopyCPU, CV_BGR2BGRA);
      break;
  }

  /****************************************************************************
  * Copy from CPU to GPU!                                                     *
  ****************************************************************************/
  frameCopyGPU = frameCopyCPU;

  try
  {
    /**************************************************************************
    * MORPHOLOGY                                                              *
    **************************************************************************/
    std::cout << "Running Morphology..." << std::endl;
    // 3rd argument is a 7x7 structuring element
    cv::ocl::erode(frameCopyGPU, destGPU1, cv::Mat(7,7,CV_8U,cv::Scalar(1)));
    cv::ocl::dilate(frameCopyGPU, destGPU2, cv::Mat(7,7,CV_8U,cv::Scalar(1)));
    // Math works!
    destGPU = destGPU2 - destGPU1;
    // Write to disk.
    imageWrite = destGPU;
    imwrite("Output_Morphology.jpg", imageWrite);

    /**************************************************************************
    * BOX_FILTER                                                              *
    **************************************************************************/
    std::cout << "Running Box Filter... ";
    destGPU.setTo(0);
    cv::ocl::boxFilter(frameCopyGPU, destGPU,
                       frameCopyGPU.type(), cv::Size(5,5));
    cv::boxFilter(frameCopyCPU, cpuCalcResult,
                  frameCopyCPU.type(), cv::Size(5,5));
    printMaxDiff(cpuCalcResult, destGPU);
    // Write to disk.
    imageWrite = destGPU;
    imwrite("Output_BoxFilter.jpg", imageWrite);

    /**************************************************************************
    * LAPLACIAN_FILTER                                                        *
    **************************************************************************/
    std::cout << "Running Laplacian Filter... ";
    destGPU.setTo(0);
    cpuCalcResult.setTo(0);
    cv::ocl::Laplacian(frameCopyGPU, destGPU, -1);
    cv::Laplacian(frameCopyCPU, cpuCalcResult, -1);
    printMaxDiff(cpuCalcResult, destGPU);
    // Write to disk.
    imageWrite = destGPU;
    imwrite("Output_Laplacian.jpg", imageWrite);
  }
  catch (cv::Exception &e)
  {
    const char *errMsg = e.what();
    cout << "Exception encountered with message: " << errMsg << endl;
  }
  
  
  namedWindow( "Laplacian Filter", CV_WINDOW_AUTOSIZE );
  imshow( "Laplacian Filter", imageWrite );
  waitKey(0);
  
  return SDK_SUCCESS;
}
