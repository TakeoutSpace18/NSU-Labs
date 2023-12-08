#include <iostream>
#include <thread>

#include "CacheSizeMeasure.h"

static std::string random_filename = "measured_data_random.csv";
static std::string direct_filename = "measured_data_direct.csv";
static std::string reverse_filename = "measured_data_reverse.csv";

static CacheSizeMeasure::Config conifg = {
    .array_increase_step_start = 100,
    .array_increase_step_multiplier = 1.005,
    .array_max_size_bytes = 20000000,
    .traverse_repeat_count = 30,
    .measure_repeats = 3
};

int main() {
    auto measurer = std::make_unique<CacheSizeMeasure>(conifg);;
    measurer->doMeasurings(random_filename, CacheSizeMeasure::TraverseType::Random);
    measurer->doMeasurings(direct_filename, CacheSizeMeasure::TraverseType::Direct);
    measurer->doMeasurings(reverse_filename, CacheSizeMeasure::TraverseType::Reverse);

    return EXIT_SUCCESS;
}
