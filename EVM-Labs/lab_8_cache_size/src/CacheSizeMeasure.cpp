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

CacheSizeMeasure::CacheSizeMeasure(const Config& config)
: config_(config),
array_(new size_t[config_.array_max_size_bytes / sizeof(*array_) + 1]) {}

CacheSizeMeasure::~CacheSizeMeasure() {
    delete[] array_;
}

void CacheSizeMeasure::doMeasurings(const std::string& output_filename, const TraverseType type) {
    std::ofstream data_output(output_filename);

    size_t measurings_count =
            std::log(
                (config_.array_increase_step_multiplier - 1) * config_.array_max_size_bytes / sizeof(array_) /
                config_.array_increase_step_start + 1) / std::log(config_.array_increase_step_multiplier);

    size_t curr_measure = 1;
    double curr_step = config_.array_increase_step_start;
    for (size_t N = 10; N * sizeof(*array_) < config_.array_max_size_bytes; N += curr_step) {
        prepareForTraverse(N, type);
        traverseArray(N, 1); // cache warm-up

        double avg_time = 0;
        for (uint32_t j = 0; j < config_.measure_repeats; ++j) {

            prepareForTraverse(N, type);
            auto time = measureFunctionRuntime(std::mem_fn(&CacheSizeMeasure::traverseArray), this, N,
                                               config_.traverse_repeat_count);
            avg_time += static_cast<double>(time.count());
        }
        avg_time /= config_.measure_repeats;

        if (curr_measure % 10 == 0) {
            std::cout << curr_measure << "/" << measurings_count << std::endl;
        }
        data_output << N * sizeof(array_) << "," << avg_time / N << std::endl;
        curr_step *= config_.array_increase_step_multiplier;
        ++curr_measure;
    }
    data_output.close();
}

void CacheSizeMeasure::prepareForTraverse(const size_t N, const TraverseType type) const {
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
            std::vector<std::size_t> traverse_order(N);
            std::iota(traverse_order.begin(), traverse_order.end(), 0);

            std::random_device rd;
            std::mt19937_64 g(rd());
            std::ranges::shuffle(traverse_order, g);

            for (std::size_t i = 0; i < N-1; ++i) {
                array_[traverse_order[i]] = traverse_order[i+1];
            }
            array_[traverse_order[N-1]] = traverse_order[0];

            break;
    }
}

void CacheSizeMeasure::traverseArray(const size_t N, const uint32_t repeat_count) const {
    size_t k = 0;
    for (size_t i = 0; i < N * repeat_count; i++) {
        k = static_cast<size_t volatile *>(array_)[k];
    }
}
