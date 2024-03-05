#include "SLESolverEffective.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <omp.h>
#include <sched.h>
#include <set>
#include <fmt/ranges.h>

void SLESolverEffective::Solve(const DataType* matA, const DataType* vecB, DataType* foundX, const std::size_t N, const DataType eps)
{
    std::vector<DataType> vecAxMinusB(N);
    DataType vecAxMinusBLength = 0;
    DataType vecBLength = 0;

    // set initial X value
    std::fill_n(foundX, N, 0);

    std::set<int> cpus;

    #pragma omp parallel proc_bind(close)
    {
        #pragma omp critical
        cpus.insert(sched_getcpu());

        #pragma omp for reduction(+:vecBLength) SCHEDULE
        for (std::size_t i = 0; i < N; ++i)
        {
            vecBLength += vecB[i] * vecB[i];
        }

        while (true)
        {
            #pragma omp single
            std::fill_n(vecAxMinusB.begin(), N, 0);

            vecAxMinusBLength = 0;
            #pragma omp barrier

            #pragma omp for reduction(+: vecAxMinusBLength) SCHEDULE
            for (std::size_t i = 0; i < N; ++i) // row
            {
                for (std::size_t j = 0; j < N; ++j) // col
                {
                    vecAxMinusB[i] += matA[i * N + j] * foundX[j];
                }

                vecAxMinusB[i] -= vecB[i];
                vecAxMinusBLength += vecAxMinusB[i] * vecAxMinusB[i];

                vecAxMinusB[i] *= sParam;
            }

            #pragma omp for SCHEDULE
            for (std::size_t i = 0; i < N; ++i)
            {
                foundX[i] -= vecAxMinusB[i];
            }

            if(vecAxMinusBLength / vecBLength < eps * eps)
            {
                break;
            }

            #pragma omp barrier
        }
    }

    fmt::print("cpus used: {}\n", cpus);
}
