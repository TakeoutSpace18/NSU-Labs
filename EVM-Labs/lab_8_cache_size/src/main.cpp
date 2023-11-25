#include <iostream>
#include <thread>

#include "CacheSizeMeasure.h"

static std::string random_filename = "measured_data_random.dat";
static std::string direct_filename = "measured_data_direct.dat";
static std::string reverse_filename = "measured_data_reverse.dat";

static CacheSizeMeasure::Config L1_config = {
    .array_increase_step = 40,
    .measuring_count = 530,
    .traverse_repeat_count = 50
};

static CacheSizeMeasure::Config L2_config = {
    .array_increase_step = 100,
    .measuring_count = 3000,
    .traverse_repeat_count = 10
};

static CacheSizeMeasure::Config L3_config = {
    .array_increase_step = 4000,
    .measuring_count = 530,
    .traverse_repeat_count = 4
};

int main() {
    auto measurer = std::make_unique<CacheSizeMeasure>(L1_config);
    std::string name_prefix = "L1_";
    measurer->doMeasurings(name_prefix + random_filename, CacheSizeMeasure::TraverseType::Random);
    measurer->doMeasurings(name_prefix + direct_filename, CacheSizeMeasure::TraverseType::Direct);
    measurer->doMeasurings(name_prefix + reverse_filename, CacheSizeMeasure::TraverseType::Reverse);

    name_prefix = "L2_";
    measurer = std::make_unique<CacheSizeMeasure>(L2_config);;
    measurer->doMeasurings(name_prefix + random_filename, CacheSizeMeasure::TraverseType::Random);
    measurer->doMeasurings(name_prefix + direct_filename, CacheSizeMeasure::TraverseType::Direct);
    measurer->doMeasurings(name_prefix + reverse_filename, CacheSizeMeasure::TraverseType::Reverse);

    name_prefix = "L3_";
    measurer = std::make_unique<CacheSizeMeasure>(L3_config);;
    measurer->doMeasurings(name_prefix + random_filename, CacheSizeMeasure::TraverseType::Random);
    measurer->doMeasurings(name_prefix + direct_filename, CacheSizeMeasure::TraverseType::Direct);
    measurer->doMeasurings(name_prefix + reverse_filename, CacheSizeMeasure::TraverseType::Reverse);

    return EXIT_SUCCESS;
}
