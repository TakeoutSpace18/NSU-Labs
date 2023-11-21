#include <iostream>
#include <thread>

#include "CacheSizeMeasure.h"

static const char* random_filename = "measured_data_random.dat";
static const char* direct_filename = "measured_data_direct.dat";
static const char* reverse_filename = "measured_data_reverse.dat";

static void openGNUPlot(const char* filename) {
    const size_t L1d_cache_size = 49152;
    const size_t L2_cache_size = 1310720;
    const size_t L3_cache_size = 12582912;

    FILE* gnuplot_ = popen("gnuplot -","w");
    fprintf(gnuplot_, "set xlabel \"array size\" \n");
    fprintf(gnuplot_, "set ylabel \"time\" \n");
    fprintf(gnuplot_, "set parametric \n");
    fprintf(gnuplot_, "set arrow from %lu, graph 0 to %lu, graph 1 nohead \n", L1d_cache_size, L1d_cache_size);
    fprintf(gnuplot_, "set arrow from %lu, graph 0 to %lu, graph 1 nohead \n", L2_cache_size, L2_cache_size);
    fprintf(gnuplot_, "set arrow from %lu, graph 0 to %lu, graph 1 nohead \n", L3_cache_size, L3_cache_size);
    fprintf(gnuplot_, "plot '%s' with lines \n", filename);

    fflush(gnuplot_);
}

int main() {
    CacheSizeMeasure cache_size_measure;
    // cache_size_measure.doMeasurings(random_filename, CacheSizeMeasure::TraverseType::Random);
    // cache_size_measure.doMeasurings(direct_filename, CacheSizeMeasure::TraverseType::Direct);
    // cache_size_measure.doMeasurings(reverse_filename, CacheSizeMeasure::TraverseType::Reverse);
    openGNUPlot(random_filename);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(180s);
    return EXIT_SUCCESS;
}
