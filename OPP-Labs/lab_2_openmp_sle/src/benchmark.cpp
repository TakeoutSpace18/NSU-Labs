#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <gnuplot-iostream.h>
#include <omp.h>
#include <filesystem>
#include <vector>

#include "SLESolver.h"
#include "SLESolverNaive.h"
#include "SLESolverEffective.h"

constexpr SLESolver::DataType PRECISION = 5e-2;
constexpr int MEASURE_REPEATS = 1;
constexpr auto SOLVER = SLESolverEffective::Solve;

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

struct Graphs {
    std::vector<std::pair<int, double>> threadsToTime;
    std::vector<std::pair<int, double>> speedup;
    std::vector<std::pair<int, double>> parallelEfficiency;

    std::int64_t singleThreadTime;
    std::int64_t minTime;
    int numThreadsWithMinTime;


    explicit Graphs(int maxThreads)
    {
        minTime = std::numeric_limits<std::int64_t>::max();

        speedup.reserve(maxThreads);
        parallelEfficiency.reserve(maxThreads);
        threadsToTime.reserve(maxThreads);
    }

    void addMeasure(int numThreads, std::int64_t time)
    {
        if (numThreads == 1)
        {
            singleThreadTime = time;
        }

        if (time < minTime)
        {
            minTime = time;
            numThreadsWithMinTime = numThreads;
        }

        threadsToTime.emplace_back(numThreads, time);
        speedup.emplace_back(numThreads, (double)singleThreadTime / time);
        parallelEfficiency.emplace_back(numThreads, (double)singleThreadTime / (time * numThreads));
    }

    void pipeToGnuplot()
    {
        std::filesystem::create_directory("plot");
        Gnuplot gp("tee plot/script.gp | gnuplot -persist");
        gp << "set multiplot layout 2, 2\n";
        gp << "set label \"min time: " << minTime << "ms, threads: " << numThreadsWithMinTime << "\" at 25,8\n";
        gp << "plot" << gp.file1d(threadsToTime, "plot/rawTimeGraph.dat") << "with lines title 'raw time'\n";
        gp << "set size ratio -1\n";
        gp << "plot" << gp.file1d(speedup, "plot/speedupGraph.dat") << "with lines title 'speedup'\n";
        gp << "set size noratio\n";
        gp << "plot" << gp.file1d(parallelEfficiency, "plot/parallelEfficiencyGraph.dat") << "with lines title 'parallel efficiency'\n";
    }
};

int benchmark()
{
    std::size_t N = 50 * 50;
    int maxThreads = omp_get_max_threads();

    Graphs graphs(maxThreads);

    auto matA = loadData("matA.bin", N * N);
    auto vecB = loadData("vecB.bin", N);

    std::vector<SLESolver::DataType> foundVecX(N);

    for (int numThreads = 1; numThreads <= maxThreads; ++numThreads)
    {
        std::cout << "Computing with " << numThreads << " thread(s)... ";
        std::cout.flush();

        omp_set_num_threads(numThreads);

        auto elapsedTime = measureFunctionRuntimeWithRepeats(MEASURE_REPEATS, SOLVER, matA.data(),
                                                             vecB.data(), foundVecX.data(), N, PRECISION);

        std::int64_t elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
        std::cout << elapsedTimeMs << "ms\n";

        graphs.addMeasure(numThreads, elapsedTimeMs);
        saveData(foundVecX, "foundVecX.bin");
    }


    graphs.pipeToGnuplot();
    system("python3 visualize.py foundVecX.bin");

    return EXIT_SUCCESS;
}

int main()
{
    return benchmark();
}