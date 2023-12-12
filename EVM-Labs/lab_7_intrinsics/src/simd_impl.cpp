#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <immintrin.h>

#include "matrix_inversion.h"


namespace  {
    __mmask16 sizeToMask[] = {
        0x0000, 0x0001, 0x0003, 0x0007,
        0x000F, 0x001F, 0x003F, 0x007F,
        0x00FF, 0x01FF, 0x03FF, 0x07FF,
        0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF,
        0xFFFF
    };

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
        for (std::size_t i = 0; i < N; ++i)
        {
            for (std::size_t j = 0; j < N; ++j)
            {
                __m128 scalar = _mm_maskz_loadu_ps(sizeToMask[1], a + i*N + j);
                __m512 scalar_line = _mm512_broadcastss_ps(scalar);
                std::size_t k;
                for (k = 0; k < (N & ~15); k += 16)
                {
                    __m512 line = _mm512_loadu_ps(b + j*N + k);
                    line = _mm512_mul_ps(line, scalar_line);
                    __m512 out_line = _mm512_loadu_ps(out + i*N + k);
                    out_line = _mm512_add_ps(out_line, line);
                    _mm512_storeu_ps(out + i*N + k, out_line);
                }
                __mmask16 mask = sizeToMask[N - k];
                __m512 line = _mm512_maskz_loadu_ps(mask, b + N*j + k);
                line = _mm512_mul_ps(line, scalar_line);
                __m512 out_line = _mm512_maskz_loadu_ps(mask, out + i*N + k);
                out_line = _mm512_add_ps(out_line, line);
                _mm512_mask_storeu_ps(out + i*N + k, mask, out_line);
            }
        }
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
        std::size_t i;
        for (i = 0; i < (N*N & ~15); i += 16) {
            __m512 a = _mm512_loadu_ps(src + i);
            __m512 b = _mm512_loadu_ps(dest + i);
            __m512 sum = _mm512_add_ps(a, b);
            _mm512_storeu_ps(dest + i, sum);
        }

        __mmask16 mask = sizeToMask[N*N - i];

        __m512 a = _mm512_maskz_loadu_ps(mask, src + i);
        __m512 b = _mm512_maskz_loadu_ps(mask, dest + i);
        __m512 sum = _mm512_maskz_add_ps(mask, a, b);
        _mm512_mask_storeu_ps(dest + i, mask, sum);
    }

}

void calc_inv_matrix_simd::operator()(float* in, float* out, std::size_t N) {
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

std::string calc_inv_matrix_simd::getDescription() const
{
    return "SIMD_implementation";
}

