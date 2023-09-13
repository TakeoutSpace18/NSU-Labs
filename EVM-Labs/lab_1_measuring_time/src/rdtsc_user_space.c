#include <stdio.h>
#include <stdint.h>

#include "measurable_code.h"

int main(void)
{
    uint64_t cycles_low_begin, cycles_high_begin, cycles_low_end, cycles_high_end;
    uint64_t start, end;
    uint64_t measured_cycles;

    printf("measuring with rdtsc in user-space...\n");

    asm volatile (
            "lfence\n\t"
            "mfence\n\t"
            "rdtsc\n\t"
            "mov %%rdx, %0\n\t"
            "mov %%rax, %1\n\t"
            "lfence"

            : "=r" (cycles_high_begin), "=r" (cycles_low_begin));

    measured_function(NULL, 15000000);

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

    printf("function execution time is %lu clock cycles\n", measured_cycles);

    return 0;
}