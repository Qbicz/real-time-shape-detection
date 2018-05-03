#define NOMINMAX
#include <CL/cl.hpp>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "OpenCVUtil.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/operations.hpp>

// for OPENCV-CL
#include "opencv2/ocl/ocl.hpp"

#define SAMPLE_VERSION "AMD-APP-SDK-v3.0.130.2"

using namespace cv;
using namespace cv::ocl;
using namespace std;
using namespace appsdk;

/*!<The string to hold the OpenCL kernel file name.*/
const char *kernelFile = "CartoonFilter_Kernels.cl";

cv::ocl::ProgramSource program_src1("RGB2Gray",NULL, NULL);

cv::ocl::ProgramSource program_src2("Gray2RGB",NULL, NULL);



/*!<OpenCL information.*/

cv::ocl::DevicesInfo oclinfo;


int main( int argc, char** argv )
{  
    if(argc != 3)
    {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return SDK_FAILURE;
    }

//(not) required variables    
    cv::Mat cvFrameCPU;  /*!<The input image file in cv::Mat format.*/
    cv::Mat cvSrcMat;    /*!<The input cv::Mat for filtering.*/
    cv::Mat cvDstMat;    /*!<The output cv::Mat obtained after filtering.*/
    cv::Mat cvCannyDstMat;  /*!<The output canny matrix in cv::Mat.*/
    cv::Mat cvDstCPU;    /*!<The output cv::Mat obtained after
                    *  verification by running on CPU.*/
    cv::ocl::oclMat oclSrcMat;  /*!<The input image in cv::ocl::oclMat format.*/
    cv::ocl::oclMat oclDstMat;
    
    std::string imageFile(argv[1]);
    std::string outputFile(argv[2]);

//initialize openCL device   
    cv::ocl::DevicesInfo oclinfo;
    int devnums = cv::ocl::getOpenCLDevices(oclinfo, CVCL_DEVICE_TYPE_CPU); 

    if(devnums < 1)
    {
        std::cout << "Device not found!" << std::endl;
        return SDK_FAILURE;
    }

    IplImage* iplImg = cvLoadImage(imageFile.c_str());
    cvFrameCPU = iplImg;
    if( cvFrameCPU.empty() )
    {
        std::cout << "Unable to open file: " << imageFile << std::endl;
        return SDK_FAILURE;
    }   
    
//perform some basic tasks like computing matrix norm
    cv::ocl::oclMat image;
    
    //the lines are required for interoperability between OpenCV and OpenCL matrixes!
    cv::cvtColor(cvFrameCPU, cvSrcMat, CV_BGRA2BGR); 
    image = cvSrcMat;
    
    double normL2 = cv::ocl::norm(image, NORM_L2);
    printf("Computed matrix norm is: %f\n", normL2);

//blur the image, write to file and show in new window    
    
    cv::ocl::blur(image, oclDstMat, cv::Size( 3, 3 ));
    cvDstMat = oclDstMat;

// Write to disk.
    imwrite(outputFile, cvDstMat);

// display
    cv::namedWindow("original");
    cv::namedWindow("blurred");
    cv::imshow("original", cvFrameCPU);
    cv::imshow("blurred", cvDstMat);
    waitKey(0);

    return SDK_SUCCESS;
}
