#ifndef MATRIX_INVERSION_H
#define MATRIX_INVERSION_H

#include <cstddef>

struct calc_inv_matrix
{
    virtual void operator()(float *in, float *out, std::size_t N) = 0;
    [[nodiscard]] virtual std::string getDescription() const = 0;

protected:
    static const std::size_t sum_iter_count = 130;
};

struct calc_inv_matrix_naive : calc_inv_matrix
{
    void operator()(float* in, float* out, std::size_t N) override;
    [[nodiscard]] std::string getDescription() const override;
};

struct calc_inv_matrix_simd : calc_inv_matrix
{
    void operator()(float* in, float* out, std::size_t N) override;
    [[nodiscard]] std::string getDescription() const override;
};

struct calc_inv_matrix_blas : calc_inv_matrix
{
    void operator()(float* in, float* out, std::size_t N) override;
    [[nodiscard]] std::string getDescription() const override;
};

#endif //MATRIX_INVERSION_H
