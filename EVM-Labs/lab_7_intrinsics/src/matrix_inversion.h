#ifndef MATRIX_INVERSION_H
#define MATRIX_INVERSION_H
#include <cstddef>

constexpr std::size_t sum_iter_count = 100;

namespace naive_impl {
    void calc_inv_matrix(float *in, float *out, std::size_t N);
}

namespace simd_impl {
    void calc_inv_matrix(float *in, float *out, std::size_t N);
}

#endif //MATRIX_INVERSION_H
