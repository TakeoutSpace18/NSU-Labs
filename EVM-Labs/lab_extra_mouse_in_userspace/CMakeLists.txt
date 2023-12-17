cmake_minimum_required(VERSION 3.26)
project(lab_extra_mouse_in_userspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab_extra_mouse_in_userspace
        src/mouse_in_userspace.cpp
)

target_include_directories(lab_extra_mouse_in_userspace PRIVATE src)

target_link_libraries(lab_extra_mouse_in_userspace PRIVATE usb-1.0)
target_compile_options(lab_extra_mouse_in_userspace PRIVATE -Wall -Wpedantic)