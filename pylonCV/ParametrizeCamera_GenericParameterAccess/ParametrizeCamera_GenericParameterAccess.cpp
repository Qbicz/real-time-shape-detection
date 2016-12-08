// ParametrizeCamera_GenericParameterAccess.cpp
/*
    Note: Before getting started, Basler recommends reading the Programmer's Guide topic
    in the pylon C++ API documentation that gets installed with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the Migration topic in the pylon C++ API documentation.

    For camera configuration and for accessing other parameters, the pylon API
    uses the technologies defined by the GenICam standard hosted by the
    European Machine Vision Association (EMVA). The GenICam specification
    (http://www.GenICam.org) defines a format for camera description files.
    These files describe the configuration interface of GenICam compliant cameras.
    The description files are written in XML (eXtensible Markup Language) and
    describe camera registers, their interdependencies, and all other
    information needed to access high-level features such as Gain,
    Exposure Time, or Image Format by means of low-level register read and
    write operations.

    The elements of a camera description file are represented as software
    objects called Nodes. For example, a node can represent a single camera
    register, a camera parameter such as Gain, a set of available parameter
    values, etc. Each node implements the GenApi::INode interface.

    The nodes are linked together by different relationships as explained in the
    GenICam standard document available at www.GenICam.org. The complete set of
    nodes is stored in a data structure called Node Map.
    At runtime, a Node Map is instantiated from an XML description.

    This sample shows the 'generic' approach for configuring a camera
    using the GenApi nodemaps represented by the GenApi::INodeMap interface.

    The names and types of the parameter nodes can be found in the Basler pylon Programmer's Guide
    and API Reference, in the camera User's Manual, in the camera's document about
    Register Structure and Access Methodes (if applicable), and by using the pylon Viewer tool.

    See also the ParametrizeCamera_NativeParameterAccess sample for the 'native'
    approach for configuring a camera.
*/

// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using GenApi objects.
using namespace GenApi;


// Namespace for using cout.
using namespace std;

// Adjust value to make it comply with range and increment passed.
//
// The parameter's minimum and maximum are always considered as valid values.
// If the increment is larger than one, the returned value will be: min + (n * inc).
// If the value doesn't meet these criteria, it will be rounded down to ensure compliance.
int64_t Adjust(int64_t val, int64_t minimum, int64_t maximum, int64_t inc)
{
    // Check the input parameters.
    if (inc <= 0)
    {
        // Negative increments are invalid.
        throw LOGICAL_ERROR_EXCEPTION("Unexpected increment %d", inc);
    }
    if (minimum > maximum)
    {
        // Minimum must not be bigger than or equal to the maximum.
        throw LOGICAL_ERROR_EXCEPTION("minimum bigger than maximum.");
    }

    // Check the lower bound.
    if (val < minimum)
    {
        return minimum;
    }

    // Check the upper bound.
    if (val > maximum)
    {
        return maximum;
    }

    // Check the increment.
    if (inc == 1)
    {
        // Special case: all values are valid.
        return val;
    }
    else
    {
        // The value must be min + (n * inc).
        // Due to the integer division, the value will be rounded down.
        return minimum + ( ((val - minimum) / inc) * inc );
    }
}

