#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "measurable_code.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define MEASURED_FUNC_ITERATIONS 200000
#define MEASURE_COUNT 10

struct elapsed_times {
    uint64_t user_time_ns;
    uint64_t system_time_ns;
};

void print_elapsed_times(FILE* file, struct elapsed_times* elapsed_times) {
    fprintf(file, "\n    user time: %lus %luns\n    system time: %lus %luns",
            elapsed_times->user_time_ns / 1000000000,
            elapsed_times->user_time_ns % 1000000000,
            elapsed_times->system_time_ns / 1000000000,
            elapsed_times->system_time_ns % 1000000000);
}

static inline struct elapsed_times get_elapsed_nanoseconds(struct tms* begin_tms, struct tms* end_tms) {
    uint64_t clocks_per_sec = sysconf(_SC_CLK_TCK);
    struct elapsed_times ret = {
        .user_time_ns = (end_tms->tms_utime - begin_tms->tms_utime) * 1000000000 / clocks_per_sec,
        .system_time_ns = (end_tms->tms_stime - begin_tms->tms_stime) * 1000000000 / clocks_per_sec
    };
    return ret;
}

static struct elapsed_times measure_time(double *output_buffer) {
    struct tms begin_tms, end_tms;

    times(&begin_tms);
    measured_function(output_buffer, MEASURED_FUNC_ITERATIONS);
    times(&end_tms);

    return get_elapsed_nanoseconds(&begin_tms, &end_tms);
}

int main(void) {
    printf("measuring code execution time...\n");

    FILE* output = fopen("results.txt", "w");
    double* output_buffer = (double*)malloc(sizeof(double) * MEASURED_FUNC_ITERATIONS);

    struct elapsed_times min_time = {UINT64_MAX, UINT64_MAX};

    for (int i = 0; i < MEASURE_COUNT; ++i) {
        struct elapsed_times measured_time = measure_time(output_buffer);

        min_time.system_time_ns = MIN(measured_time.system_time_ns, min_time.system_time_ns);
        min_time.user_time_ns = MIN(measured_time.user_time_ns, min_time.user_time_ns);

        for (size_t j = 0; j < MEASURED_FUNC_ITERATIONS; ++j) {
            fprintf(output, "%f\n", output_buffer[j]);
        }

//        printf("\n(%i) function execution time is:", i);
//        print_elapsed_times(stdout, &measured_time);
//        fflush(stdout);
    }

    printf("\nminimum time:");
    print_elapsed_times(stdout, &min_time);
    printf("\n");

    fclose(output);
    free(output_buffer);
    return EXIT_SUCCESS;
}