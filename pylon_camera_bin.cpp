// Include OpenCV API
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video/video.hpp>
#include <pylon/PylonIncludes.h>
#include <fstream>
#define COLOR_ROWS 80
#define COLOR_COLS 250
using namespace cv;
using namespace Pylon;
using namespace std;
static const uint32_t c_countOfImagesToGrab = 10;

static const size_t c_maxCamerasToUse = 2;

class CPixelFormatAndAoiConfiguration : public Pylon::CConfigurationEventHandler
{
public:
    void OnOpened( Pylon::CInstantCamera& camera)
    {
        try
        {
            // Allow all the names in the namespace GenApi to be used without qualification.
            using namespace Pylon;
            // Get the camera control object.
            GenApi::INodeMap& nodemap = camera.GetNodeMap();
            // Get the parameters for setting the image area of interest (Image AOI).
            CIntegerParameter width(nodemap, "Width");
            CIntegerParameter height(nodemap, "Height");
            CIntegerParameter offsetX(nodemap, "OffsetX");
            CIntegerParameter offsetY(nodemap, "OffsetY");
            // Maximize the Image AOI.
            offsetX.TrySetToMinimum(); // Set to minimum if writable.
            offsetY.TrySetToMinimum(); // Set to minimum if writable.
            width.SetToMaximum();
            height.SetToMaximum();
            // Set the pixel data format.
            CEnumParameter(nodemap, "PixelFormat").SetValue("Mono12");
        }
        catch (const Pylon::GenericException& e)
        {
            throw RUNTIME_EXCEPTION( "Could not apply configuration. const GenericException caught in OnOpened method msg=%hs", e.what());
        }
    }
};

int main(int argc, char* argv[]){
    // The exit code of the sample application.
    int exitCode = 0;
    // Before using any pylon methods, the pylon runtime must be initialized.
    PylonInitialize();
    try
    {
        CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
        camera.Open();
        GenApi::INodeMap& nodemap = camera.GetNodeMap();
        GenApi::CIntegerPtr width = nodemap.GetNode("Width");
        GenApi::CIntegerPtr height = nodemap.GetNode("Height");
        CEnumParameter pixelFormat(nodemap, "PixelFormat");
        // Remember the current pixel format.
        String_t oldPixelFormat = pixelFormat.GetValue();
        pixelFormat.SetValue( "Mono12");
        camera.StartGrabbing( GrabStrategy_LatestImageOnly);
        CGrabResultPtr ptrGrabResult;

        while(camera.IsGrabbing()){
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if(ptrGrabResult->GrabSucceeded()){
                const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                int width {(int)ptrGrabResult->GetWidth()};
                int height {(int)ptrGrabResult->GetHeight()};
                uint32_t max_int {0};
                for(int i = 0; i < width*height; ++i){
                    auto present_value = (uint32_t) pImageBuffer[i];
                    if(present_value > max_int){
                        max_int = present_value;
                    }
                }
                std::cout << "Highest Value is: " << max_int << std::endl;
                max_int = 0;
                }

            }
        }

    catch (const GenericException &e)
    {
        // Error handling
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

