#define TSC_FREQUENCY 3302400 // KHz (can be obtained by "sudo dmesg | grep tsc")
#define MEASURE_COUNT 10

#define MEASURED_FUNC_ITERATIONS 15000000

#define CYCLES_TO_NANOSECONDS(cycles) ((cycles) * 1000000 / TSC_FREQUENCY)

#define DIFF(a, b) a > b ? a - b : b - a