find_package(MPI)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    message("MPI_INCLUDE_PATH :${MPI_INCLUDE_PATH}")
endif (MPI_FOUND)

find_package(fmt)

add_executable(MatrixMultiplier src/main.cpp src/MatrixMultiplierMPI.cpp)

target_include_directories(MatrixMultiplier PRIVATE ../Utility)
target_include_directories(MatrixMultiplier PRIVATE src)
target_compile_options(MatrixMultiplier PRIVATE -Wall -Wextra -Wpedantic -O2)

target_link_libraries(MatrixMultiplier MPI::MPI_CXX fmt::fmt)