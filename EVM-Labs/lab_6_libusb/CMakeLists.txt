cmake_minimum_required(VERSION 3.26)
project(lab_6_libusb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab_6_libusb
        src/main.cpp
)

target_include_directories(lab_6_libusb PRIVATE src)

target_link_libraries(lab_6_libusb PRIVATE usb-1.0 udev)
target_compile_options(lab_6_libusb PRIVATE -Wall -Wpedantic)