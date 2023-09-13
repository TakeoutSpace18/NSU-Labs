#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/preempt.h>

#include "measurable_code.h"


uint64_t measure_clock_cycles(void) {

}

int init_module(void)
{
    unsigned long flags;
    uint64_t cycles_low_begin, cycles_high_begin, cycles_low_end, cycles_high_end;
    uint64_t start, end;
    uint64_t measured_cycles;

    printk(KERN_INFO "cpu_time_measure: loading...\n");

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

    start = ( (cycles_high_begin << 32) | cycles_low_begin );
    end = ( (cycles_high_end << 32) | cycles_low_end );
    measured_cycles = end - start;

    printk(KERN_INFO "cpu_time_measure: function execution time is %llu clock cycles", measured_cycles);

    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "cpu_time_measure: closing...\n");
}

MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");