#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <mpi.h>

#include "common.hpp"

constexpr float TAO = -0.01;
constexpr float EPS = 1e-1;

std::vector<float> scatter_mat_a(std::vector<float>& matA, int N, int worldSize);

void gather_found_x(std::vector<float>& xChunk, std::vector<float>& dest, int N, int worldSize);

float scalar_product(const float* a, const float* b, int N);

int main(int argc, char** argv)
{
    MPI::Init(argc, argv);
    int worldSize = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    int N = 50 * 50;
    std::vector<float> matA(N * N);
    std::vector<float> vecB(N);
    std::vector<float> curVecX(N, 0);

    if (rank == 0)
    {
        loadData(matA.data(), "matA.bin", N * N);
        loadData(vecB.data(), "vecB.bin", N);
    }

    std::vector matAChunk = scatter_mat_a(matA, N, worldSize);
    int nLines = matAChunk.size() / N;

    MPI::COMM_WORLD.Bcast(vecB.data(), N, MPI_FLOAT, 0);
    float vecBNormSq = scalar_product(vecB.data(), vecB.data(), N);

    int iterCount = 0;

    while (true)
    {
        iterCount++;

        MPI::COMM_WORLD.Bcast(curVecX.data(), N, MPI_FLOAT, 0);

        float normChunk = 0;
        std::vector<float> intermChunk(nLines);

        for (int i = 0; i < nLines; ++i)
        {
            intermChunk[i] = scalar_product(matAChunk.data() + N * i, curVecX.data(), N);
            intermChunk[i] -= vecB[i];
            normChunk += intermChunk[i] * intermChunk[i];
            intermChunk[i] *= -TAO;
            intermChunk[i] += curVecX[i];
        }

        gather_found_x(intermChunk, curVecX, N, worldSize);

        float norm;
        MPI::COMM_WORLD.Allreduce(&normChunk, &norm, 1, MPI_FLOAT, MPI_SUM);

        if (rank == 0)
        {
            std::cout << "iteration " << iterCount << ": norm - " << norm << std::endl;
        }

        if (norm < EPS * EPS * vecBNormSq)
        {
            break;
        }
    }

    if (rank == 0)
    {
        saveData(curVecX, "foundX.bin");
    }

    MPI::Finalize();
    return EXIT_SUCCESS;
}

float scalar_product(const float* a, const float* b, int N)
{
    float result = 0;
    for (int i = 0; i < N; ++i)
    {
        result += a[i] * b[i];
    }

    return result;
}

std::vector<float> scatter_mat_a(std::vector<float>& matA, int N, int worldSize)
{

    std::cout << "world size: " << worldSize << std::endl;
    int chunkSize = (N / worldSize) * N;
    std::vector<int> sizes(worldSize, chunkSize);

    int remainingSize = N * N - chunkSize * worldSize;
    std::cout << remainingSize << std::endl;

    if (remainingSize) chunkSize += N;
    for (int i = 0; i < remainingSize; ++i)
    {
        sizes[i] += N;
    }

    std::vector<int> offsets;
    offsets.reserve(worldSize);

    std::vector<float> matAChunk(chunkSize);

    int curOffset = 0;
    for (int i = 0; i < worldSize; i++)
    {
        offsets.push_back(curOffset);
        curOffset += sizes[i];
    }

    int status = MPI_Scatterv(
        matA.data(),
        sizes.data(),
        offsets.data(),
        MPI_FLOAT,
        matAChunk.data(),
        chunkSize,
        MPI_FLOAT,
        0,
        MPI_COMM_WORLD
        );

    return matAChunk;
}

void gather_found_x(std::vector<float>& xChunk, std::vector<float>& dest, int N, int worldSize)
{
    int chunkSize = N / worldSize;
    std::vector sizes(worldSize, chunkSize);

    int remainingSize = N - chunkSize * worldSize;
    std::cout << remainingSize << std::endl;
    if (remainingSize) chunkSize++;
    for (int i = 0; i < remainingSize; ++i)
    {
        sizes[i]++;
    }

    std::vector<int> offsets;
    offsets.reserve(worldSize);

    int curOffset = 0;
    for (int i = 0; i < worldSize; i++)
    {
        offsets.push_back(curOffset);
        curOffset += sizes[i];
    }

    MPI::COMM_WORLD.Gatherv(
        xChunk.data(),
        chunkSize,
        MPI_FLOAT,
        dest.data(),
        sizes.data(),
        offsets.data(),
        MPI_FLOAT,
        0);
}
