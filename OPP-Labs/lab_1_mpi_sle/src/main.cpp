#include "SLESolverMPI.h"

int main(int argc, char** argv)
{
    SLESolverMPI solver;
    return solver.measureTimeWithRepeats(argc, argv);
}