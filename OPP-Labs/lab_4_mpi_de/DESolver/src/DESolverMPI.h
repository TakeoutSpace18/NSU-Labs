#ifndef DESOLVERMPI_H
#define DESOLVERMPI_H
#include <array>
#include <string>
#include <vector>
#include <functional>

#include <mpi.h>
#include <optional>

#define COORDS(x, y, z, gridSize) ((x) * gridSize[1] * gridSize[2] + (y) * gridSize[2] + (z))

class DESolverMPI
{
public:
    using ValueType = double;
    MPI::Datatype MPIValueType = MPI::DOUBLE;

    using NextIterationCallback = std::function<ValueType(
        const std::array<ValueType, 3>& stepSize,
        ValueType x, ValueType y, ValueType z, // point in global coordinates that is currently updated
        int i, int j, int k, // data array indexes of the current cell
        const ValueType* data,
        std::array<int, 3>& gridSize)>;

    using Func = std::function<ValueType(ValueType x, ValueType y, ValueType z)>;

    struct SolutionProperties
    {
        std::array<ValueType, 3> areaStart;
        std::array<ValueType, 3> areaSize;
        std::array<int, 3> gridSize;
    };

    explicit DESolverMPI(
        const MPI::Comm& comm,
        const SolutionProperties& properties);

    void Solve(
        NextIterationCallback nextIterFn,
        Func boundaryConditionsFunc,
        ValueType innerStartValue,
        ValueType eps);

    ValueType GetMaxDelta(Func referenceFunc);

private:
    struct SplitInfo
    {
        std::vector<int> sizes;
        std::vector<int> offsets;
    };

private:
    std::array<ValueType, 3> GetStepSize(const SolutionProperties& properties);

    SplitInfo GenerateSplitInfo(std::array<int, 3> gridSize);

    void SetInitialValues(ValueType* chunk,
                          Func boundaryConditionsFunc,
                          ValueType innerStartValue);

    ValueType UpdatePlane(int i, int offset,
                          const ValueType* srcChunk,
                          ValueType* dstChunk,
                          NextIterationCallback nextIterFn);

private:
    const MPI::Comm& mCommWorld;
    int mProcRank;
    int mWorldSize;

    SplitInfo mSplitInfo;
    SolutionProperties mProperties;
    std::array<ValueType, 3> mStepSize;


    std::optional<std::vector<ValueType>> mSolveResultExtended;
    ValueType* mSolveResultDataView;
};


#endif //DESOLVERMPI_H
