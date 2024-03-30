#include "MultiplicationTester.h"

#include <random>
#include <algorithm>
#include <cstdlib>

#include <fmt/color.h>
#include <eigen3/Eigen/Core>

#include "VectorSerialization.hpp"

int MultiplicationTester::Run(int argc, char** argv)
{
    if (argc < 2)
    {
        PrintUsageInfo();
        return EXIT_FAILURE;
    }

    if (std::strcmp(argv[1], "generate") == 0 && argc == 6)
    {
        GenerateRandomMatrix(argv[2], std::atoi(argv[3]), std::atoi(argv[4]));
        fmt::print(fg(fmt::color::green), "Generated matrix written to file {}", argv[2]);
        return EXIT_SUCCESS;
    }
    else if (std::strcmp(argv[1], "verify") == 0 && argc == 8)
    {
        bool correct = VerifyMultiplicationCorrectness(argv[2], argv[3], argv[4],
            std::atoi(argv[5]), std::atoi(argv[6]), std::atoi(argv[7]));

        if (correct)
        {
            fmt::print(fg(fmt::color::green), "Correct!");
        }
        else
        {
            fmt::print(fg(fmt::color::red), "Error!");
        }
        return EXIT_SUCCESS;
    }
    else
    {
        fmt::print(fg(fmt::color::red), "incorrect usage\n");
        PrintUsageInfo();
        return EXIT_FAILURE;
    }
}

void MultiplicationTester::GenerateRandomMatrix(const std::string& filename, int n1, int n2)
{
    std::vector<ValueType> matrix(n1 * n2);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distr(0, 100.0);

    std::ranges::generate(matrix, [&]() {
        return distr(generator);
    });

    SaveVectorToFile(matrix, filename);
}

bool MultiplicationTester::VerifyMultiplicationCorrectness(const std::string& matAFilename,
    const std::string& matBFilename, const std::string& matResultFilename, int n1, int n2, int n3)
{
    std::vector<ValueType> matA = LoadVectorFromFile<ValueType>(matAFilename, n1 * n2);
    std::vector<ValueType> matB = LoadVectorFromFile<ValueType>(matBFilename, n2 * n3);
    std::vector<ValueType> matResult = LoadVectorFromFile<ValueType>(matResultFilename, n1 * n3);

    using Mat = Eigen::MatrixX<ValueType>;

    Mat matAEigen = Eigen::Map<const Mat>(matA.data(), n1, n2);
    Mat matBEigen = Eigen::Map<const Mat>(matB.data(), n2, n3);
    Mat matResultEigen = Eigen::Map<const Mat>(matResult.data(), n1, n3);

    Mat trustedResult = matAEigen * matBEigen;

    return matResultEigen.isApprox(trustedResult);
}

void MultiplicationTester::PrintUsageInfo()
{
    fmt::print("usage:\n");
    fmt::print("\tgenerate <filename> <n1> <n2>\n");
    fmt::print("\tverify <matA> <matB> <matResult> <n1> <n2> <n3>\n");

}