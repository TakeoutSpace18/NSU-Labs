#ifndef CAMERADEMO_H
#define CAMERADEMO_H
#include <opencv2/videoio.hpp>


class CameraDemo {
public:
    int launch();

private:
    int main_loop();

    bool is_running_ = true;
    uint32_t fps_ = 0;
    std::string window_name_ = "WebCam Demo";
    cv::VideoCapture video_capture_;
};



#endif //CAMERADEMO_H
