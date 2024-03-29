#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "measurable_code.h"
#include "common.h"

static inline uint64_t get_elapsed_nanoseconds(struct timespec* begin, struct timespec* end) {
    return 1000000000*(end->tv_sec - begin->tv_sec) + end->tv_nsec - begin->tv_nsec;
}

static uint64_t measure_time() {
    struct timespec begin_time, end_time;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &begin_time) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    measured_function(NULL, MEASURED_FUNC_ITERATIONS);

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &end_time)) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    return get_elapsed_nanoseconds(&begin_time, &end_time);
}

int main(void) {
    printf("measuring code execution time with clock_gettime()...\n");

    uint64_t measure_results[MEASURE_COUNT];
    uint64_t average_time = 0;
    uint64_t max_diff = 0;

    for (int i = 0; i < MEASURE_COUNT; ++i) {
        uint64_t measured_time = measure_time();
        measure_results[i] = measured_time;
        average_time += measured_time;

        for (int j = 0; j < i; ++j) {
            uint64_t diff = DIFF(measure_results[j], measured_time);
            if (diff > max_diff) {
                max_diff = diff;
            }
        }

        printf("(%i) function execution time is %lus %luns\n", i, measured_time / 1000000000, measured_time % 1000000000);
    }

    average_time /= MEASURE_COUNT;

    printf("average time - %lus %luns\n", average_time / 1000000000, average_time % 1000000000);
    printf("max difference - %lus %luns\n", max_diff / 1000000000, max_diff % 1000000000);

    return EXIT_SUCCESS;
}