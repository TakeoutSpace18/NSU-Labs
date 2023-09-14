#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/preempt.h>

#include "measurable_code.h"

#define MEASURE_COUNT 10
#define DIFF(a, b) a > b ? a - b : b - a

uint64_t measure_clock_cycles(void) {
    unsigned long flags;
    uint64_t cycles_low_begin, cycles_high_begin, cycles_low_end, cycles_high_end;
    uint64_t start, end;
    uint64_t measured_cycles;

    preempt_disable(); /*we disable preemption on our CPU*/
    raw_local_irq_save(flags); /*we disable hard interrupts on our CPU*/
    /*at this stage we exclusively own the CPU*/

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

    raw_local_irq_restore(flags); /*we enable hard interrupts on our CPU*/
    preempt_enable(); /*we enable preemption*/

    start = ((cycles_high_begin << 32) | cycles_low_begin);
    end = ((cycles_high_end << 32) | cycles_low_end);
    measured_cycles = end - start;

    return measured_cycles;
}

int init_module(void) {
    printk(KERN_INFO
    "cpu_time_measure: loading...\n");

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

        printk(KERN_INFO
        "cpu_time_measure: (%i) function execution time is %llu clock cycles", i, measured_cycles);
    }

    average_clock_cycles /= MEASURE_COUNT;

    printk(KERN_INFO
    "cpu_time_measure: average clock_cycles - %lu", average_clock_cycles);
    printk(KERN_INFO
    "cpu_time_measure: max difference - %lu", max_diff);


    return 0;
}

void cleanup_module(void) {
    printk(KERN_INFO
    "cpu_time_measure: closing...\n");
}

MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");