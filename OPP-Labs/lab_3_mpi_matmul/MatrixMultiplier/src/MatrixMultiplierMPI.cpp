#include "MatrixMultiplierMPI.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <mpi.h>

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

void MatrixMultiplierMPI::Multiply(
    const std::vector<ValueType>& matA,
    const std::vector<ValueType>& matB,
    std::vector<ValueType>& matOutput,
    int n1, int n2, int n3,
    std::array<int, 2> gridSize)
{
    ValidateGridSize(n1, n2, n3, gridSize);

    constexpr std::array periods = {false, false};
    MPI::Cartcomm cartComm = MPI::COMM_WORLD.Create_cart(2, gridSize.data(), periods.data(), true);
    std::array<int, 2> procCoords;
    cartComm.Get_coords(mProcRank, 2, procCoords.data());


    std::array chunkSize = {n1 / gridSize[0], n3 / gridSize[1]};
    if (mProcRank == 0) {
        fmt::print("chunkSize: {}\n", chunkSize);
    }

    std::array remainDims = {true, false};
    MPI::Cartcomm colComm = cartComm.Sub(remainDims.data());

    remainDims = {false, true};
    MPI::Cartcomm rowComm = cartComm.Sub(remainDims.data());


    // TODO: create vector type for rows
    std::vector<ValueType> rowsLocal(chunkSize[0] * n2);
    if (procCoords[1] == 0) {
        colComm.Scatter(matA.data(), rowsLocal.size(), MPIValueType, rowsLocal.data(), rowsLocal.size(), MPIValueType, 0);
    }
    rowComm.Bcast(rowsLocal.data(), rowsLocal.size(), MPIValueType, 0);
    // fmt::print("procCoords: {}, rank: {}\nrowsLocal: {}\n", procCoords, mProcRank, rowsLocal);


    MPI::Datatype ColumnType = MPIValueType.Create_vector(n2, chunkSize[1], n3).Create_resized(0, chunkSize[1] * sizeof(ValueType));
    ColumnType.Commit();

    std::vector<ValueType> colsLocal(chunkSize[1] * n2);
    if (procCoords[0] == 0) {
        rowComm.Scatter(matB.data(), 1, ColumnType, colsLocal.data(), colsLocal.size(), MPIValueType, 0);
    }
    colComm.Bcast(colsLocal.data(), colsLocal.size(), MPIValueType, 0);
    // fmt::print("procCoords: {}, rank: {}\ncolsLocal: {}\n", procCoords, mProcRank, colsLocal);

    std::vector<ValueType> blockLocal(chunkSize[0] * chunkSize[1]);

    MultiplySingleProcess(rowsLocal, colsLocal, blockLocal, chunkSize[0], n2, chunkSize[1]);

    // fmt::print("procCoords: {}, rank: {}\nblockLocal: {}\n", procCoords, mProcRank, blockLocal);


    MPI::Datatype BlockType = MPIValueType.Create_vector(chunkSize[0], chunkSize[1], n3).Create_resized(0, sizeof(ValueType));
    BlockType.Commit();

    std::vector recvCounts(gridSize[0] * gridSize[1], 1);
    std::vector<int> displacements(gridSize[0] * gridSize[1]);

    for (int i = 0; i < gridSize[0]; ++i) {
        for (int j = 0; j < gridSize[1]; ++j) {
            displacements[i * gridSize[1] + j] = i * n3 * chunkSize[0] + j * chunkSize[1];
        }
    }

    MPI::COMM_WORLD.Gatherv(blockLocal.data(), chunkSize[0] * chunkSize[1], MPIValueType, matOutput.data(), recvCounts.data(), displacements.data(), BlockType, 0);
}

void MatrixMultiplierMPI::ValidateGridSize(int n1, int n2, int n3, std::array<int, 2> gridSize) {
    if ((n1 % gridSize[0] != 0) || (n3 % gridSize[1] != 0))
    {
        throw std::runtime_error("Invalid grid size: n1 should be divisible by gridSize[0], n3 - by gridSize[1]");
    }
}

void MatrixMultiplierMPI::MultiplySingleProcess(const std::vector<ValueType> &matA, const std::vector<ValueType> &matB,
    std::vector<ValueType> &matOutput, int n1, int n2, int n3)
{
    std::ranges::fill(matOutput, 0);

    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            for (int k = 0; k < n3; k++) {
                matOutput[i * n3 + k] += matA[i * n2 + j] * matB[j * n3 + k];
            }
        }
    }
}
