cmake_minimum_required(VERSION 3.27)
project(lab_5_opencv)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV CONFIG REQUIRED)

add_executable(lab_5_opencv src/main.cpp
        src/CameraDemo.cpp
        src/CameraDemo.h)

target_link_libraries(lab_5_opencv PRIVATE
        opencv_core
        opencv_highgui
        opencv_dnn_superres
        opencv_video
)