#include "SLESolver.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const SLESolver::DataType SLESolver::sParam = -0.01;

void SLESolver::Solve(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps)
{
    SLESolver solver;
    solver.solve(matA, vecB, foundX, N, eps);
}

void SLESolver::solve(const DataType* matA, const DataType* vecB, DataType* foundX, const std::size_t N, const DataType eps)
{
    mMatA = matA;
    mVecB = vecB;

    mVecBLength = vectorLength(mVecB, N);

    std::vector<DataType> vecAxMinusB(N);

    // set initial X value
    std::fill_n(foundX, N, 0);

    do {
        multiplyMatrixByVector(mMatA, foundX, vecAxMinusB.data(), N);
        vectorSubtract(vecAxMinusB.data(), mVecB, N);

        multiplyVectorByScalar(vecAxMinusB.data(), sParam, N);
        vectorSubtract(foundX, vecAxMinusB.data(), N);
    }
    while (!checkPrecision(vecAxMinusB.data(), eps, N));
}

void SLESolver::multiplyMatrixByVector(const DataType* mat, const DataType* vec, DataType* vecOut, std::size_t N)
{
    std::fill_n(vecOut, N, 0);

    #pragma omp parallel for
    for (std::size_t i = 0; i < N; ++i) // row
    {
        for (std::size_t j = 0; j < N; ++j) // col
        {
            vecOut[i] += mat[i * N + j] * vec[j];
        }
    }
}

void SLESolver::multiplyVectorByScalar(DataType* vec, const DataType scalar, std::size_t N)
{
    #pragma omp parallel for
    for (std::size_t i = 0; i < N; ++i)
    {
        vec[i] *= scalar;
    }
}

void SLESolver::vectorSubtract(DataType* vecA, const DataType* vecB, std::size_t N)
{
    #pragma omp parallel for
    for (std::size_t i = 0; i < N; ++i)
    {
        vecA[i] -= vecB[i];
    }
}

SLESolver::DataType SLESolver::vectorLength(const DataType* vec, std::size_t N)
{
    DataType ret = 0;

    #pragma omp parallel for reduction(+: ret)
    for (std::size_t i = 0; i < N; ++i)
    {
        ret += vec[i] * vec[i];
    }

    return std::sqrt(ret);
}

bool SLESolver::checkPrecision(const DataType* vecAxMinusB, DataType eps, std::size_t N)
{
    DataType vecAxMinusBLength = vectorLength(vecAxMinusB, N);
    return vecAxMinusBLength / mVecBLength < eps;
}
