#ifndef MATRIX_INVERSION_H
#define MATRIX_INVERSION_H

#include <cstddef>
#include <string>

struct calc_inv_matrix
{
    virtual void operator()(float *in, float *out, std::size_t N) = 0;
    [[nodiscard]] virtual std::string getDescription() const = 0;

protected:
    static std::size_t matSizeToIterCount(std::size_t N);
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
