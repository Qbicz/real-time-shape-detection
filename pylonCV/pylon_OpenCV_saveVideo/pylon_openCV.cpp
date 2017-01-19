/*
   This sample illustrates how to grab and process images using the CInstantCamera class.
   The images are grabbed and processed asynchronously, i.e.,
   while the application is processing a buffer, the acquisition of the next buffer is done
   in parallel.

   The CInstantCamera class uses a pool of buffers to retrieve image data
   from the camera device. Once a buffer is filled and ready,
   the buffer can be retrieved from the camera object for processing. The buffer
   and additional image data are collected in a grab result. The grab result is
   held by a smart pointer after retrieval. The buffer is automatically reused
   when explicitly released or when the smart pointer object is destroyed.
*/
// Include files to use OpenCV API.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// Include files to use the PYLON API.
#include "pylon/PylonIncludes.h"

// Use sstream to create image names including integer
#include <sstream>

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using GenApi objects
using namespace GenApi;

// Namespace for using opencv objects.
using namespace cv;

// Namespace for using cout.
using namespace std;

// Define if images and videos are to be saved.
#define saveImages  1
#define recordVideo 0

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 1000;

int main(int argc, char* argv[])
{
    int exitCode = 0;

    // Automagically call PylonInitialize and PylonTerminate to ensure the pylon runtime system
    // is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    try
    {
        // Create an instant camera object with the camera device found first.
        // CDeviceInfo info;
        // info.SetSerialNumber("21694497");
        cout << "Creating Camera..." << endl;
        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
        cout << "Camera Created." << endl;
        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        
        // TODO: ustawic niska rozdzielczosc zdjecia akwizycji

        // The parameter MaxNumBuffer can be used to control the count of buffers
        // allocated for grabbing. The default value of this parameter is 10.
        camera.MaxNumBuffer = 5;

        // create pylon image format converter and pylon image
        CImageFormatConverter formatConverter;
        formatConverter.OutputPixelFormat= PixelType_BGR8packed;
        CPylonImage pylonImage;
        int grabbedImages = 0;

        cout << "// Get camera nodemap to access parameters\n";
        INodeMap& nodemap = camera.GetNodeMap();
        
        cout << "// Create pointers to access the camera Width and Height\n";
        CIntegerPtr width = nodemap.GetNode("Width");
        CIntegerPtr height = nodemap.GetNode("Height");
        
        cout << "// Create an OpenCV video creator\n";
        VideoWriter cvVideoCreator;
        // Create an OpenCV image
        Mat openCvImage;
        
        cout << "// define the video frame size.\n";
        //cv::Size frameSize = Size((int)width->GetValue(), (int)height->GetValue());
        cv::Size frameSize = Size(640, 480);
        //cout << "Video frame size: " << (int)width->GetValue() << ", " << (int)height->GetValue() << endl;
        
        cout << "// set the codec and frame rate\n";
        if(recordVideo)
        	cvVideoCreator.open("NutsVideo.avi", CV_FOURCC('D', 'I', 'V', 'X'), 20, frameSize, true);
        
        cout << "// Start the grabbing of c_countOfImagesToGrab images.\n";
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.
        camera.StartGrabbing( c_countOfImagesToGrab, GrabStrategy_LatestImageOnly);

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;

        
        // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
        cout << "// when c_countOfImagesToGrab images have been retrieved.\n";
        while ( camera.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
                // Access the image data.
                cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
                cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
                const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                // cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl << endl;
		
                // Convert the grabbed buffer to pylon imag
                formatConverter.Convert(pylonImage, ptrGrabResult);
                // Create an OpenCV image out of pylon image
                openCvImage= cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());

                // set saveImages to '1' to save images.
                if (saveImages)
                {
                    // current image name for saving
                    std::ostringstream s;
                    // create image name files with ascending grabbed image numbers.
                    s << "image_" << grabbedImages << ".jpg";
                    std::string imageName(s.str());
                    // Save an OpenCV image.
                    imwrite(imageName, openCvImage);
                    grabbedImages++;
                }
                
                if(recordVideo)
                    cvVideoCreator.write(openCvImage);
                
                // Create a display window
                namedWindow( "OpenCV Display Window", CV_WINDOW_NORMAL);//AUTOSIZE //FREERATIO
                // Display the current image with opencv
                imshow( "OpenCV Display Window", openCvImage);
                // Define a timeout for customer's input in ms.
                // '0' means indefinite, i.e. the next image will be displayed after closing the window 
                // '1' means live stream
                waitKey(1);

            }
            else
            {
                cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl << e.GetDescription() << endl;
        exitCode = 1;
    }

    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    return exitCode;
}

