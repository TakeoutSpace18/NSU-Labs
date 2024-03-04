cmake_minimum_required(VERSION 3.26)
project(lab_0b_word_counter)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(lab_0b_word_counter
        src/WordCounter.cpp
        src/WordCounter.h
        src/main.cpp)

target_compile_options(lab_0b_word_counter PRIVATE -Wall -Wpedantic)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/af5ddbde4d57b0331e1d0ed3659ca2e39b37580d.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)