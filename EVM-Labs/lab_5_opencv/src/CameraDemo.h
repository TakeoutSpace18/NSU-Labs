#ifndef CAMERADEMO_H
#define CAMERADEMO_H
#include <opencv2/dnn_superres.hpp>
#include <opencv2/videoio.hpp>


class CameraDemo {
public:
    int Launch();

private:
    cv::Mat CaptureFrame();

    void ShowFrame(const cv::Mat& frame);

    cv::Mat ProcessFrame(cv::Mat frame);

    void HandleEvents();

    int MainLoop();

    bool is_running_ = true;
    double fps_ = 0;
    std::string window_name_ = "WebCam Demo";
    cv::VideoCapture video_capture_;
    cv::dnn_superres::DnnSuperResImpl sr_;
};



#endif //CAMERADEMO_H
