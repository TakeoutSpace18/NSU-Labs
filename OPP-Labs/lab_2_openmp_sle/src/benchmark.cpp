#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <gnuplot-iostream.h>
#include <omp.h>
#include <filesystem>
#include <vector>

#include "SLESolver.h"

constexpr SLESolver::DataType PRECISION = 1e-3;
constexpr int MEASURE_REPEATS = 3;

template<class F, class... Args>
auto measureFunctionRuntime(F func, Args&&... args)
{
    using clock = std::chrono::high_resolution_clock;

    const auto start = clock::now();
    func(std::forward<Args>(args)...);
    const auto stop = clock::now();
    return stop - start;
}

template<class F, class... Args>
auto measureFunctionRuntimeWithRepeats(int repeats, F func, Args&&... args)
{
    auto minTime = measureFunctionRuntime(func, std::forward<Args>(args)...);
    for (int i = 1; i < repeats; ++i)
    {
        auto time = measureFunctionRuntime(func, std::forward<Args>(args)...);
        minTime = std::min(time, minTime);
    }
    return minTime;
}

std::vector<SLESolver::DataType> loadData(const std::string& filename, std::size_t size)
{
    std::vector<SLESolver::DataType> loadedData(size);
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to load data file " + filename);
    }
    input.read(reinterpret_cast<char *>(loadedData.data()), sizeof(SLESolver::DataType) * size);
    return loadedData;
}

void saveData(const std::vector<SLESolver::DataType>& dataToSave, const std::string& filename)
{
    std::ofstream output(filename, std::ios::binary);
    output.write(reinterpret_cast<const char *>(dataToSave.data()), sizeof(SLESolver::DataType) * dataToSave.size());
}

int benchmark()
{
    std::size_t N = 50 * 50;
    int maxThreads = omp_get_max_threads();

    std::vector<std::pair<int, double>> threadsToTimeGraph;
    std::vector<std::pair<int, double>> speedupGraph;
    std::vector<std::pair<int, double>> parallelEfficiencyGraph;
    threadsToTimeGraph.reserve(maxThreads);
    speedupGraph.reserve(maxThreads);
    parallelEfficiencyGraph.reserve(maxThreads);

    auto matA = loadData("matA.bin", N * N);
    auto vecB = loadData("vecB.bin", N);

    std::vector<SLESolver::DataType> foundVecX(N);

    std::int64_t singleThreadTime;
    std::int64_t minTime = std::numeric_limits<std::int64_t>::max();
    int numThreadsWithMinTime;
    for (int numThreads = 1; numThreads <= maxThreads; ++numThreads)
    {
        std::cout << "Computing with " << numThreads << " thread(s)... ";
        std::cout.flush();

        omp_set_num_threads(numThreads);
        auto elapsedTime = measureFunctionRuntimeWithRepeats(MEASURE_REPEATS, SLESolver::Solve, matA.data(),
                                                             vecB.data(), foundVecX.data(), N, PRECISION);
        std::int64_t elapsedTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
        std::cout << elapsedTimeMilliseconds << "ms\n";

        if (numThreads == 1)
        {
            singleThreadTime = elapsedTimeMilliseconds;
        }

        if (elapsedTimeMilliseconds < minTime)
        {
            minTime = elapsedTimeMilliseconds;
            numThreadsWithMinTime = numThreads;
        }

        threadsToTimeGraph.emplace_back(numThreads, elapsedTimeMilliseconds);
        speedupGraph.emplace_back(numThreads, (double)singleThreadTime / elapsedTimeMilliseconds);
        parallelEfficiencyGraph.emplace_back(numThreads, (double)singleThreadTime / (elapsedTimeMilliseconds * numThreads));
    }
    saveData(foundVecX, "foundVecX.bin");


    std::filesystem::create_directory("plot");
    Gnuplot gp("tee plot/script.gp | gnuplot -persist");
    gp << "set multiplot layout 2, 2\n";
    gp << "set label \"min time: " << minTime << "ms, threads: " << numThreadsWithMinTime << "\" at 25,8\n";
    gp << "plot" << gp.file1d(threadsToTimeGraph, "plot/rawTimeGraph.dat") << "with lines title 'raw time'\n";
    gp << "set size ratio -1\n";
    gp << "plot" << gp.file1d(speedupGraph, "plot/speedupGraph.dat") << "with lines title 'speedup'\n";
    gp << "set size noratio\n";
    gp << "plot" << gp.file1d(parallelEfficiencyGraph, "plot/parallelEfficiencyGraph.dat") << "with lines title 'parallel efficiency'\n";

    return EXIT_SUCCESS;
}


int main()
{
    return benchmark();
}