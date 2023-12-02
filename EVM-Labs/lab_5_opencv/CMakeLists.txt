cmake_minimum_required(VERSION 3.27)
project(lab_5_opencv)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH "/home/pavel/repos/build")
find_package(OpenCV CONFIG REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(lab_5_opencv src/main.cpp
        src/CameraDemo.cpp
        src/CameraDemo.h)
target_link_libraries(lab_5_opencv PRIVATE
        opencv_core
        opencv_highgui
        opencv_ml
        opencv_dnn
        opencv_dnn_superres
        opencv_core
        opencv_flann
        opencv_video)