#include "measurable_code.h"

static const double pi = 3.14159265359;

static double factorial(u64_t n) {
    double result = 1.0;
    for (u64_t i = 2; i <= n; ++i) {
        result *= (double)i;
    }
    return result;
}

static double uint_pow(double x, long long y) {
    double result = 1.0;
    for (u64_t i = 0; i < y; ++i) {
        result *= x;
    }
    return result;

}
static double my_sin(double x, double eps) {
    u64_t n = 1;
    double m;
    double result = 0;

    while (x > 2 * pi) {
        x -= 2 * pi;
    }

    do {
        m = uint_pow(x, 2 * n - 1) / factorial(2*n - 1);
        result += n & 0x1 ? m : -m;
                ++n;
    }
    while (eps < m);
    return result;
}

void measured_function(double *buffer, u64_t n) {
    for (int i = 0; i < n; ++i) {
        double s = my_sin(pi * 0.25, 1e-40);
        if (buffer) {
            *buffer = s;
            ++buffer;
        }
    }
}
