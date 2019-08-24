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
int get_max_of_array(int &input);

void writeCSV(string &filename, Mat &m){
    ofstream myfile;
    myfile.open(filename.c_str());
    myfile<< cv::format(m, cv::Formatter::FMT_CSV) << std::endl;
    myfile.close();
}

void on_mouse_click(int event, int x, int y, int flags, void* ptr) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        Mat* snapshot = (cv::Mat*)ptr;
        Vec3b pixel = snapshot->at<cv::Vec3b>(cv::Point (x, y));
        int b, g, r;
        b = pixel[0];
        g = pixel[1];
        r = pixel[2];
        std::string rgbText = "[" + std::to_string(r) + ", " + std::to_string(g)
                              + ", " + std::to_string(b) + "]";
        std::cout << "RGB Values at the click x = "<< x << " y= "<< y << rgbText << std::endl;

    }
}

int get_max_of_array(int &input){
    static int max_int {0};
    if(input > max_int){
        max_int = input;
    }
    return max_int;
}

int main(int argc, char* argv[])
{
    // The exit code of the sample application.
    int exitCode = 0;
    // Before using any pylon methods, the pylon runtime must be initialized.
    PylonInitialize();
    try
    {
        CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
        GenApi::INodeMap& nodemap = camera.GetNodeMap();
        camera.Open();
        GenApi::CIntegerPtr width = nodemap.GetNode("Width");
        GenApi::CIntegerPtr height = nodemap.GetNode("Height");
        CImageFormatConverter formatConverter;
        formatConverter.OutputPixelFormat = PixelType_BGR8packed;
        CPylonImage pylonImage;
        Mat openCvImage;
        camera.StartGrabbing( GrabStrategy_LatestImageOnly);
        CGrabResultPtr ptrGrabResult;
        int keyVal;
        Mat snapshot, colorArray;
        bool run_once = true;

        while(camera.IsGrabbing()){
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if(ptrGrabResult->GrabSucceeded()){
                formatConverter.Convert(pylonImage, ptrGrabResult);
                openCvImage = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
                if(run_once){
                    snapshot = cv::Mat(openCvImage.size(), CV_8UC3, cv::Scalar(0,0,0));
                    cv::imshow("Snapshot", snapshot);
                    cv::setMouseCallback("Snapshot", on_mouse_click, &snapshot);
                    run_once = false;
                }
                snapshot = cv::Mat(openCvImage.size(), CV_8UC3, cv::Scalar(0,0,0));

                // Display Image
//                namedWindow("Basler IP Cam Display");
                imshow("OpenCV Display", openCvImage);
                keyVal = cv::waitKey(1) & 0xFF;

                // Take snapshot if needed be
                if (keyVal == 116) {
                    snapshot = openCvImage.clone();
                    cv::imshow("Snapshot", snapshot);
                    std::string my_file = "test.csv";
                    writeCSV(my_file, snapshot);

                }



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