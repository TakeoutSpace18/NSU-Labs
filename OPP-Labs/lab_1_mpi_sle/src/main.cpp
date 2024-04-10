#include <iostream>
#include <fstream>

#include "SLESolverMPI.h"

int main(int argc, char** argv)
{
    MPI::Init();

    SLESolverMPI solver(MPI::COMM_WORLD);

    double beginTime = MPI::Wtime();
    int status = solver.compute();
    double endTime = MPI::Wtime();

    double timeElapsed = endTime - beginTime;
    double maxTimeElapsed;
    MPI::COMM_WORLD.Reduce(&timeElapsed, &maxTimeElapsed, 1, MPI::DOUBLE, MPI::MAX, 0);

    int rank = MPI::COMM_WORLD.Get_rank();
    if (rank == 0) {
        std::cout << "Computation took " << maxTimeElapsed << " seconds\n";

        std::ofstream out("timeElapsed.txt");
        out << maxTimeElapsed;
    }

    MPI::Finalize();
    return status;
}