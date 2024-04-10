#ifndef SLESOLVERMPI_H
#define SLESOLVERMPI_H
#include <vector>
#include <mpi.h>


class SLESolverMPI {
public:

    SLESolverMPI(const MPI::Comm& comm);

    int measureTimeWithRepeats(int argc, char** argv) const;
    int compute() const;

private:
    struct DataSplitInfo {
        std::vector<int> sizes;
        std::vector<int> offsets;
    };

private:
    [[nodiscard]] DataSplitInfo generateLinesSplitInfo(int N) const;

    std::vector<float> scatterMatA(const std::vector<float>& matA, const DataSplitInfo& linesSplitInfo, int N) const;

    void allGatherVecX(std::vector<float>& vecXChunk, std::vector<float>& vecXResult, const DataSplitInfo& linesSplitInfo) const;

    static float scalarProduct(const float* a, const float* b, int N);

private:
    const MPI::Comm& mWorldComm;
    int mRank;
    int mWorldSize;
};



#endif //SLESOLVERMPI_H
