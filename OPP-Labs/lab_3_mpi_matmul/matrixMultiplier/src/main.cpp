#include ""

int main(int argc, char** argv)
{
    MatMultilplyerMPI solver;
    return solver.measureTimeWithRepeats(argc, argv);
}