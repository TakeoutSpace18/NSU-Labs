find_package(MPI)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    message("MPI_INCLUDE_PATH :${MPI_INCLUDE_PATH}")
endif (MPI_FOUND)

find_package(fmt)

add_executable(DESolver src/main.cpp src/DESolverMPI.cpp src/DESolverMPI.h)

target_include_directories(DESolver PRIVATE ../Utility)
target_include_directories(DESolver PRIVATE src)
target_compile_options(DESolver PRIVATE -Wall -Wextra -Wpedantic -O2)

target_link_libraries(DESolver MPI::MPI_CXX fmt::fmt)