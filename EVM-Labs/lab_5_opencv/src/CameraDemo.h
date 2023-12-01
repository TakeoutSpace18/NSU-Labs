#ifndef CAMERADEMO_H
#define CAMERADEMO_H
#include <opencv2/videoio.hpp>


class CameraDemo {
public:
    int Launch();


private:
    void CaptureFrame();

    void ShowFrame();

    void ProcessFrame();

    void HandleEvents();

    int MainLoop();

    bool is_running_ = true;
    uint32_t fps_ = 0;
    std::string window_name_ = "WebCam Demo";
    cv::VideoCapture video_capture_;
    cv::Mat frame_;
};



#endif //CAMERADEMO_H
