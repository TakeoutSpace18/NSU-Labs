#include "DESolverMPI.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <mpi.h>
#include <fmt/ranges.h>


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
    // TODO: paralellize this too
    std::array stepSize = GetStepSize(properties);
    ValueType precision = 0;
    for (int x = 0; x < properties.gridSize[0]; ++x) {
        for (int y = 0; y < properties.gridSize[1]; ++y) {
            for (int z = 0; z < properties.gridSize[2]; ++z) {
            }
        }
    }
}

std::array<DESolverMPI::ValueType, 3> DESolverMPI::GetStepSize(const SolutionProperties &properties)
{
    double hx = properties.areaSize[0] / properties.gridSize[0];
    double hy = properties.areaSize[1] / properties.gridSize[1];
    double hz = properties.areaSize[2] / properties.gridSize[2];
    return {hx, hy, hz};
}

DESolverMPI::SplitInfo DESolverMPI::GenerateSplitInfo(std::array<int, 3> gridSize)
{
    SplitInfo splitInfo;
    splitInfo.sizes.reserve(mWorldSize);
    splitInfo.offsets.reserve(mWorldSize);

    int chunkLines = gridSize[0] / mWorldSize;
    int remainingLines = gridSize[0] % mWorldSize;

    int curOffset = 0;
    for (int i = 0; i < remainingLines; ++i) {
        int curSize = chunkLines + 1;
        splitInfo.sizes.push_back(curSize);
        splitInfo.offsets.push_back(curOffset);
        curOffset += curSize;
    }

    for (int i = remainingLines; i < mWorldSize; ++i) {
        int curSize = chunkLines;
        splitInfo.sizes.push_back(curSize);
        splitInfo.offsets.push_back(curOffset);
        curOffset += curSize;
    }

    return splitInfo;
}

void DESolverMPI::PrintDebugInfo(DESolverMPI::SolutionProperties properties, DESolverMPI::SplitInfo splitInfo, std::vector<DESolverMPI::ValueType> extendedChunk, int Ny, int Nz)
{
    fmt::print("Rank: {}, splitSizes: {}, splitOffsets: {}", mProcRank, splitInfo.sizes, splitInfo.offsets);

    std::stringstream ss;
    ss << "Rank: " << mProcRank << "\n";
    ss << "neighbour top:\n";
    for (int y = 0; y < Ny; ++y) {
        for (int z = 0; z < Nz; ++z) {
            ss << extendedChunk[COORDS(0, y, z, properties.gridSize)] << " ";
        }
        ss << std::endl;
    }

    ss << "\nmy data:\n";
    for (int x = 1; x < splitInfo.sizes[mProcRank] + 1; ++x) {
        for (int y = 0; y < Ny; ++y) {
            for (int z = 0; z < Nz; ++z) {
                ss << extendedChunk[COORDS(x, y, z, properties.gridSize)] << " ";
            }
            ss << std::endl;
        }
        ss << std::endl;
        ss << std::endl;
    }

    ss << "neighbour bottom:\n";
    for (int y = 0; y < Ny; ++y) {
        for (int z = 0; z < Nz; ++z) {
            ss << extendedChunk[COORDS(splitInfo.sizes[mProcRank] + 1, y, z, properties.gridSize)] << " ";
        }
        ss << std::endl;
    }
    ss << std::endl;

    std::cout << ss.str();
}

void DESolverMPI::SetInitialValues(
    ValueType *chunk,
    ValueType borderValue,
    ValueType innerValue,
    std::array<int, 3> gridSize,
    int xStart, int xEnd)
{
    int Nx = gridSize[0];
    int Ny = gridSize[1];
    int Nz = gridSize[2];

    for (int x = xStart; x < xEnd; ++x) {
        for (int y = 0; y < Ny; ++y) {
            for (int z = 0; z < Nz; ++z) {
                ValueType value;
                if (x == 0 || x == Nx - 1 || y == 0 || y == Ny - 1 || z == 0 || z == Nz - 1) {
                    value = borderValue;
                } else {
                    value = innerValue;
                }
                chunk[COORDS(x - xStart, y, z, gridSize)] = value;
            }
        }
    }
}

DESolverMPI::ValueType DESolverMPI::UpdatePlane(int x, int offset, const ValueType *srcChunk, ValueType *dstChunk,
                                                NextIterationCallback fn)
{
    ValueType precision = 0;

    for (int y = 1; y < mGridSize[1] - 1; ++y) {
        for (int z = 1; z < mGridSize[2] - 1; ++z) {
            int idx = COORDS(x - offset, y, z, mGridSize);
            dstChunk[idx] = fn(mStepSize, x - offset, y, z, srcChunk, mGridSize);

            ValueType diff = std::abs(srcChunk[idx] - dstChunk[idx]);
            precision = std::max(precision, diff);
        }
    }

    return precision;
}

