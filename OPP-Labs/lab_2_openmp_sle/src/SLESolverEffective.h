#ifndef SLESOLVEREFFECTIVE_H
#define SLESOLVEREFFECTIVE_H
#include <cstddef>

#include "SLESolver.h"

// this implementation has external "omp parallel" directive and less for loops than naive variant

class SLESolverEffective : public SLESolver {
public:

    static void Solve(const DataType* matA, const DataType* vecB, DataType* foundX, std::size_t N, DataType eps);

private:
    SLESolverEffective() = default;
};


#endif //SLESOLVEREFFECTIVE_H
