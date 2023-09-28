cmake_minimum_required(VERSION 3.26)
project(lab_1_circular_buffer)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/af5ddbde4d57b0331e1d0ed3659ca2e39b37580d.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(lab_1_circular_buffer
        src/CircularBuffer.cpp
        src/CircularBuffer.h
        src/test/CircularBufferTests.cpp)

target_compile_options(lab_1_circular_buffer PRIVATE -Wall -Wpedantic)
target_link_libraries(lab_1_circular_buffer GTest::gtest_main)
enable_testing()
include(GoogleTest)
gtest_discover_tests(lab_1_circular_buffer)