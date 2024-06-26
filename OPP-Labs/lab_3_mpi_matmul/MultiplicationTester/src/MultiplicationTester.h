#ifndef MULTIPLICATIONTESTER_H
#define MULTIPLICATIONTESTER_H
#include <string>
#include <vector>


class MultiplicationTester {
public:
     using ValueType = float;

    int Run(int argc, char** argv);

    void GenerateRandomMatrix(const std::string& filename, int n1, int n2);
    void GenerateIdentityMatrix(const std::string& filename, int n1);
    void GenerateSerialMatrix(const std::string& filename, int n1, int n2, int startNum);
    void PrintMatrix(const std::string& filename, int n1, int n2);

    bool VerifyMultiplicationCorrectness(
      const std::string& matAFilename,
      const std::string& matBFilename,
      const std::string& matResultFilename,
      int n1, int n2, int n3);

private:

    void PrintUsageInfo();
};



#endif //MULTIPLICATIONTESTER_H
