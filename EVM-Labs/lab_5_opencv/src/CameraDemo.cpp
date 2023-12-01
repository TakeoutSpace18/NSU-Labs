#include "CameraDemo.h"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int CameraDemo::Launch() {
    video_capture_.open(0);
    if (!video_capture_.isOpened()) {
        std::cout << "***Could not initialize capturing...***\n";
        return EXIT_FAILURE;
    }
    cv::namedWindow(window_name_, 0);
    is_running_ = true;

    return MainLoop();
}

void CameraDemo::CaptureFrame() {
    video_capture_ >> frame_;
}

void CameraDemo::ShowFrame() {
    cv::imshow(window_name_, frame_);
}

void CameraDemo::ProcessFrame() {
    cv::putText(frame_, "FPS: " + std::to_string(fps_), {5, 25}, cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,255,0));
}

void CameraDemo::HandleEvents() {
    switch (cv::waitKey(1)) {
        case 'q':
            is_running_ = false;
            break;
    }
}

int CameraDemo::MainLoop() {
    static constexpr uint32_t n_frames_update_fps = 60;

    uint32_t frames_since_last_time_measure = 0;
    auto time_at_last_measure = std::chrono::high_resolution_clock::now();

    while (is_running_) {
        ++frames_since_last_time_measure;


        CaptureFrame();
        ProcessFrame();
        ShowFrame();

        // HandleEvents();

        if (frames_since_last_time_measure >= n_frames_update_fps) {
            auto time_now = std::chrono::high_resolution_clock::now();
            auto time_elapsed = time_now - time_at_last_measure;
            fps_ = frames_since_last_time_measure * 10000 / std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed).count();


            time_at_last_measure = time_now;
            frames_since_last_time_measure = 0;
        }

    }

    return EXIT_SUCCESS;
}
