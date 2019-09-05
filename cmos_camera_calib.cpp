/*
 * Good Resources for camera Calibration
 * http://www.cs.cmu.edu/~16385/s17/Slides/11.1_Camera_matrix.pdf
 * Entire slides: http://www.cs.cmu.edu/~16385/s17/Slides/
 * */
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/calib3d/calib3d.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/core/types_c.h>
#include <opencv4/opencv2/calib3d/calib3d_c.h>
#include <opencv4/opencv2/core/core_c.h>
#include <pylon/PylonIncludes.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>

using namespace std;
using namespace cv;
using namespace Pylon;

vector<vector<Point3f> > object_points;
vector<vector<Point2f> > image_points;
vector<Point2f> corners;
vector<vector<Point2f> > left_img_points;

Mat img, gray, opencvimage;
Size im_size;

bool doesExist(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void setup_calibration(int board_width, int board_height,
                       float square_size) {
    Size board_size = Size(board_width, board_height);
    int board_n = board_width * board_height;
    // The exit code of the sample application.
    int exitCode = 0;
    // Before using any pylon methods, the pylon runtime must be initialized.
    PylonInitialize();
    try {
        CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
        camera.Open();
        GenApi::INodeMap &nodemap = camera.GetNodeMap();
        GenApi::CIntegerPtr width = nodemap.GetNode("Width");
        GenApi::CIntegerPtr height = nodemap.GetNode("Height");
        CEnumParameter pixelFormat(nodemap, "PixelFormat");
        pixelFormat.SetValue("Mono12");
        camera.StartGrabbing(GrabStrategy_LatestImageOnly);
        CGrabResultPtr ptrGrabResult;
        int max_images {0};
        while (camera.IsGrabbing()) {
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if (ptrGrabResult->GrabSucceeded()) {
                opencvimage = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),
                          CV_16UC1, (uint32_t *) ptrGrabResult->GetBuffer());
                opencvimage.convertTo(img, CV_8U, 0.5);
                bool found = false;
                found = cv::findChessboardCorners(img, board_size, corners,
                                                  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
                if (found) {
                    cornerSubPix(gray, corners, cv::Size(5, 5), cv::Size(-1, -1),
                                 TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
                    drawChessboardCorners(gray, board_size, corners, found);
                }

                vector<Point3f> obj;
                for (int i = 0; i < board_height; i++)
                    for (int j = 0; j < board_width; j++)
                        obj.push_back(Point3f((float) j * square_size, (float) i * square_size, 0));

                if (found) {
                    cout << ". Found corners!" << endl;
                    image_points.push_back(corners);
                    object_points.push_back(obj);
                    max_images++;
                    cout << "i = " << max_images << endl;
                    if (max_images > 1000){break;}
                }



            }

        }
    }

    catch (const GenericException &e) {
        // Error handling
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
        exitCode = 1;
    }
    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press Enter to exit." << endl;
    while (cin.get() != '\n');
    // Releases all pylon resources.
    PylonTerminate();


//    Original Code
//    for (int k = 1; k <= num_imgs; k++) {
//        char img_file[100];
//        sprintf(img_file, "%s%s%d.%s", imgs_directory, imgs_filename, k, extension);
//        if(!doesExist(img_file))
//            continue;
//        img = imread(img_file, CV_LOAD_IMAGE_COLOR);
//        cv::cvtColor(img, gray, CV_BGR2GRAY);
//
//        bool found = false;
//        found = cv::findChessboardCorners(img, board_size, corners,
//                                          CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
//        if (found)
//        {
//            cornerSubPix(gray, corners, cv::Size(5, 5), cv::Size(-1, -1),
//                         TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
//            drawChessboardCorners(gray, board_size, corners, found);
//        }
//
//        vector< Point3f > obj;
//        for (int i = 0; i < board_height; i++)
//            for (int j = 0; j < board_width; j++)
//                obj.push_back(Point3f((float)j * square_size, (float)i * square_size, 0));
//
//        if (found) {
//            cout << k << ". Found corners!" << endl;
//            image_points.push_back(corners);
//            object_points.push_back(obj);
//        }
//    }
}

double computeReprojectionErrors(const vector<vector<Point3f> > &objectPoints,
                                 const vector<vector<Point2f> > &imagePoints,
                                 const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                                 const Mat &cameraMatrix, const Mat &distCoeffs) {
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    vector<float> perViewErrors;
    perViewErrors.resize(objectPoints.size());

    for (i = 0; i < (int) objectPoints.size(); ++i) {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
                      distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);
        int n = (int) objectPoints[i].size();
        perViewErrors[i] = (float) std::sqrt(err * err / n);
        totalErr += err * err;
        totalPoints += n;
    }
    return std::sqrt(totalErr / totalPoints);
}

int main(int argc, char const **argv) {
    int board_width, board_height, num_imgs;
    float square_size;
    const char *out_file = "camera_properties.txt";
    board_width = 7;
    board_height = 9;
    square_size = 20;

    setup_calibration(board_width, board_height, square_size);

    printf("Starting Calibration\n");
    Mat K;
    Mat D;
    vector<Mat> rvecs, tvecs;
    int flag = 0;
    flag |= CV_CALIB_FIX_K4;
    flag |= CV_CALIB_FIX_K5;
    calibrateCamera(object_points, image_points, img.size(), K, D, rvecs, tvecs, flag);

    cout << "Calibration error: " << computeReprojectionErrors(object_points, image_points, rvecs, tvecs, K, D) << endl;

    FileStorage fs(out_file, FileStorage::WRITE);
    fs << "K" << K;
    fs << "D" << D;
    fs << "board_width" << board_width;
    fs << "board_height" << board_height;
    fs << "square_size" << square_size;
    printf("Done Calibration\n");

    return 0;
}


