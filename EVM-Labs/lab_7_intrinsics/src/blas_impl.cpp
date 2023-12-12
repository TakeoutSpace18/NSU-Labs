#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <cblas-openblas.h>
#include <cblas.h>

#include "matrix_inversion.h"


namespace  {
    float calc_a_one_mul_a_inf(const float* mat_a, std::size_t N) {
        float a_inf_max = std::numeric_limits<float>::min();
        std::vector<float> a_one_sums(N, 0);

        for (std::size_t i = 0; i < N; ++i) {
            float a_inf_sum = 0;
            for (std::size_t j = 0; j < N; ++j) {
                a_inf_sum += mat_a[i*N + j];
                a_one_sums[j] += mat_a[i*N + j];
            }
            a_inf_max = std::max(a_inf_max, a_inf_sum);
        }

        float a_one_max = std::numeric_limits<float>::min();
        for (std::size_t j = 0; j < N; ++j) {
            a_one_max = std::max(a_one_max, a_one_sums[j]);
        }

        return a_one_max * a_inf_max;
    }

    void calc_mat_b(const float* a, float* out, std::size_t N) {
        float denom = calc_a_one_mul_a_inf(a, N);
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < N; ++j) {
                float ij = a[i*N + j] / denom;
                float ji = a[j*N + i] / denom;

                out[i*N + j] = ji;
                out[j*N + i] = ij;
            }
        }
    }

    void mat_mul(const float* a, const float* b, float* out, std::size_t N) {
        std::fill_n(out, N*N, 0);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1, a, N, b, N, 0, out, N);
    }

    void calc_mat_r(const float* a, const float* b, float* out, std::size_t N) {
        mat_mul(b, a, out, N);

        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < N; ++j) {
                out[i*N + j] *= -1;
                if (i == j) {
                    out[i*N + j] += 1;
                }
            }
        }
    }

    void create_identity_mat(float* out, std::size_t N) {
        std::fill_n(out, N*N, 0);
        for (std::size_t i = 0; i < N; ++i) {
            out[i*N + i] = 1;
        }
    }


    void mat_add(float* dest, const float* src, std::size_t N) {
        for (std::size_t i = 0; i < N*N; ++i) {
            dest[i] += src[i];
        }
    }

}

void calc_inv_matrix_blas::operator()(float* in, float* out, std::size_t N) {
    openblas_set_num_threads(1);

    std::vector<float> b(N*N, 0);
    std::vector<float> r(N*N, 0);
    std::vector<float> cur_r(N*N, 0);
    std::vector<float> cur_r_copy(N*N, 0);
    std::vector<float> sum(N*N, 0);

    create_identity_mat(cur_r.data(), N);
    calc_mat_b(in, b.data(), N);
    calc_mat_r(in, b.data(), r.data(), N);

    for (std::size_t iter = 0; iter < sum_iter_count; ++iter) {
        mat_add(sum.data(), cur_r.data(), N);
        std::swap(cur_r, cur_r_copy);
        mat_mul(r.data(), cur_r_copy.data(), cur_r.data(), N);
    }

    mat_mul(sum.data(), b.data(), out, N);
 }

std::string calc_inv_matrix_blas::getDescription() const
{
    return "BLAS_implementation";
}

