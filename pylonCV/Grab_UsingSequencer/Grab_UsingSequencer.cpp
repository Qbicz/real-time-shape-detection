// Grab_UsingSequencer.cpp
/*
    Note: Before getting started, Basler recommends reading the Programmer's Guide topic
    in the pylon C++ API documentation that gets installed with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the Migration topic in the pylon C++ API documentation.

    This sample shows how to grab images using the sequencer feature of a camera.
    Three sequence sets are used for image acquisition. Each sequence set
    uses a different image height.
*/

// Include files to use the PYLON API
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

using namespace Pylon;

#if defined ( USE_GIGE )
// Settings for using Basler GigE Vision cameras.
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;
#else
#error Camera type is not specified. For example, define USE_GIGE for using GigE cameras.
#endif

// Namespace for using cout
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 10;

int main(int argc, char* argv[])
{
    // The exit code of the sample application.
    int exitCode = 0;

    // Before using any pylon methods, the pylon runtime must be initialized. 
    PylonInitialize();

    try
    {
        // Only look for cameras supported by Camera_t.
        CDeviceInfo info;
        info.SetDeviceClass( Camera_t::DeviceClass());

        // Create an instant camera object with the first found camera device that matches the specified device class.
        Camera_t camera( CTlFactory::GetInstance().CreateFirstDevice( info));

        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        // Register the standard configuration event handler for enabling software triggering.
        // The software trigger configuration handler replaces the default configuration
        // as all currently registered configuration handlers are removed by setting the registration mode to RegistrationMode_ReplaceAll.
        camera.RegisterConfiguration( new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

        // Open the camera.
        camera.Open();

        if ( GenApi::IsWritable(camera.SequenceEnable))
        {
            // Disable the sequencer before changing parameters.
            // The parameters under control of the sequencer are locked
            // when the sequencer is enabled. For a list of parameters
            // controlled by the sequencer, see the camera User's Manual.
            camera.SequenceEnable.SetValue(false);

            // Maximize the image area of interest (Image AOI).
            if (IsWritable(camera.OffsetX))
            {
                camera.OffsetX.SetValue(camera.OffsetX.GetMin());
            }
            if (IsWritable(camera.OffsetY))
            {
                camera.OffsetY.SetValue(camera.OffsetY.GetMin());
            }
            camera.Width.SetValue(camera.Width.GetMax());
            camera.Height.SetValue(camera.Height.GetMax());

            // Set the pixel data format.
            camera.PixelFormat.SetValue(PixelFormat_Mono8);

            // Set up sequence sets.

            // Configure how the sequence will advance.
            // 'Auto' refers to the auto sequence advance mode.
            // The advance from one sequence set to the next will occur automatically with each image acquired.
            // After the end of the sequence set cycle was reached a new sequence set cycle will start.
            camera.SequenceAdvanceMode = SequenceAdvanceMode_Auto;

            // Our sequence sets relate to three steps (0..2).
            // In each step we will increase the height of the Image AOI by one increment.
            camera.SequenceSetTotalNumber = 3;

            int64_t increments = (camera.Height.GetMax() - camera.Height.GetMin()) / camera.Height.GetInc();

            // Set the parameters for step 0; quarter height image.
            camera.SequenceSetIndex = 0;
            camera.Height.SetValue( camera.Height.GetInc() * (increments / 4));
            camera.SequenceSetStore.Execute();

            // Set the parameters for step 1; half height image.
            camera.SequenceSetIndex = 1;
            camera.Height.SetValue( camera.Height.GetInc() * (increments / 2));
            camera.SequenceSetStore.Execute();

            // Set the parameters for step 2; full height image.
            camera.SequenceSetIndex = 2;
            camera.Height.SetValue( camera.Height.GetInc() * increments);
            camera.SequenceSetStore.Execute();

            // Enable the sequencer feature.
            // From here on you cannot change the sequencer settings anymore.
            camera.SequenceEnable.SetValue(true);

            // Start the grabbing of c_countOfImagesToGrab images.
            camera.StartGrabbing( c_countOfImagesToGrab);

            // This smart pointer will receive the grab result data.
            CGrabResultPtr grabResult;

            // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
            // when c_countOfImagesToGrab images have been retrieved.
            while ( camera.IsGrabbing())
            {
                // Execute the software trigger. Wait up to 100 ms for the camera to be ready for trigger.
                if ( camera.WaitForFrameTriggerReady( 100, TimeoutHandling_ThrowException))
                {
                    camera.ExecuteSoftwareTrigger();

                    // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
                    camera.RetrieveResult( 5000, grabResult, TimeoutHandling_ThrowException);

                    // Image grabbed successfully?
                    if (grabResult->GrabSucceeded())
                    {
#ifdef PYLON_WIN_BUILD
                        // Display the grabbed image.
                        Pylon::DisplayImage(1, grabResult);
#endif

                        // Access the image data.
                        cout << "SizeX: " << grabResult->GetWidth() << endl;
                        cout << "SizeY: " << grabResult->GetHeight() << endl;
                        const uint8_t *pImageBuffer = (uint8_t *) grabResult->GetBuffer();
                        cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl << endl;
                    }
                    else
                    {
                        cout << "Error: " << grabResult->GetErrorCode() << " " << grabResult->GetErrorDescription() << endl;
                    }
                }

                // Wait for user input.
                cerr << endl << "Press enter to continue." << endl << endl;
                while( camera.IsGrabbing() && cin.get() != '\n');
            }

            // Disable the sequencer.
            camera.SequenceEnable.SetValue(false);
        }
        else
        {
            cout << "The sequencer feature is not available for this camera."<< endl;
        }
    }
    catch (const GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
        exitCode = 1;
    }

    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    // Releases all pylon resources. 
    PylonTerminate();  

    return exitCode;
}
