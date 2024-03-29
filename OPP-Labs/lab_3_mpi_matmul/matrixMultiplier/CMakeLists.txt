find_package(MPI)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    message("MPI_INCLUDE_PATH :${MPI_INCLUDE_PATH}")
endif (MPI_FOUND)

add_executable(matrixMultiplier src/main.cpp src/MatrixMultiplierMPI.cpp)

target_include_directories(matrixMultiplier PRIVATE ../utility)
target_include_directories(matrixMultiplier PRIVATE src)
target_compile_options(matrixMultiplier PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(matrixMultiplier MPI::MPI_CXX)