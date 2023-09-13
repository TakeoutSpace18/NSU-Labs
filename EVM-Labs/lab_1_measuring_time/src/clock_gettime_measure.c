#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "measurable_code.h"

int main(void) {
    printf("measuring code execution time...\n");

    struct timespec begin_time, end_time;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &begin_time) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    measured_function(NULL, 15000000);

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &end_time)) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    uint64_t seconds_elapsed = end_time.tv_sec - begin_time.tv_sec;
    long nano_seconds_elapsed = 1000000000*(end_time.tv_sec - begin_time.tv_sec) + end_time.tv_nsec - begin_time.tv_nsec;

    printf("function execution time is %lu seconds or %li nano seconds\n", seconds_elapsed, nano_seconds_elapsed);

    return EXIT_SUCCESS;
}