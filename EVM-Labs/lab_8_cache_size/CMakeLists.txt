cmake_minimum_required(VERSION 3.26)
project(lab_8_cache_size)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab_8_cache_size
        src/main.cpp
        src/CacheSizeMeasure.h
        src/CacheSizeMeasure.cpp
)

target_include_directories(lab_8_cache_size PRIVATE src)
target_compile_options(lab_8_cache_size PRIVATE -O3 -Wall -Wpedantic)