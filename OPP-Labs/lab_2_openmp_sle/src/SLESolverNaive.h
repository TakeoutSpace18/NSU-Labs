#ifndef SLESOLVERNAIVE_H
#define SLESOLVERNAIVE_H
#include <cstddef>

#include "SLESolver.h"

// this implementation has distinct for loops with "omp parallel for" for every math operation

class SLESolverNaive : public SLESolver {
public:

    static void Solve(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps);

private:
    SLESolverNaive() = default;

    void solve(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps);

    /// computes vecA := vecA - vecB
    static void vectorSubtract(DataType* vecA, const DataType* vecB, std::size_t N);
    static void multiplyMatrixByVector(const DataType* mat, const DataType* vec, DataType* vecOut, std::size_t N);
    static void multiplyVectorByScalar(DataType* vec, DataType scalar, std::size_t N);
    static DataType vectorLength(const DataType* vec, std::size_t N);

    bool checkPrecision(const DataType* vecAxMinusB, DataType eps, std::size_t N);

private:
    const DataType* mMatA = nullptr;
    const DataType* mVecB = nullptr;

    DataType mVecBLength;
};


#endif //SLESOLVERNAIVE_H
