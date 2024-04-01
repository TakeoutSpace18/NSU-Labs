#include "MatrixMultiplierMPI.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include <numeric>
#include <ranges>
#include <algorithm>

#include <fmt/ranges.h>

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
    int n1, int n2, int n3,
    std::array<int, 2> gridSize)
{
    std::vector<ValueType> matA, matB, matOutput;

    if (mProcRank == 0)
    {
        matA = LoadVectorFromFile<ValueType>(matAFilename, n1 * n2);
        matB = LoadVectorFromFile<ValueType>(matBFilename, n2 * n3);
        matOutput.resize(n1 * n3);
    }
    Multiply(matA, matB, matOutput, n1, n2, n3, gridSize);

    if (mProcRank == 0)
    {
        SaveVectorToFile(matOutput, outputFilename);
    }
}

void MatrixMultiplierMPI::CreateTopology(int n1, int n2, int n3, std::array<int, 2> gridSize) {
    constexpr std::array periods = {false, false};
    MPI::Cartcomm cartComm = MPI::COMM_WORLD.Create_cart(2, gridSize.data(), periods.data(), true);

    cartComm.Get_coords(mProcRank, 2, mProcCoords.data());

    std::array remainDims = {true, false};
    mColComm = cartComm.Sub(remainDims.data());

    remainDims = {false, true};
    mRowComm = cartComm.Sub(remainDims.data());

    mChunkSize = {n1 / gridSize[0], n3 / gridSize[1]};
}

void MatrixMultiplierMPI::Multiply(
    const std::vector<ValueType>& matA,
    const std::vector<ValueType>& matB,
    std::vector<ValueType>& matOutput,
    int n1, int n2, int n3,
    std::array<int, 2> gridSize)
{
    ValidateGridSize(n1, n2, n3, gridSize);

    CreateTopology(n1, n2, n3, gridSize);

    std::vector rowsLocal = DistributeMatARows(matA, n1, n2, n3);
    std::vector colsLocal = DistributeMatBCols(matB, n1, n2, n3);

    std::vector blockLocal = RegularMatMultiply(rowsLocal, colsLocal, mChunkSize[0], n2, mChunkSize[1]);

    GatherOutputMatrix(blockLocal, matOutput, gridSize, n1, n2, n3);
}

void MatrixMultiplierMPI::ValidateGridSize(int n1, int n2, int n3, std::array<int, 2> gridSize) {
    if ((n1 % gridSize[0] != 0) || (n3 % gridSize[1] != 0))
    {
        throw std::runtime_error("Invalid grid size: n1 should be divisible by gridSize[0], n3 - by gridSize[1]");
    }
    if (gridSize[0] * gridSize[1] != mWorldSize)
    {
        throw std::runtime_error("Invalid grid size: total grid elements doesn't match the world size");
    }
}

std::vector<MatrixMultiplierMPI::ValueType> MatrixMultiplierMPI::RegularMatMultiply(const std::vector<ValueType> &matA,
    const std::vector<ValueType> &matB,
    int n1, int n2, int n3)
{
    std::vector<ValueType> matResult(n1 * n3);

    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            for (int k = 0; k < n3; k++) {
                matResult[i * n3 + k] += matA[i * n2 + j] * matB[j * n3 + k];
            }
        }
    }

    return matResult;
}

std::vector<MatrixMultiplierMPI::ValueType> MatrixMultiplierMPI::DistributeMatARows(
    const std::vector<ValueType> &matA, int n1, int n2, int n3)
{
    std::vector<ValueType> rowsLocal(mChunkSize[0] * n2);
    if (mProcCoords[1] == 0)
    {
        mColComm.Scatter(
            matA.data(),                        // sendbuf
            static_cast<int>(rowsLocal.size()), // sendcount
            MPIValueType,                       // sendtype
            rowsLocal.data(),                   // recvbuf
            static_cast<int>(rowsLocal.size()), // recvcount
            MPIValueType,                       // recvtype
            0                                   // root
            );
    }
    mRowComm.Bcast(
        rowsLocal.data(),                   // buffer
        static_cast<int>(rowsLocal.size()), // count
        MPIValueType,                       // datatype
        0                                   // root
        );

    return rowsLocal;
}

std::vector<MatrixMultiplierMPI::ValueType> MatrixMultiplierMPI::DistributeMatBCols(
    const std::vector<ValueType> &matB, int n1, int n2, int n3)
{
    MPI::Datatype ColumnTypeTmp = MPIValueType.Create_vector(
        n2,             // block count
        mChunkSize[1],  // block length
        n3);            // stride

    MPI::Datatype ColumnType = ColumnTypeTmp.Create_resized(
        0,                                  // lower bound
        mChunkSize[1] * sizeof(ValueType)   // extent
        );
    ColumnType.Commit();

    std::vector<ValueType> colsLocal(mChunkSize[1] * n2);
    if (mProcCoords[0] == 0)
    {
        mRowComm.Scatter(
            matB.data(),        // sendbuf
            1,                  // sendcount
            ColumnType,         // sendtype
            colsLocal.data(),   // recvbuf
            colsLocal.size(),   // recvcount
            MPIValueType,       // recvtype
            0);                 // root
    }
    mColComm.Bcast(
        colsLocal.data(), // buffer
        colsLocal.size(), // count
        MPIValueType,     // datatype
        0                 // root
        );

    ColumnTypeTmp.Free();
    ColumnType.Free();

    return colsLocal;
}

void MatrixMultiplierMPI::GatherOutputMatrix(const std::vector<ValueType> &blockLocal,
    std::vector<ValueType> &matOutput, std::array<int, 2> gridSize, int n1, int n2, int n3)
{
    // Create a type to describe how to place block in the output matrix
    MPI::Datatype BlockTypeTmp = MPIValueType.Create_vector(
        mChunkSize[0], // block count
        mChunkSize[1], // block length
        n3);           // stride

    // Pretend that block has size of one value, and later manually specify the offsets in Gatherv routine
    MPI::Datatype BlockType = BlockTypeTmp.Create_resized(
        0,                  // lower bound
        sizeof(ValueType)   // extent
        );
    BlockType.Commit();

    std::vector sizes(gridSize[0] * gridSize[1], 1);
    std::vector<int> offsets(gridSize[0] * gridSize[1]);

    std::iota(offsets.begin(), offsets.end(), 0);
    std::ranges::transform(offsets, offsets.begin(), [&](int num) {
        int blockRow = num / gridSize[1];
        int blockCol = num % gridSize[1];
        return blockRow * n3 * mChunkSize[0] + blockCol * mChunkSize[1];
    });

    MPI::COMM_WORLD.Gatherv(
        blockLocal.data(),              // sendbuf
        mChunkSize[0] * mChunkSize[1],  // sendcount
        MPIValueType,                   // sendtype
        matOutput.data(),               // recvbuf
        sizes.data(),                   // recvcounts
        offsets.data(),                 // displacements
        BlockType,                      // recvtype
        0);                             // root

    BlockTypeTmp.Free();
    BlockType.Free();
}
