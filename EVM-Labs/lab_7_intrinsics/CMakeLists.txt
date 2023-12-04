cmake_minimum_required(VERSION 3.14)
project(lab_7_intrinsics
    VERSION 1.0.0
    DESCRIPTION "Template for C++ library built with CMake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -O3")

add_executable(InversionTest)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(GTest REQUIRED)

target_sources(InversionTest PRIVATE test/inversion_test.cpp src/naive_impl.cpp)
target_include_directories(InversionTest PRIVATE src)

source_group(TREE "${CMAKE_CURRENT_SOURCEO_DIR}" FILES ${sources})

target_link_libraries(InversionTest Eigen3::Eigen gtest gtest_main)

add_test(NAME InversionTest
	 COMMAND InversionTest)
enable_testing()
