#ifndef SLESOLVER_H
#define SLESOLVER_H
#include <cstddef>


class SLESolver {
public:
    using DataType = float;

    void operator()(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps);

private:
    SLESolver() = default;

    /// computes vecA := vecA - vecB
    static void vectorSubtract(DataType* vecA, const DataType* vecB, std::size_t N);
    static void multiplyMatrixByVector(const DataType* mat, const DataType* vec, DataType* vecOut, std::size_t N);
    static void multiplyVectorByScalar(DataType* vec, DataType scalar, std::size_t N);
    static DataType vectorLength(const DataType* vec, std::size_t N);

    static bool checkPrecision(const DataType* vecAxMinusB, const DataType* vecB, DataType eps, std::size_t N);

private:
    static const DataType sParam;

    const DataType* mMatA = nullptr;
    const DataType* mVecB = nullptr;
};


#endif //SLESOLVER_H
