#!/bin/bash

MULTIPLIER=../cmake-build-debug/MatrixMultiplier/MatrixMultiplier
TESTER=../cmake-build-debug/MultiplicationTester/MultiplicationTester

n1=3
n2=5
n3=7

$TESTER generate matA.bin $n1 $n2
$TESTER generate matB.bin $n2 $n3

mpirun -np 1 $MULTIPLIER matA.bin matB.bin matResult.bin $n1 $n2 $n3

$TESTER verify matA.bin matB.bin matResult.bin $n1 $n2 $n3
