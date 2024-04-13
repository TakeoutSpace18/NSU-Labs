#include "DESolverMPI.h"

#include <vector>
#include <iostream>
#include <fstream>

#include <mpi.h>

DESolverMPI::DESolverMPI(const MPI::Comm &comm) : mCommWorld(comm)
{
    mWorldSize = mCommWorld.Get_size();
    mProcRank = mCommWorld.Get_rank();

    int procNameLen;
    char procName[256];
    MPI::Get_processor_name(procName, procNameLen);
    std::cout << "Hello from " << procName << "\n";
}

std::vector<DESolverMPI::ValueType> DESolverMPI::CreateInitialData(
    std::array<int, 3> gridSize,
    ValueType borderValue,
    ValueType innerValue)
{
    int Nx = gridSize[0];
    int Ny = gridSize[1];
    int Nz = gridSize[2];

    std::vector<ValueType> initialData(Nx * Ny * Nz, innerValue);
    for (int x = 0; x < Nx; ++x) {
        for (int y = 0; y < Ny; ++y) {
            initialData[x * Ny * Nz + y * Nz] = borderValue;
            initialData[COORDS(x, y, 0, gridSize)] = borderValue;
            initialData[COORDS(x, y, Nz - 1, gridSize)] = borderValue;
        }
    }

    for (int x = 0; x < Nx; ++x) {
        for (int z = 0; z < Nz; ++z) {
            initialData[COORDS(x, 0, z, gridSize)] = borderValue;
            initialData[COORDS(x, Ny - 1, z, gridSize)] = borderValue;
        }
    }

    for (int y = 0; y < Ny; ++y) {
        for (int z = 0; z < Nz; ++z) {
            initialData[COORDS(0, y, z, gridSize)] = borderValue;
            initialData[COORDS(Nx - 1, y, z, gridSize)] = borderValue;
        }
    }

    return initialData;
}

DESolverMPI::ValueType DESolverMPI::ExaminePrecision(std::function<ValueType(int x, int y, int z)> referenceFunc,
                                                     const std::vector<ValueType> &data, SolutionProperties properties)
{
    throw std::runtime_error("Not implemented yet");
}

std::vector<DESolverMPI::ValueType> DESolverMPI::Solve(
    NextIterationCallback fnCallback,
    const std::vector<ValueType> &initialData,
    SolutionProperties properties,
    ValueType eps)
{
    throw std::runtime_error("Not implemented yet");
}