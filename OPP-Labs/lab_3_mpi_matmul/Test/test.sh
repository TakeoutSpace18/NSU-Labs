#!/bin/bash

MULTIPLIER=../cmake-build-debug/MatrixMultiplier/MatrixMultiplier
TESTER=../cmake-build-debug/MultiplicationTester/MultiplicationTester

n1=4
n2=8
n3=16

$TESTER generate matA.bin $n1 $n2
$TESTER generate matB.bin $n2 $n3

$TESTER print matA.bin $n1 $n2
$TESTER print matB.bin $n2 $n3

mpirun -np 8 $MULTIPLIER matA.bin matB.bin matResult.bin $n1 $n2 $n3 2 4

$TESTER verify matA.bin matB.bin matResult.bin $n1 $n2 $n3