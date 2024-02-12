cmake_minimum_required(VERSION 3.14)
project(lab_2_openmp_sle
    VERSION 1.0.0
    DESCRIPTION "Template for C++ library built with CMake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
find_package(Boost 1.40.0 COMPONENTS filesystem system iostreams REQUIRED)

file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(lab_2_openmp_sle)

target_sources(lab_2_openmp_sle PRIVATE ${SOURCES})
target_include_directories(lab_2_openmp_sle PRIVATE src)
target_compile_options(lab_2_openmp_sle PRIVATE -Wall -Wextra -Wpedantic -O2)

source_group(TREE "${CMAKE_CURRENT_SOURCEO_DIR}" FILES ${sources})

target_link_libraries(lab_2_openmp_sle OpenMP::OpenMP_CXX ${Boost_LIBRARIES})