#include "SLESolverMPI.h"

int main(int argc, char** argv)
{
    MPI::Init();

    SLESolverMPI solver(MPI::COMM_WORLD);
    int status = solver.measureTimeWithRepeats(argc, argv);

    MPI::Finalize();
    return status;
}