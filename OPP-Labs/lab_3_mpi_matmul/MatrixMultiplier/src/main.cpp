#include <iostream>

#include "MatrixMultiplierMPI.h"

int main(int argc, char** argv)
{
    if (argc < 7)
    {
        std::cout << "usage:\n\t<matA> <matB> <matResult> <n1> <n2> <n3> <gridRows> <gridCols>\n";
        return EXIT_FAILURE;
    }

    MPI::Init();

    MatrixMultiplierMPI multiplier(MPI::COMM_WORLD);
    std::array gridSize = {std::atoi(argv[7]), std::atoi(argv[8])};
    multiplier.MultiplyFromFiles(argv[1], argv[2], argv[3],
                                 std::atoi(argv[4]), std::atoi(argv[5]), std::atoi(argv[6]), gridSize);

    MPI::Finalize();
    return EXIT_FAILURE;
}
