#include "matrix_inversion.h"

std::size_t calc_inv_matrix::matSizeToIterCount(std::size_t N)
{
    double input_start = 0;
    double input_end = 200;

    double output_start = 2000;
    double output_end = 50;

    if (N > 200) return output_end;

    double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
    return output_start + slope * (N - input_start);
}
