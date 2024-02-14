#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <gnuplot-iostream.h>

#include "SLESolver.h"

std::vector<SLESolver::DataType> loadData(const std::string& filename, std::size_t size)
{
    std::vector<SLESolver::DataType> loadedData(size);
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to load data file " + filename);
    }
    input.read(reinterpret_cast<char*>(loadedData.data()), sizeof(SLESolver::DataType) * size);
    return loadedData;
}

void saveData(const std::vector<SLESolver::DataType>& dataToSave, const std::string& filename)
{
    std::ofstream output(filename, std::ios::binary);
    output.write(reinterpret_cast<const char*>(dataToSave.data()), sizeof(SLESolver::DataType) * dataToSave.size());
}

int benchmark()
{
    std::size_t N = 50*50;

    auto matA = loadData("matA.bin", N*N);
    auto vecB = loadData("vecB.bin", N);

    std::vector<SLESolver::DataType> foundVecX(N);

    SLESolver::Solve(matA.data(), vecB.data(), foundVecX.data(), N, 1e-3);

    saveData(foundVecX, "foundVecX.bin");
    return EXIT_SUCCESS;
}
