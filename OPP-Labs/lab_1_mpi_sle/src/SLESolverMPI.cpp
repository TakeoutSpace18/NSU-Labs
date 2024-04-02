#include "SLESolverMPI.h"

#include <cstdlib>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <mpi.h>

#include "utils.hpp"

constexpr float TAO = -0.01;
constexpr float EPS = 1e-2;
constexpr int N = 50 * 50;
constexpr int MEASURE_REPEATS = 2;

SLESolverMPI::SLESolverMPI(const MPI::Comm& comm) : mWorldComm(comm)
{
    mWorldSize = mWorldComm.Get_size();
    mRank = mWorldComm.Get_rank();
    char procName[MPI::MAX_PROCESSOR_NAME];
    int procNameLen;
    MPI::Get_processor_name(procName, procNameLen);
    std::cout << "Hello from "<<  procName << "\n";
}

int SLESolverMPI::measureTimeWithRepeats(int argc, char** argv) const
{
    double minTime = std::numeric_limits<double>::max();
    for (int i = 0; i < MEASURE_REPEATS; ++i)
    {
        minTime = std::min(minTime, compute(argc, argv));
    }
    if (mRank == 0)
    {
        std::ofstream out("timeElapsed.txt");
        out << minTime;
    }
    return EXIT_SUCCESS;
}

double SLESolverMPI::compute(int argc, char** argv) const
{
    double beginTime = MPI::Wtime();

    DataSplitInfo splitInfo = generateLinesSplitInfo(N);

    // load and scatter matrix A
    std::vector<float> matA;
    if (mRank == 0)
    {
        matA = loadData(N * N, "matA.bin");
    }
    std::vector<float> matAChunk = scatterMatA(matA, splitInfo, N);

    // load and calc norm of vector B
    std::vector<float> vecB = loadData(N, "vecB.bin");
    float vecBNormSq = scalarProduct(vecB.data(), vecB.data(), N);

    int iterCount = 0;
    std::vector<float> curVecX(N, 0);
    while (true)
    {
        iterCount++;

        float normSqChunk = 0;
        std::vector<float> intermVec(splitInfo.sizes[mRank]);
        for (int i = 0; i < splitInfo.sizes[mRank]; ++i)
        {
            intermVec[i] = scalarProduct(matAChunk.data() + N * i, curVecX.data(), N);
            intermVec[i] -= vecB[splitInfo.offsets[mRank] + i];
            normSqChunk += intermVec[i] * intermVec[i];
            intermVec[i] *= -TAO;
            intermVec[i] += curVecX[splitInfo.offsets[mRank] + i];
        }

        allGatherVecX(intermVec, curVecX, splitInfo);
        float normSq = 0;
        mWorldComm.Allreduce(&normSqChunk, &normSq, 1, MPI::FLOAT, MPI::SUM);

        if (normSq < EPS * EPS * vecBNormSq)
        {
            break;
        }

        if (mRank == 0 && iterCount % 50 == 0)
        {
            std::cout << "iteration " << iterCount << ": normSq - " << normSq << std::endl;
        }
    }

    double endTime = MPI::Wtime();
    double timeElapsed = endTime - beginTime;
    double minTimeElapsed;

    mWorldComm.Reduce(&timeElapsed, &minTimeElapsed, 1, MPI::DOUBLE, MPI::MAX, 0);

    if (mRank == 0)
    {
        saveData(curVecX, "foundVecX.bin");
        std::cout << "Finished!\n";
        std::cout << "Total iterations: " << iterCount << std::endl;
        std::cout << "Time elapsed: " << minTimeElapsed << " seconds\n";
    }

    return minTimeElapsed;
}

SLESolverMPI::DataSplitInfo SLESolverMPI::generateLinesSplitInfo(const int N) const
{
    DataSplitInfo split_info;
    split_info.sizes.reserve(mWorldSize);
    split_info.offsets.reserve(mWorldSize);

    int chunk_lines = N / mWorldSize;
    int remaining_lines = N % mWorldSize;

    int cur_offset = 0;
    for (int i = 0; i < remaining_lines; ++i)
    {
        int cur_size = chunk_lines + 1;
        split_info.sizes.push_back(cur_size);
        split_info.offsets.push_back(cur_offset);
        cur_offset += cur_size;
    }

    for (int i = remaining_lines; i < mWorldSize; ++i)
    {
        int cur_size = chunk_lines;
        split_info.sizes.push_back(cur_size);
        split_info.offsets.push_back(cur_offset);
        cur_offset += cur_size;
    }

    return split_info;
}

std::vector<float> SLESolverMPI::scatterMatA(const std::vector<float>& matA, const DataSplitInfo& linesSplitInfo, const int N) const
{
    std::vector<int> sizes(mWorldSize);
    std::transform(
        linesSplitInfo.sizes.begin(),
        linesSplitInfo.sizes.end(),
        sizes.begin(),
        [N](const int sz) { return sz * N; }
    );

    std::vector<int> offsets(mWorldSize);
    std::transform(
        linesSplitInfo.offsets.begin(),
        linesSplitInfo.offsets.end(),
        offsets.begin(),
        [N](const int sz) { return sz * N; }
    );

    std::vector<float> matAChunk(sizes[mRank]);

    mWorldComm.Scatterv(
        matA.data(),
        sizes.data(),
        offsets.data(),
        MPI::FLOAT,
        matAChunk.data(),
        sizes[mRank],
        MPI::FLOAT,
        0);

    return matAChunk;
}

void SLESolverMPI::allGatherVecX(std::vector<float>& vecXChunk, std::vector<float>& vecXResult,
                                 const DataSplitInfo& linesSplitInfo) const
{
    mWorldComm.Allgatherv(
        vecXChunk.data(),
        linesSplitInfo.sizes[mRank],
        MPI_FLOAT,
        vecXResult.data(),
        linesSplitInfo.sizes.data(),
        linesSplitInfo.offsets.data(),
        MPI_FLOAT);
}

float SLESolverMPI::scalarProduct(const float* a, const float* b, int N)
{
    float result = 0;
    for (int i = 0; i < N; ++i)
    {
        result += a[i] * b[i];
    }
    return result;
}
