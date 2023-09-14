cmake_minimum_required(VERSION 3.5.1)
project(cpu_time_measure C)

add_executable(clock_gettime_measure src/clock_gettime_measure.c src/measurable_code.c)
target_compile_options(clock_gettime_measure PRIVATE -O0)
add_executable(rdtsc_user_space src/rdtsc_user_space.c src/measurable_code.c)
target_compile_options(rdtsc_user_space PRIVATE -O0)