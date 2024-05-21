#include "DESolverMPI.h"

#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <mpi.h>


DESolverMPI::DESolverMPI(const MPI::Comm& comm,
                         const SolutionProperties& properties)
    : mCommWorld(comm), mProperties(properties)
{
    mWorldSize = mCommWorld.Get_size();
    mProcRank = mCommWorld.Get_rank();

    int procNameLen;
    char procName[256];
    MPI::Get_processor_name(procName, procNameLen);
    std::cout << "Hello from " << procName << "\n";

    mSplitInfo = GenerateSplitInfo(properties.gridSize);
    mStepSize = GetStepSize(properties);

    mSolveResultExtended = std::nullopt;
    mSolveResultDataView = nullptr;
}

std::array<DESolverMPI::ValueType, 3> DESolverMPI::GetStepSize(const SolutionProperties& properties)
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

void DESolverMPI::SetInitialValues(
    ValueType* chunk,
    Func boundaryConditionsFunc,
    ValueType innerStartValue)
{
    int Nx = mProperties.gridSize[0];
    int Ny = mProperties.gridSize[1];
    int Nz = mProperties.gridSize[2];

    int iStart = mSplitInfo.offsets[mProcRank];
    int iEnd = mSplitInfo.offsets[mProcRank] + mSplitInfo.sizes[mProcRank];

    for (int i = iStart; i < iEnd; ++i) {
        ValueType x = mProperties.areaStart[0] + mStepSize[0] * i;

        for (int j = 0; j < Ny; ++j) {
            ValueType y = mProperties.areaStart[1] + mStepSize[1] * j;

            for (int k = 0; k < Nz; ++k) {
                ValueType z = mProperties.areaStart[2] + mStepSize[2] * k;

                ValueType value;
                if (i == 0 || i == Nx - 1 || j == 0 || j == Ny - 1 || k == 0 || k == Nz - 1) {
                    value = boundaryConditionsFunc(x, y, z);
                }
                else {
                    value = innerStartValue;
                }
                chunk[COORDS(i - iStart, j, k, mProperties.gridSize)] = value;
            }
        }
    }
}

void DESolverMPI::Solve(
    NextIterationCallback nextIterFn,
    Func boundaryConditionsFunc,
    ValueType innerStartValue,
    const ValueType eps)
{
    int iStart = mSplitInfo.offsets[mProcRank];
    int iEnd = mSplitInfo.offsets[mProcRank] + mSplitInfo.sizes[mProcRank];

    int planeSize = mProperties.gridSize[1] * mProperties.gridSize[2];
    int extendedChunkSize = planeSize * (mSplitInfo.sizes[mProcRank] + 2);

    std::vector<ValueType> extendedChunk(extendedChunkSize);
    ValueType* dataChunkView = &*(extendedChunk.begin() + planeSize);

    SetInitialValues(dataChunkView, std::move(boundaryConditionsFunc), innerStartValue);

    std::vector<ValueType> extendedChunkSecond = extendedChunk;
    ValueType* dataChunkViewSecond = &*(extendedChunkSecond.begin() + planeSize);

    int iterCount = 0;
    ValueType globalPrecision = std::numeric_limits<ValueType>::max();
    while (globalPrecision > eps) {
        iterCount++;

        MPI::Request topPlaneSend, topPlaneRecieve, bottomPlaneSend, bottomPlaneRecieve;

        // Initiate sending and recieving border planes
        if (mProcRank != 0) {
            ValueType* myTopPlane = &*(extendedChunk.begin() + planeSize);
            topPlaneSend = mCommWorld.Isend(myTopPlane, planeSize, MPIValueType, mProcRank - 1, 0);

            ValueType* neighbourTopPlane = &*(extendedChunk.begin());
            topPlaneRecieve = mCommWorld.Irecv(neighbourTopPlane, planeSize, MPIValueType, mProcRank - 1, MPI::ANY_TAG);
        }

        if (mProcRank != mWorldSize - 1) {
            ValueType* myBottomPlane = &*(extendedChunk.end() - 2 * planeSize);
            bottomPlaneSend = mCommWorld.Isend(myBottomPlane, planeSize, MPIValueType, mProcRank + 1, 0);

            ValueType* neighbourBottomPlane = &*(extendedChunk.end() - planeSize);
            bottomPlaneRecieve = mCommWorld.Irecv(neighbourBottomPlane, planeSize, MPIValueType, mProcRank + 1,
                                                  MPI::ANY_TAG);
        }


        // Calculate inner values
        ValueType localPrecision = 0;
        for (int i = iStart + 1; i < iEnd - 1; ++i) {
            ValueType p = UpdatePlane(i, iStart, dataChunkView, dataChunkViewSecond, nextIterFn);
            localPrecision = std::max(p, localPrecision);
        }


        // Wait until top of bottom plane is recieved
        while (!topPlaneRecieve.Test() || !bottomPlaneRecieve.Test()) {
            // do nothing
        }


        if (topPlaneRecieve.Test()) { // If top plane was recieved first
            if (mProcRank != 0) {
                ValueType p = UpdatePlane(iStart, iStart, dataChunkView, dataChunkViewSecond, nextIterFn);
                localPrecision = std::max(p, localPrecision);
            }

            bottomPlaneRecieve.Wait();

            if (mProcRank != mWorldSize - 1) {
                ValueType p = UpdatePlane(iEnd - 1, iStart, dataChunkView, dataChunkViewSecond, nextIterFn);
                localPrecision = std::max(p, localPrecision);
            }
        }
        else { // If bottom plane was recieved first
            if (mProcRank != mWorldSize - 1) {
                ValueType p = UpdatePlane(iEnd - 1, iStart, dataChunkView, dataChunkViewSecond, nextIterFn);
                localPrecision = std::max(p, localPrecision);
            }

            topPlaneRecieve.Wait();

            if (mProcRank != 0) {
                ValueType p = UpdatePlane(iStart, iStart, dataChunkView, dataChunkViewSecond, nextIterFn);
                localPrecision = std::max(p, localPrecision);
            }
        }

        mCommWorld.Allreduce(&localPrecision, &globalPrecision, 1, MPIValueType, MPI::MAX);

        std::swap(dataChunkView, dataChunkViewSecond);
        std::swap(extendedChunk, extendedChunkSecond);

        if (mProcRank == 0) {
            std::cout << "Iteration: " << iterCount << "; precision: " << globalPrecision << "\n";
        }
    }

    mSolveResultExtended = std::move(extendedChunk);
    mSolveResultDataView = &*(mSolveResultExtended->begin() + planeSize);
}

