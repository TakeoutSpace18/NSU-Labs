#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H
#include <cstddef>
#include <iostream>

inline void mat_print(const float* mat, const std::size_t N) {
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            std::cout << std::fixed << mat[i*N + j] << " ";
        }
        std::cout << "\n";
    }
}

#endif //MATRIX_UTILS_H