std::vector<DESolverMPI::ValueType> DESolverMPI::Solve(
    NextIterationCallback fnCallback,
    ValueType borderValue,
    ValueType innerValue,
    SolutionProperties properties,
    ValueType eps)
{
    mGridSize = properties.gridSize;

    SplitInfo splitInfo = GenerateSplitInfo(properties.gridSize);
    int xStart = splitInfo.offsets[mProcRank];
    int xEnd = splitInfo.offsets[mProcRank] + splitInfo.sizes[mProcRank];

    int planeSize = properties.gridSize[1] * properties.gridSize[2];
    int extendedChunkSize = planeSize * (splitInfo.sizes[mProcRank] + 2);

    std::vector<ValueType> extendedChunk(extendedChunkSize);
    ValueType *myDataChunk = &*(extendedChunk.begin() + planeSize);

    std::vector<ValueType> extendedChunkSecond(extendedChunkSize);
    ValueType* myDataChunkSecond = &*(extendedChunkSecond.begin() + planeSize);

    SetInitialValues(myDataChunk, borderValue, innerValue, properties.gridSize, xStart, xEnd);

    int iterCount = 0;
    while (true) {
        iterCount++;

        MPI::Request topPlaneSend, topPlaneRecieve, bottomPlaneSend, bottomPlaneRecieve;

        if (mProcRank != 0) {
            ValueType *myTopPlane = &*(extendedChunk.begin() + planeSize);
            topPlaneSend = mCommWorld.Isend(myTopPlane, planeSize, MPIValueType, mProcRank - 1, 0);

            ValueType *neighbourTopPlane = &*(extendedChunk.begin());
            topPlaneRecieve = mCommWorld.Irecv(neighbourTopPlane, planeSize, MPIValueType, mProcRank - 1, MPI::ANY_TAG);
        }
        if (mProcRank != mWorldSize - 1) {
            ValueType *myBottomPlane = &*(extendedChunk.end() - 2 * planeSize);
            bottomPlaneSend = mCommWorld.Isend(myBottomPlane, planeSize, MPIValueType, mProcRank + 1, 0);

            ValueType *neighbourBottomPlane = &*(extendedChunk.end() - planeSize);
            bottomPlaneRecieve = mCommWorld.Irecv(neighbourBottomPlane, planeSize, MPIValueType, mProcRank + 1, MPI::ANY_TAG);
        }

        mStepSize = GetStepSize(properties);

        fmt::print("Proc: {}, xStart: {}, xEnd: {}\n", mProcRank, xStart, xEnd);
        // Calculate inner values
        ValueType localPrecision = 0;
        for (int x = xStart + 1; x < xEnd - 1; ++x) {
                ValueType p = UpdatePlane(x, xStart, myDataChunk, myDataChunkSecond, fnCallback);
                localPrecision = std::max(p, localPrecision);
        }

        fmt::print("Proc: {}, inner part updated\n", mProcRank);

        // TODO: maybe use conditional variable?
        while (!topPlaneRecieve.Test() || !bottomPlaneRecieve.Test()) {
            // do nothing
        }

        fmt::print("Proc: {}, smth recieved\n", mProcRank);

        if (topPlaneRecieve.Test() && mProcRank != 0) {
            fmt::print("Proc: {}, top recieved first\n", mProcRank);
            if (mProcRank != 0) {
                ValueType p = UpdatePlane(xStart, xStart, myDataChunk, myDataChunkSecond, fnCallback);
                localPrecision = std::max(p, localPrecision);
            }


            if (mProcRank != mWorldSize - 1) {
                bottomPlaneRecieve.Wait();
                fmt::print("Proc: {}, bottom recieved\n", mProcRank);
                ValueType p = UpdatePlane(xEnd - 1, xStart, myDataChunk, myDataChunkSecond, fnCallback);
                localPrecision = std::max(p, localPrecision);
            }

        } else {
            fmt::print("Proc: {}, bottom recieved first\n", mProcRank);
            if (mProcRank != mWorldSize - 1) {
                ValueType p = UpdatePlane(xEnd - 1, xStart, myDataChunk, myDataChunkSecond, fnCallback);
                localPrecision = std::max(p, localPrecision);
            }


            if (mProcRank != 0) {
                topPlaneRecieve.Wait();
                fmt::print("Proc: {}, top recieved\n", mProcRank);
                ValueType p = UpdatePlane(xStart, xStart, myDataChunk, myDataChunkSecond, fnCallback);
                localPrecision = std::max(p, localPrecision);
            }
        }
        std::cout << "Hello " + std::to_string(mProcRank) + "\n";

        ValueType globalPrecision;
        mCommWorld.Allreduce(&localPrecision, &globalPrecision, 1, MPIValueType, MPI::MAX);

        std::swap(extendedChunk, extendedChunkSecond);

        if (mProcRank == 0) {
            std::cout << "Iteration: " << iterCount << "; precision: " << globalPrecision << "\n";
        }

        if (globalPrecision < eps) {
            break;
        }
    }

    // PrintDebugInfo(properties, splitInfo, extendedChunk, mGridSize[1], mGridSize[2]);

    return {};
}
