#ifndef SLESOLVER_H
#define SLESOLVER_H
#include <cstddef>


class SLESolver {
public:
    using DataType = float;

    static void Solve(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps);

private:
    SLESolver() = default;

    void solve(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps);

    /// computes vecA := vecA - vecB
    static void vectorSubtract(DataType* vecA, const DataType* vecB, std::size_t N);
    static void multiplyMatrixByVector(const DataType* mat, const DataType* vec, DataType* vecOut, std::size_t N);
    static void multiplyVectorByScalar(DataType* vec, DataType scalar, std::size_t N);
    static DataType vectorLength(const DataType* vec, std::size_t N);

    bool checkPrecision(const DataType* vecAxMinusB, DataType eps, std::size_t N);

private:
    static const DataType sParam;

    const DataType* mMatA = nullptr;
    const DataType* mVecB = nullptr;

    DataType mVecBLength;
};


#endif //SLESOLVER_H
