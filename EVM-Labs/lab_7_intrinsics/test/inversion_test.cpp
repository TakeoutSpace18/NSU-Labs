#include <new>
#include <random>
#include <gtest/gtest.h>
#include <Eigen/Dense>

#include "matrix_inversion.h"
using namespace naive_impl;

// void calc_inv_matrix(float *in, float *out, std::size_t N) {
// 	Eigen::MatrixXf mat_a = Eigen::Map<const Eigen::MatrixXf>(in, N, N);
// 	Eigen::MatrixXf mat_inv = mat_a.colPivHouseholderQr().inverse();
// 	std::copy(mat_inv.data(), mat_inv.data() + N * N, out);
// }

template <class F, class... Args>
auto measure_function_runtime(F func, Args&&... args) {
	using clock = std::chrono::high_resolution_clock;

	const auto start = clock::now();
	func(std::forward<Args>(args)...);
	const auto stop = clock::now();
	return stop - start;
}

void create_random_diagonally_dominant_matrix(float *a, std::size_t N) {
	std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 10.0);

	for (std::size_t i = 0; i < N; i++)
		for (std::size_t j = 0; j < N; j++)
			a[i * N + j] = dis(gen);

	for (std::size_t i = 0; i < N; i++) {
		float sum = 0.f;
		for (std::size_t j = 0; j < N; j++)
			sum += a[i * N + j];
		a[i * N + i] += sum;
	}
}

class MatrixInversionParametrizedTest : public ::testing::TestWithParam<std::size_t> {
protected:
	std::size_t N;
};

TEST_P(MatrixInversionParametrizedTest, test1) {
	std::size_t N = GetParam();
	
	float *a = new(std::align_val_t(32)) float[N * N];
	float *a_inv = new float[N * N];
	
	create_random_diagonally_dominant_matrix(a, N);

	namespace chr = std::chrono;
	const auto time = measure_function_runtime(calc_inv_matrix, a, a_inv, N);
	std::cout << "Time elapsed: " << chr::duration_cast<chr::milliseconds>(time).count() << " ms\n";

	Eigen::MatrixXf mat_a = Eigen::Map<const Eigen::MatrixXf>(a, N, N);
	Eigen::MatrixXf mat_inv = Eigen::Map<const Eigen::MatrixXf>(a_inv, N, N);
	EXPECT_TRUE((mat_a * mat_inv).isIdentity(1e-3));

	delete[] a;
	delete[] a_inv;
}

INSTANTIATE_TEST_SUITE_P(
        MatrixInversionTests,
        MatrixInversionParametrizedTest,
        ::testing::Values(
                4, 200, 256
        ));