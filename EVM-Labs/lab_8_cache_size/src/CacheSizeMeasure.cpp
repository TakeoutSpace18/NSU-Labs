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

CacheSizeMeasure::CacheSizeMeasure()
: array_(new size_t[array_increase_step * measuring_count]) {}

CacheSizeMeasure::~CacheSizeMeasure() {
    delete[] array_;
}

void CacheSizeMeasure::openGNUPlot() {
    FILE* gnuplot_ = popen("gnuplot -","w");
    fprintf(gnuplot_, "set xlabel \"array size\" \n");
    fprintf(gnuplot_, "set ylabel \"time\" \n");
    fprintf(gnuplot_, "plot '%s' with lines \n", output_filename_);
    fflush(gnuplot_);
}

void CacheSizeMeasure::doMeasurings() {
    output_filename_ = "measured_points.dat";
    std::ofstream data_output(output_filename_);

    for (size_t i = 1; i <= measuring_count; ++i) {
        size_t N = array_increase_step * i;
        prepareForTraverse(i * array_increase_step, traverse_type_);
        auto time = measureFunctionRuntime(std::mem_fn(&CacheSizeMeasure::traverseArray), this,  N, 5);

        if (i % 100 == 0) {
            std::cout << i << "/" << measuring_count << std::endl;
        }
        data_output << N << " " << time.count() << std::endl;
    }
    data_output.close();
}

int CacheSizeMeasure::launch() {
    doMeasurings();
    openGNUPlot();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(180s);
    return EXIT_SUCCESS;
}

void CacheSizeMeasure::prepareForTraverse(size_t N, TraverseType type) {
    assert(N <= measuring_count * array_increase_step);
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
