cmake_minimum_required(VERSION 3.14)
project(lab_1_mpi_sle
    VERSION 1.0.0
    DESCRIPTION "Template for C++ library built with CMake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    message("MPI_INCLUDE_PATH :${MPI_INCLUDE_PATH}")
endif (MPI_FOUND)

add_executable(lab_1_mpi_sle src/main.cpp src/SLESolverMPI.cpp)

target_include_directories(lab_1_mpi_sle PRIVATE src)
target_compile_options(lab_1_mpi_sle PRIVATE -Wall -Wextra -Wpedantic -g)

source_group(TREE "${CMAKE_CURRENT_SOURCEO_DIR}" FILES ${sources})

target_link_libraries(lab_1_mpi_sle MPI::MPI_CXX)