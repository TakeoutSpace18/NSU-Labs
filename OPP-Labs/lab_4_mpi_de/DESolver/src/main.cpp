#include <cmath>
#include <iostream>
#include <fstream>
#include <mpi.h>

#include "DESolverMPI.h"

constexpr double EPS = 1e-4;

int main(int argc, char **argv)
{
    MPI::Init();
    int rank = MPI::COMM_WORLD.Get_rank();
    DESolverMPI solver(MPI::COMM_WORLD);

    DESolverMPI::NextIterationCallback func = [](
            const std::array<double, 3> &stepSize,
            int i, int j, int k,
            const DESolverMPI::ValueType* data,
            std::array<int, 3>& gridSize) -> double
    {
        double hx_2 = std::pow(stepSize[0], 2);
        double hy_2 = std::pow(stepSize[1], 2);
        double hz_2 = std::pow(stepSize[2], 2);

        double a = 1e5;

        return (1 / (2 / hx_2 + 2 / hy_2 + 2 / hz_2 + a)) * (
                   (data[COORDS(i+1, j, k, gridSize)] + data[COORDS(i-1, j, k, gridSize)]) / hx_2 +
                   (data[COORDS(i, j+1, k, gridSize)] + data[COORDS(i, j-1, k, gridSize)]) / hy_2 +
                   (data[COORDS(i, j, k+1, gridSize)] + data[COORDS(i, j, k-1, gridSize)]) / hz_2
                   - 6 + a * data[COORDS(i, j, k, gridSize)]);
    };

    DESolverMPI::SolutionProperties properties{
        .areaStart = {-1, -1, -1},
        .areaSize = {2, 2, 2},
        .gridSize = {100, 100, 100}
    };

    double beginTime = MPI::Wtime();
    std::vector result = solver.Solve(func, 3, 0, properties, EPS);
    double endTime = MPI::Wtime();

    double timeElapsed = endTime - beginTime;
    double maxTimeElapsed;
    MPI::COMM_WORLD.Reduce(&timeElapsed, &maxTimeElapsed, 1, MPI::DOUBLE, MPI::MAX, 0);

    // double precision = solver.ExaminePrecision([](int x, int y, int z) {
    //     return x * x + y * y + z * z;
    // }, result, properties);

    if (rank == 0) {
        std::cout << "Computation took " << maxTimeElapsed << " seconds\n";
        // std::cout << "Presicion: " << precision << "\n";

        std::ofstream out("timeElapsed.txt");
        out << maxTimeElapsed;
    }

    MPI::Finalize();
    return EXIT_SUCCESS;
}
