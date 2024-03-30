#include <iostream>

#include "MatrixMultiplierMPI.h"

int main(int argc, char** argv)
{
    if (argc < 7)
    {
        std::cout << "usage:\n\t<matA> <matB> <matResult> <n1> <n2> <n3>\n";
        return EXIT_FAILURE;
    }

    MatrixMultiplierMPI multiplier;
    multiplier.MultiplyFromFiles(argv[1], argv[2], argv[3],
                                 std::atoi(argv[4]), std::atoi(argv[5]), std::atoi(argv[6]));

    return EXIT_FAILURE;
}
