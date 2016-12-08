// ParametrizeCamera_LookupTable.cpp
/*
    Note: Before getting started, Basler recommends reading the Programmer's Guide topic
    in the pylon C++ API documentation that gets installed with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the Migration topic in the pylon C++ API documentation.

    This sample program demonstrates the use of the Luminance Lookup Table feature.
*/

// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>

// Namespace for using pylon objects.
using namespace Pylon;

#if defined( USE_1394 )
// Settings to use Basler IEEE 1394 cameras.
#include <pylon/1394/Basler1394InstantCamera.h>
typedef Pylon::CBasler1394InstantCamera Camera_t;
using namespace Basler_IIDC1394CameraParams;
#elif defined ( USE_GIGE )
// Settings to use Basler GigE cameras.
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;
#elif defined ( USE_USB )
// Settings to use Basler USB cameras.
#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;
#else
#error camera type is not specified. For example, define USE_GIGE for using GigE cameras
#endif

// Namespace for using cout.
using namespace std;

int main(int argc, char* argv[])
{
    // The exit code of the sample application.
    int exitCode = 0;

    // Before using any pylon methods, the pylon runtime must be initialized. 
    PylonInitialize();

    try
    {
        // Only look for cameras supported by Camera_t
        CDeviceInfo info;
        info.SetDeviceClass( Camera_t::DeviceClass());

        // Create an instant camera object with the first found camera device matching the specified device class.
        Camera_t camera( CTlFactory::GetInstance().CreateFirstDevice( info));

        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        cout << "Opening camera...";

        // Open the camera.
        camera.Open();

        cout << "done" << endl;

        cout << "Writing LUT....";

        // Select the lookup table using the LUTSelector.
        camera.LUTSelector.SetValue( LUTSelector_Luminance );

        // Some cameras have 10 bit and others have 12 bit lookup tables, so determine
        // the type of the lookup table for the current device.
        const int nValues = (int) camera.LUTIndex.GetMax() + 1;
        int inc;
        if ( nValues == 4096 ) // 12 bit LUT.
            inc = 8;
        else if ( nValues == 1024 ) // 10 bit LUT.
            inc = 2;
        else
        {
            throw RUNTIME_EXCEPTION( "Type of LUT is not supported by this sample.");
        }

        // Use LUTIndex and LUTValue parameter to access the lookup table values.
        // The following lookup table causes an inversion of the sensor values.

        for ( int i = 0; i < nValues; i += inc )
        {
            camera.LUTIndex.SetValue( i );
            camera.LUTValue.SetValue( nValues - 1 - i );
        }

        cout << "done" << endl;

        // Enable the lookup table.
        camera.LUTEnable.SetValue( true );

        // Grab and process images here.
        // ...

        // Disable the lookup table.
        camera.LUTEnable.SetValue( false );

        // Close the camera.
        camera.Close();
    }
    catch (const GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
        << e.GetDescription() << endl;
        exitCode = 1;
    }

    // Comment the following two lines to disable waiting on exit
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    // Releases all pylon resources. 
    PylonTerminate();  

    return exitCode;
}

