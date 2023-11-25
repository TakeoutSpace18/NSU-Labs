#include "CacheSizeMeasure.h"

#include <algorithm>
#include <random>
#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <thread>
#include <bits/random.h>

CacheSizeMeasure::CacheSizeMeasure(const Config& config)
: array_increase_step_start_(config.array_increase_step_start),
array_max_size_bytes_(config.array_max_size),
traverse_repeat_count_(config.traverse_repeat_count),
array_increase_step_multiplier_(config.array_increase_step_multiplier),
array_(new size_t[array_max_size_bytes_ / sizeof(*array_) + 1]) {}

CacheSizeMeasure::~CacheSizeMeasure() {
    delete[] array_;
}

void CacheSizeMeasure::doMeasurings(const std::string& output_filename, TraverseType type) {
    std::ofstream data_output(output_filename);

    size_t measurings_count =
            std::log(
                (array_increase_step_multiplier_ - 1) * array_max_size_bytes_ / sizeof(array_) /
                array_increase_step_start_ + 1) / std::log(array_increase_step_multiplier_);

    size_t curr_measure = 1;
    double curr_step = array_increase_step_start_;
    for (size_t N = 10; N * sizeof(*array_) < array_max_size_bytes_; N += curr_step) {
        prepareForTraverse(N, type);
        traverseArray(N, 1); // cache warm-up

        const int measure_repeats = 3;
        double avg_time = 0;
        for (int j = 0; j < measure_repeats; ++j) {
            auto time = measureFunctionRuntime(std::mem_fn(&CacheSizeMeasure::traverseArray), this,  N, traverse_repeat_count_);
            avg_time += static_cast<double>(time.count());
        }
        avg_time /= measure_repeats;

        if (curr_measure % 10 == 0) {
            std::cout << curr_measure << "/" << measurings_count << std::endl;
        }
        data_output << N * sizeof(array_) << "," << avg_time / N << std::endl;
        curr_step *= array_increase_step_multiplier_;
        ++curr_measure;
    }
    data_output.close();
}

void CacheSizeMeasure::prepareForTraverse(size_t N, TraverseType type) {
    switch (type) {
        case TraverseType::Direct:
            std::iota(array_, array_ + N, 1);
            array_[N-1] = 0;
            break;
        case TraverseType::Reverse:
            std::iota(array_ + 1, array_ + N, 0);
            array_[0] = N-1;
            break;
        case TraverseType::Random:
            std::iota(array_, array_ + N, 1);
            array_[N-1] = 0;

            std::random_device rd;
            std::mt19937_64 g(rd());
            std::shuffle(array_, array_ + N, g);
            break;
    }
}

void CacheSizeMeasure::traverseArray(size_t N, uint32_t repeat_count) const {
    size_t k = 0;
    for (size_t i = 0; i < N * repeat_count; i++) {
        k = static_cast<size_t volatile *>(array_)[k];
    }
}
