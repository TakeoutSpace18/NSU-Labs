cmake_minimum_required(VERSION 3.26)
project(lab_9_cache_associativity)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab_9_cache_associativity
        src/main.cpp
        src/CacheAssociativityMeasure.h
        src/CacheAssociativityMeasure.cpp
)

target_include_directories(lab_9_cache_associativity PRIVATE src)
target_compile_options(lab_9_cache_associativity PRIVATE -O1 -Wall -Wpedantic)