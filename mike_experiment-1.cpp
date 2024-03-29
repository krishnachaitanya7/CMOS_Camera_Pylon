// Include OpenCV API
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video/video.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <pylon/PylonIncludes.h>
#include <fstream>
#include <thread>
#include <chrono>
#include "gnuplot-iostream/gnuplot-iostream.h"
#define COLOR_ROWS 80
#define COLOR_COLS 250
using namespace cv;
using namespace Pylon;
using namespace std;

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
        Mat openCvImage;
        SimpleBlobDetector::Params params;
        params.filterByCircularity = true;
        cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
        std::vector<KeyPoint> keypoints;
        Mat im_with_keypoints;
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->setClipLimit(4);
        double min, max;
        Gnuplot gp;
        std::vector<std::pair<int, int> > pts_A;
        while(camera.IsGrabbing()){
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if(ptrGrabResult->GrabSucceeded()){
                int width {(int)ptrGrabResult->GetWidth()};
                int height {(int)ptrGrabResult->GetHeight()};
//                const uint16_t *pImageBuffer = (uint16_t *) ptrGrabResult->GetBuffer();
                openCvImage = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),
                                  CV_16UC1, (uint32_t *) ptrGrabResult->GetBuffer());

                // ToDo: Test by turning the NIR light on. Should work
                // ToDo: Test with and without CLAHE
                // ToDo: Use 16bit to 8bit conversion just to get the center of blob
//                cv::minMaxLoc(openCvImage, &min, &max);
//                cout << "Max Value: " << max << endl;
                for(int i = 0; i< 1280; i++){

                    pts_A.push_back(std::make_pair(i, static_cast<int>(openCvImage.at<uint16_t>(671, i))));
//                    std::cout << openCvImage.at<uint16_t>(784, i) << std::endl;

                }
                gp << "set xrange [0:1280]\nset yrange [0:4095]\n";
                // '-' means read from stdin.  The send1d() function sends data to gnuplot's stdin.
                gp << "plot '-' with lines title 'Breathing Exercise'\n";
                gp.send1d(pts_A);
                break;

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
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');
    PylonTerminate();
    return exitCode;
}

