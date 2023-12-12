cmake_minimum_required(VERSION 3.14)
project(lab_7_intrinsics
    VERSION 1.0.0
    DESCRIPTION "Template for C++ library built with CMake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -O3")

add_executable(lab_7_intrinsics)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(GTest REQUIRED)

target_sources(lab_7_intrinsics PRIVATE test/inversion_test.cpp src/naive_impl.cpp src/simd_impl.cpp)
target_include_directories(lab_7_intrinsics PRIVATE src)
target_compile_options(lab_7_intrinsics PRIVATE -O2 -mavx512f -mavx512vl -mfma)

source_group(TREE "${CMAKE_CURRENT_SOURCEO_DIR}" FILES ${sources})

target_link_libraries(lab_7_intrinsics Eigen3::Eigen gtest gtest_main)

gtest_discover_tests(lab_7_intrinsics)
enable_testing()
