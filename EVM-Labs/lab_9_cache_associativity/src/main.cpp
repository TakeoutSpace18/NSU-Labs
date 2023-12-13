#include <iostream>
#include <thread>

#include "CacheAssociativityMeasure.h"

static std::string output_filename = "measured_data.csv";

static CacheAssociativityMeasure::Config conifg = {
    .step_offset_bytes = 49152,
    .max_banks = 70,
    .num_sets = 96,
    .traverse_repeats = 5000
};

int main()
{
    auto measurer = std::make_unique<CacheAssociativityMeasure>(conifg);;
    measurer->doMeasurings(output_filename);

    return EXIT_SUCCESS;
}
