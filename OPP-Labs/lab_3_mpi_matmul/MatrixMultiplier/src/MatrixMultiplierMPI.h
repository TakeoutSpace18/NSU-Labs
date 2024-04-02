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

    explicit MatrixMultiplierMPI(const MPI::Comm& comm);

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
    void CreateTopology(int n1, int n2, int n3, std::array<int, 2> gridSize);

    void ValidateGridSize(int n1, int n2, int n3, std::array<int, 2> gridSize);

    std::vector<ValueType> RegularMatMultiply(const std::vector<ValueType> &matA,
                       const std::vector<ValueType> &matB,
                       int n1, int n2, int n3);

    std::vector<ValueType> DistributeMatARows(const std::vector<ValueType>& matA, int n1, int n2, int n3);

    std::vector<ValueType> DistributeMatBCols(const std::vector<ValueType>& matB, int n1, int n2, int n3);

    void GatherOutputMatrix(const std::vector<ValueType>& blockLocal,
        std::vector<ValueType>& matOutput,
        std::array<int, 2> gridSize,
        int n1, int n2, int n3);

private:
    const MPI::Comm& mCommWorld;
    int mProcRank;
    int mWorldSize;
    std::array<int, 2> mProcCoords;
    MPI::Cartcomm mRowComm;
    MPI::Cartcomm mColComm;
    std::array<int, 2> mChunkSize;
};


#endif //MATMULTIPLYERMPI_H
