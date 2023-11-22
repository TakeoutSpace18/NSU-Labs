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
: array_increase_step_(config.array_increase_step),
measuring_count_(config.measuring_count),
traverse_repeat_count_(config.traverse_repeat_count),
array_(new size_t[array_increase_step_ * measuring_count_]) {}

CacheSizeMeasure::~CacheSizeMeasure() {
    delete[] array_;
}

void CacheSizeMeasure::doMeasurings(const std::string& output_filename, TraverseType type) {
    std::ofstream data_output(output_filename);

    for (size_t i = 1; i <= measuring_count_; ++i) {
        size_t N = array_increase_step_ * i;
        prepareForTraverse(i * array_increase_step_, type);
        auto time = measureFunctionRuntime(std::mem_fn(&CacheSizeMeasure::traverseArray), this,  N, traverse_repeat_count_);

        if (i % 10 == 0) {
            std::cout << i << "/" << measuring_count_ << std::endl;
        }
        data_output << N * sizeof(array_) << " " << time.count() << std::endl;
    }
    data_output.close();
}

void CacheSizeMeasure::prepareForTraverse(size_t N, TraverseType type) {
    assert(N <= measuring_count_ * array_increase_step_);
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
