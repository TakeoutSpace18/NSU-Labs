#include "CameraDemo.h"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn_superres.hpp>

int CameraDemo::Launch() {
    video_capture_.open(0);
    if (!video_capture_.isOpened()) {
        std::cout << "***Could not initialize capturing...***\n";
        return EXIT_FAILURE;
    }
    cv::namedWindow(window_name_, 0);
    cv::setWindowProperty(window_name_, cv::WindowPropertyFlags::WND_PROP_AUTOSIZE, cv::WindowFlags::WINDOW_AUTOSIZE);
    is_running_ = true;

    sr_.readModel("../LapSRN_x8.pb");
    sr_.setModel("lapsrn", 8);

    return MainLoop();
}

cv::Mat CameraDemo::CaptureFrame() {
    cv::Mat frame;
    video_capture_ >> frame;
    return frame;
}

void CameraDemo::ShowFrame(const cv::Mat& frame) {
    cv::imshow(window_name_, frame);
}

cv::Mat CameraDemo::ProcessFrame(cv::Mat frame) {
    cv::resize(frame, frame, cv::Size2i{4,3} * 12);
    cv::Mat upscaled_frame;
    sr_.upsample(frame, upscaled_frame);
    cv::resize(frame, frame, upscaled_frame.size(), cv::INTER_LINEAR);
    cv::hconcat(frame,upscaled_frame, frame);
    // cv::resize(frame, frame, frame.size() * 2, cv::INTER_NEAREST_EXACT);

    return frame;
}

void CameraDemo::HandleEvents() {
    switch (cv::waitKey(1)) {
        case 'q':
            is_running_ = false;
            break;
    }
}

int CameraDemo::MainLoop() {
    static constexpr uint32_t n_frames_update_fps = 30;

    uint32_t frames_since_last_time_measure = 0;
    auto time_at_last_measure = std::chrono::high_resolution_clock::now();

    while (is_running_) {
        ++frames_since_last_time_measure;

        cv::Mat frame = CaptureFrame();
        frame = ProcessFrame(frame);
        ShowFrame(frame);

        HandleEvents();

        if (frames_since_last_time_measure >= n_frames_update_fps) {
            auto time_now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_elapsed = time_now - time_at_last_measure;
            fps_ = frames_since_last_time_measure / time_elapsed.count();
            cv::displayOverlay(window_name_, "FPS: " + std::to_string(static_cast<int>(fps_)));

            time_at_last_measure = time_now;
            frames_since_last_time_measure = 0;
        }

    }

    return EXIT_SUCCESS;
}
