cmake_minimum_required(VERSION 3.5.1)
project(cpu_time_measure C)

add_executable(src/clock_gettime_measure.c src/measurable_code.c)
target_compile_options(clock_gettime_measure PRIVATE -O0)
add_executable(src/rdtsc_user_space.c src/measurable_code.c)
target_compile_options(rdtsc_user_space PRIVATE -O0)
