#ifndef MATMULTIPLYERMPI_H
#define MATMULTIPLYERMPI_H
#include <string>
#include <vector>

class MatrixMultiplierMPI {
public:
    using ValueType = float;

    MatrixMultiplierMPI();
    ~MatrixMultiplierMPI();

    void MultiplyFromFiles(
        const std::string& matAFilename,
        const std::string& matBFilename,
        const std::string& outputFilename,
        int n1, int n2, int n3);

    void Multiply(
        const std::vector<ValueType>& matA,
        const std::vector<ValueType>& matB,
        std::vector<ValueType>& matOutput,
        int n1, int n2, int n3);

private:

private:
    int mProcRank;
    int mWorldSize;
};


#endif //MATMULTIPLYERMPI_H
