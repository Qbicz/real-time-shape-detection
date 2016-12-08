// Grab_UsingSequencer_usb.cpp
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

#if defined ( USE_USB )
// Settings for using Basler USB cameras.
#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;
#else
#error Camera type is not specified. Define USE_USB for using Usb cameras.
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
        info.SetDeviceClass(Camera_t::DeviceClass());

        // Create an instant camera object with the first found camera device that matches the specified device class.
        Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));

        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        // Register the standard configuration event handler for enabling software triggering.
        // The software trigger configuration handler replaces the default configuration
        // as all currently registered configuration handlers are removed by setting the registration mode to RegistrationMode_ReplaceAll.
        camera.RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

        // Open the camera.
        camera.Open();
        if (GenApi::IsAvailable(camera.SequencerMode))
        {
            // Disable the sequencer before changing parameters.
            // The parameters under control of the sequencer are locked
            // when the sequencer is enabled. For a list of parameters
            // controlled by the sequencer, see the camera User's Manual.
            camera.SequencerMode.SetValue(SequencerMode_Off);
            camera.SequencerConfigurationMode.SetValue(SequencerConfigurationMode_On);

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

            // The sequence sets relate to three steps (0..2).
            // In each step, the height of the Image AOI is doubled.

            int64_t increments = (camera.Height.GetMax() - camera.Height.GetMin()) / camera.Height.GetInc();

            int64_t initialSet = camera.SequencerSetSelector.GetMin();
            int64_t incSet = camera.SequencerSetSelector.GetInc();
            int64_t curSet = initialSet;

            // Set the parameters for step 0; quarter height image.
            camera.SequencerSetSelector.SetValue(initialSet);
            { // valid for all sets
                // reset on software signal 1;
                camera.SequencerPathSelector.SetValue(0);
                camera.SequencerSetNext.SetValue(initialSet);
                camera.SequencerTriggerSource.SetValue(SequencerTriggerSource_SoftwareSignal1);
                // advance on Frame Start
                camera.SequencerPathSelector.SetValue(1);
                camera.SequencerTriggerSource.SetValue(SequencerTriggerSource_FrameStart);
            }
            camera.SequencerSetNext.SetValue(curSet + incSet);
            // quarter height
            camera.Height.SetValue(camera.Height.GetInc() * (increments / 4));
            camera.SequencerSetSave.Execute();

            // Set the parameters for step 1; half height image.
            curSet += incSet;
            camera.SequencerSetSelector.SetValue(curSet);
            // advance on Frame Start to next set
            camera.SequencerSetNext.SetValue(curSet + incSet);
            // half height
            camera.Height.SetValue(camera.Height.GetInc() * (increments / 2));
            camera.SequencerSetSave.Execute();

            // Set the parameters for step 2; full height image.
            curSet += incSet;
            camera.SequencerSetSelector.SetValue(curSet);
            // advance on Frame End to initial set,
            camera.SequencerSetNext.SetValue(initialSet); // terminates sequence definition
            // full height
            camera.Height.SetValue(camera.Height.GetInc() * increments);
            camera.SequencerSetSave.Execute();

            // Enable the sequencer feature.
            // From here on you cannot change the sequencer settings anymore.
            camera.SequencerConfigurationMode.SetValue(SequencerConfigurationMode_Off);
            camera.SequencerMode.SetValue(SequencerMode_On);

            // Start the grabbing of c_countOfImagesToGrab images.
            camera.StartGrabbing(c_countOfImagesToGrab);

            // This smart pointer will receive the grab result data.
            CGrabResultPtr grabResult;

            // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
            // when c_countOfImagesToGrab images have been retrieved.
            while (camera.IsGrabbing())
            {
                // Execute the software trigger. Wait up to 100 ms for the camera to be ready for trigger.
                if (camera.WaitForFrameTriggerReady(100, TimeoutHandling_ThrowException))
                {
                    camera.ExecuteSoftwareTrigger();

                    // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
                    camera.RetrieveResult(5000, grabResult, TimeoutHandling_ThrowException);

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
                        const uint8_t *pImageBuffer = (uint8_t *)grabResult->GetBuffer();
                        cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;
                    }
                    else
                    {
                        cout << "Error: " << grabResult->GetErrorCode() << " " << grabResult->GetErrorDescription() << endl;
                    }
                }

                // Wait for user input.
                cerr << endl << "Press enter to continue." << endl << endl;
                while (camera.IsGrabbing() && cin.get() != '\n');
            }

            // Disable the sequencer.
            camera.SequencerMode.SetValue(SequencerMode_Off);
        }
        else
        {
            cout << "The sequencer feature is not available for this camera." << endl;
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
    while (cin.get() != '\n');

    // Releases all pylon resources. 
    PylonTerminate(); 

    return exitCode;
}
