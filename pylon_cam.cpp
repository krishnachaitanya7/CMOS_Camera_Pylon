// Include OpenCV API
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video/video.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <pylon/PylonIncludes.h>
#include <fstream>
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
        while(camera.IsGrabbing()){
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if(ptrGrabResult->GrabSucceeded()){
                int width {(int)ptrGrabResult->GetWidth()};
                int height {(int)ptrGrabResult->GetHeight()};
//                const uint16_t *pImageBuffer = (uint16_t *) ptrGrabResult->GetBuffer();
                openCvImage = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),
                        CV_16UC1, (uint32_t *) ptrGrabResult->GetBuffer());
//                detector->detect(openCvImage, keypoints);
//                drawKeypoints( openCvImage, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );//
                //namedWindow("keypoints", WINDOW_NORMAL);
                //resizeWindow("keypoints", 300, 300);
                //imshow("keypoints", openCvImage );
                // ToDo: Test by turning the NIR light on. Should work
                Mat dst;
                clahe->apply(openCvImage,dst);
                imshow("lena_CLAHE",dst);
                waitKey(1);
                cv::minMaxLoc(openCvImage, &min, &max);
                cout << "Max Value: " << max << endl;

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

