#!/bin/bash
Options=$(cat ./optimization-options.txt)
for flags in $Options
do
  printf "\n$flags:\n"
  g++ src/times_measure.c src/measurable_code.c $flags -o times_measure
  ls -l times_measure
  ./times_measure

done