DESolverMPI::ValueType DESolverMPI::UpdatePlane(int i, int offset,
                                                const ValueType* srcChunk,
                                                ValueType* dstChunk,
                                                NextIterationCallback nextIterFn)
{
    ValueType precision = 0;
    ValueType x = mProperties.areaStart[0] + mStepSize[0] * i;

    for (int j = 1; j < mProperties.gridSize[1] - 1; ++j) {
        ValueType y = mProperties.areaStart[1] + mStepSize[1] * j;

        for (int k = 1; k < mProperties.gridSize[2] - 1; ++k) {
            ValueType z = mProperties.areaStart[2] + mStepSize[2] * k;

            int idx = COORDS(i - offset, j, k, mProperties.gridSize);
            dstChunk[idx] = nextIterFn(mStepSize, x, y, z, i - offset, j, k, srcChunk, mProperties.gridSize);

            ValueType diff = std::abs(srcChunk[idx] - dstChunk[idx]);
            precision = std::max(precision, diff);
        }
    }
    return precision;
}

DESolverMPI::ValueType DESolverMPI::GetMaxDelta(Func referenceFunc)
{
    if (!mSolveResultExtended.has_value()) {
        throw std::runtime_error("There is no solving result yet");
    }

    ValueType localMaxDelta = 0;

    int iStart = mSplitInfo.offsets[mProcRank];
    int iEnd = mSplitInfo.offsets[mProcRank] + mSplitInfo.sizes[mProcRank];

    for (int i = iStart; i < iEnd; ++i) {
        ValueType x = mProperties.areaStart[0] + mStepSize[0] * i;

        for (int j = 0; j < mProperties.gridSize[1]; ++j) {
            ValueType y = mProperties.areaStart[1] + mStepSize[1] * j;

            for (int k = 0; k < mProperties.gridSize[2]; ++k) {
                ValueType z = mProperties.areaStart[2] + mStepSize[2] * k;

                ValueType delta = std::fabs(
                    mSolveResultDataView[COORDS(i - iStart, j, k, mProperties.gridSize)] - referenceFunc(x, y, z));
                localMaxDelta = std::max(localMaxDelta, delta);
            }
        }
    }

    ValueType globalMaxDelta;
    mCommWorld.Allreduce(&localMaxDelta, &globalMaxDelta, 1, MPIValueType, MPI::MAX);
    return globalMaxDelta;
}
