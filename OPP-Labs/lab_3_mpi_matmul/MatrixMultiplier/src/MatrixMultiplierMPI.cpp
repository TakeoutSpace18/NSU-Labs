#include "MatrixMultiplierMPI.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <mpi.h>

#include "VectorSerialization.hpp"

MatrixMultiplierMPI::MatrixMultiplierMPI()
{
    MPI::Init();
    mWorldSize = MPI::COMM_WORLD.Get_size();
    mProcRank = MPI::COMM_WORLD.Get_rank();

    int procNameLen;
    char procName[256];
    MPI::Get_processor_name(procName, procNameLen);
    std::cout << "Hello from " << procName << "\n";
}

MatrixMultiplierMPI::~MatrixMultiplierMPI()
{
    MPI::Finalize();
}

void MatrixMultiplierMPI::MultiplyFromFiles(
    const std::string& matAFilename,
    const std::string& matBFilename,
    const std::string& outputFilename,
    int n1, int n2, int n3)
{
    std::vector<ValueType> matA, matB, matOutput;

    if (mProcRank == 0)
    {
        matA = LoadVectorFromFile<ValueType>(matAFilename, n1 * n2);
        matB = LoadVectorFromFile<ValueType>(matBFilename, n2 * n3);
        matOutput.resize(n1 * n3);
    }
    Multiply(matA, matB, matOutput, n1, n2, n3);

    if (mProcRank == 0)
    {
        SaveVectorToFile(matOutput, outputFilename);
    }
}

void MatrixMultiplierMPI::Multiply(
    const std::vector<ValueType>& matA,
    const std::vector<ValueType>& matB,
    std::vector<ValueType>& matOutput,
    int n1, int n2, int n3)
{
    if (mProcRank == 0)
    {
        std::ranges::fill(matOutput, 0);

        for (int i = 0; i < n1; i++) {
            for (int j = 0; j < n3; j++) {
                for (int k = 0; k < n2; k++) {
                    matOutput[i * n3 + j] += matA[i * n2 + k] * matB[k * n3 + j];
                }
            }
        }
    }
}
