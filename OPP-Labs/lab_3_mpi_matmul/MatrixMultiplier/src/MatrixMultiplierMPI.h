#ifndef MATMULTIPLYERMPI_H
#define MATMULTIPLYERMPI_H
#include <array>
#include <string>
#include <vector>

#include <mpi.h>

class MatrixMultiplierMPI {
public:
    using ValueType = float;
    MPI::Datatype MPIValueType = MPI::FLOAT;

    MatrixMultiplierMPI();

    ~MatrixMultiplierMPI();

    void MultiplyFromFiles(
        const std::string &matAFilename,
        const std::string &matBFilename,
        const std::string &outputFilename,
        int n1, int n2, int n3,
        std::array<int, 2> gridSize);

    void Multiply(
        const std::vector<ValueType> &matA,
        const std::vector<ValueType> &matB,
        std::vector<ValueType> &matOutput,
        int n1, int n2, int n3,
        std::array<int, 2> gridSize);

private:
    void ValidateGridSize(int n1, int n2, int n3, std::array<int, 2> gridSize);

    void MultiplySingleProcess(const std::vector<ValueType> &matA,
                       const std::vector<ValueType> &matB,
                       std::vector<ValueType> &matOutput,
                       int n1, int n2, int n3);

private:
    int mProcRank;
    int mWorldSize;
};


#endif //MATMULTIPLYERMPI_H
