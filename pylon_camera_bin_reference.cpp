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
        pixelFormat.SetValue( "Mono12");
        camera.StartGrabbing( GrabStrategy_LatestImageOnly);
        CGrabResultPtr ptrGrabResult;
        uint32_t max_int;
        while(camera.IsGrabbing()){
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if(ptrGrabResult->GrabSucceeded()){
                int width {(int)ptrGrabResult->GetWidth()};
                int height {(int)ptrGrabResult->GetHeight()};
                const uint16_t *pImageBuffer = (uint16_t *) ptrGrabResult->GetBuffer();
                max_int = 0;
                for(int i=0; i < width*height; ++i){
                    if(max_int < (uint32_t) pImageBuffer[i]){
                        max_int = (uint32_t) pImageBuffer[i];
                    }
                }
                cout << "Max Value in the array: " << max_int << std::endl;
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

