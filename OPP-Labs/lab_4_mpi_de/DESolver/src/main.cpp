#include <cmath>
#include <iostream>
#include <fstream>
#include <mpi.h>

#include "DESolverMPI.h"

constexpr double EPS = 1e-8;
constexpr double a = 1e5;

int main(int argc, char **argv)
{
    MPI::Init();
    int rank = MPI::COMM_WORLD.Get_rank();

    auto phi = [](double x, double y, double z){
        return x*x + y*y + z*z;
    };

    auto ro = [&](double x, double y, double z){
        return 6 - a * phi(x, y, z);
    };

    DESolverMPI::NextIterationCallback nextIterFn = [&](
            const std::array<double, 3> &stepSize,
            double x, double y, double z,
            int i, int j, int k,
            const DESolverMPI::ValueType* data,
            std::array<int, 3>& gridSize) -> double
    {
        double hx_2 = std::pow(stepSize[0], 2);
        double hy_2 = std::pow(stepSize[1], 2);
        double hz_2 = std::pow(stepSize[2], 2);


        return (1 / (2 / hx_2 + 2 / hy_2 + 2 / hz_2 + a)) * (
                   (data[COORDS(i+1, j, k, gridSize)] + data[COORDS(i-1, j, k, gridSize)]) / hx_2 +
                   (data[COORDS(i, j+1, k, gridSize)] + data[COORDS(i, j-1, k, gridSize)]) / hy_2 +
                   (data[COORDS(i, j, k+1, gridSize)] + data[COORDS(i, j, k-1, gridSize)]) / hz_2
                   - ro(x, y, z));
    };

    DESolverMPI::SolutionProperties properties{
        .areaStart = {-1, -1, -1},
        .areaSize = {0.25, 0.25, 0.25},
        .gridSize = {200, 200, 200}
    };

    DESolverMPI solver(MPI::COMM_WORLD, properties);

    double beginTime = MPI::Wtime();
    solver.Solve(nextIterFn, phi, 0, EPS);
    double endTime = MPI::Wtime();

    double timeElapsed = endTime - beginTime;
    double maxTimeElapsed;
    MPI::COMM_WORLD.Reduce(&timeElapsed, &maxTimeElapsed, 1, MPI::DOUBLE, MPI::MAX, 0);

    double maxDelta = solver.GetMaxDelta(phi);

    if (rank == 0) {
        std::cout << "Computation took " << maxTimeElapsed << " seconds\n";
        std::cout << "Max delta: " << maxDelta << "\n";

        std::ofstream out("timeElapsed.txt");
        out << maxTimeElapsed;
    }

    MPI::Finalize();
    return EXIT_SUCCESS;
}
