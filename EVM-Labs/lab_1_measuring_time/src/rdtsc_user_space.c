#include <stdio.h>
#include <stdint.h>

#include "measurable_code.h"
#include "common.h"

// for correct measuring execute "taskset --cpu-list 0 ./rdtsc_user_space"

static uint64_t measure_clock_cycles(void) {
    uint64_t cycles_low_begin, cycles_high_begin, cycles_low_end, cycles_high_end;
    uint64_t start, end;
    uint64_t measured_cycles;

    asm volatile (
            "lfence\n\t"
            "mfence\n\t"
            "rdtsc\n\t"
            "mov %%rdx, %0\n\t"
            "mov %%rax, %1\n\t"
            "lfence"

            : "=r" (cycles_high_begin), "=r" (cycles_low_begin));

    measured_function(NULL, MEASURED_FUNC_ITERATIONS);

    asm volatile (
            "lfence\n\t"
            "mfence\n\t"
            "rdtsc\n\t"
            "mov %%rdx, %0\n\t"
            "mov %%rax, %1\n\t"
            "lfence"

            : "=r" (cycles_high_end), "=r" (cycles_low_end));

    start = ( (cycles_high_begin << 32) | cycles_low_begin );
    end = ( (cycles_high_end << 32) | cycles_low_end );
    measured_cycles = end - start;

    return measured_cycles;
}

int main(void)
{
    printf("measuring with rdtsc in user-space...\n");

    uint64_t measure_results[MEASURE_COUNT];
    uint64_t average_clock_cycles = 0;
    uint64_t max_diff = 0;

    for (int i = 0; i < MEASURE_COUNT; ++i) {
        uint64_t measured_cycles = measure_clock_cycles();
        measure_results[i] = measured_cycles;
        average_clock_cycles += measured_cycles;

        for (int j = 0; j < i; ++j) {
            uint64_t diff = DIFF(measure_results[j], measured_cycles);
            if (diff > max_diff) {
                max_diff = diff;
            }
        }

        uint64_t elapsed_ns = CYCLES_TO_NANOSECONDS(measured_cycles);
        printf("(%i) function execution time is %lu clock cycles, %lis %lins\n", i, measured_cycles, elapsed_ns / 1000000000, elapsed_ns % 1000000000);
    }

    average_clock_cycles /= MEASURE_COUNT;

    uint64_t average_ns = CYCLES_TO_NANOSECONDS(average_clock_cycles);
    printf("average clock_cycles - %lu, average time - %lis %lins\n", average_clock_cycles, average_ns / 1000000000, average_ns % 1000000000);
    printf("max difference - %lu\n", max_diff);

    return 0;
}