cmake_minimum_required(VERSION 3.5.1)
project(lab_2_optimizations C)

add_executable(times_measure src/times_measure.c src/measurable_code.c)
target_compile_options(times_measure PRIVATE -O2 -Wall -v)