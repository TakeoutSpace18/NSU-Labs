cmake_minimum_required(VERSION 3.14)
project(lab_3_mpi_matmul
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(MultiplicationTester)
add_subdirectory(MatrixMultiplier)