int main(int argc, char* argv[])
{

    // The exit code of the sample application.

    int exitCode = 0;

    // Before using any pylon methods, the pylon runtime must be initialized. 
    PylonInitialize();

    try
    {
        // Create an instant camera object with the camera found first.
        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());

        INodeMap& nodemap = camera.GetNodeMap();

        // Open the camera for accessing the parameters.
        camera.Open();

        // Get camera device information.
        cout << "Camera Device Information" << endl
             << "=========================" << endl;
        cout << "Vendor           : "
             << CStringPtr( nodemap.GetNode( "DeviceVendorName") )->GetValue() << endl;
        cout << "Model            : "
             << CStringPtr( nodemap.GetNode( "DeviceModelName") )->GetValue() << endl;
        cout << "Firmware version : "
             << CStringPtr( nodemap.GetNode( "DeviceFirmwareVersion") )->GetValue() << endl << endl;

        // Camera settings.
        cout << "Camera Device Settings" << endl
             << "======================" << endl;



        // Set the AOI:

        // Get the integer nodes describing the AOI.
        CIntegerPtr offsetX( nodemap.GetNode( "OffsetX"));
        CIntegerPtr offsetY( nodemap.GetNode( "OffsetY"));
        CIntegerPtr width( nodemap.GetNode( "Width"));
        CIntegerPtr height( nodemap.GetNode( "Height"));

        // On some cameras the offsets are read-only,
        // so we check whether we can write a value. Otherwise, we would get an exception.
        // GenApi has some convenience predicates to check this easily.
        if ( IsWritable( offsetX))
        {
            offsetX->SetValue( offsetX->GetMin());
        }
        if ( IsWritable( offsetY))
        {
            offsetY->SetValue( offsetY->GetMin());
        }

        // Some properties have restrictions. Use GetInc/GetMin/GetMax to make sure you set a valid value.
        int64_t newWidth = 640;
        newWidth = Adjust(newWidth, width->GetMin(), width->GetMax(), width->GetInc());

        int64_t newHeight = 480;
        newHeight = Adjust(newHeight, height->GetMin(), height->GetMax(), height->GetInc());

        width->SetValue(newWidth);
        height->SetValue(newHeight);

        cout << "OffsetX          : " << offsetX->GetValue() << endl;
        cout << "OffsetY          : " << offsetY->GetValue() << endl;

        cout << "Width            : " << width->GetValue() << endl;
        cout << "Height           : " << height->GetValue() << endl;



        // Access the PixelFormat enumeration type node.
        CEnumerationPtr pixelFormat( nodemap.GetNode( "PixelFormat"));

        // Remember the current pixel format.
        String_t oldPixelFormat = pixelFormat->ToString();
        cout << "Old PixelFormat  : " << oldPixelFormat << endl;

        // Set the pixel format to Mono8 if available.
        if ( IsAvailable( pixelFormat->GetEntryByName( "Mono8")))
        {
            pixelFormat->FromString( "Mono8");
            cout << "New PixelFormat  : " << pixelFormat->ToString() << endl;
        }



        // Set the new gain to 50% ->  Min + ((Max-Min) / 2).
        //
        // Note: Some newer camera models may have auto functions enabled.
        //       To be able to set the gain value to a specific value
        //       the Gain Auto function must be disabled first.
        // Access the enumeration type node GainAuto.
        CEnumerationPtr gainAuto( nodemap.GetNode( "GainAuto"));
        if ( IsWritable( gainAuto))
        {
            gainAuto->FromString("Off");
        }

        // Check to see which Standard Feature Naming Convention (SFNC) is used by the camera device.
        if ( camera.GetSfncVersion() >= Sfnc_2_0_0)
        {
            // Access the Gain float type node. This node is available for USB camera devices.
            // USB camera devices are compliant to SFNC version 2.0.
            CFloatPtr gain( nodemap.GetNode( "Gain"));
            double newGain = gain->GetMin() + ((gain->GetMax() - gain->GetMin()) / 2);
            gain->SetValue(newGain);
            cout << "Gain (50%)       : " << gain->GetValue() << " (Min: " << gain->GetMin() << "; Max: " << gain->GetMax() << ")" << endl;
        }
        else
        {
            // Access the GainRaw integer type node. This node is available for IIDC 1394 and GigE camera devices.
            CIntegerPtr gainRaw( nodemap.GetNode( "GainRaw"));
            int64_t newGainRaw = gainRaw->GetMin() + ((gainRaw->GetMax() - gainRaw->GetMin()) / 2);
            // Make sure the calculated value is valid.
            newGainRaw = Adjust(newGainRaw, gainRaw->GetMin(), gainRaw->GetMax(), gainRaw->GetInc());
            gainRaw->SetValue(newGainRaw);
            cout << "Gain (50%)       : " << gainRaw->GetValue() << " (Min: " << gainRaw->GetMin() << "; Max: " << gainRaw->GetMax() << "; Inc: " << gainRaw->GetInc() << ")" << endl;
        }

        // Restore the old pixel format.
        pixelFormat->FromString(oldPixelFormat);

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

    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    // Releases all pylon resources. 
    PylonTerminate(); 

    return exitCode;
}

