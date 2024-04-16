#ifndef DESOLVERMPI_H
#define DESOLVERMPI_H
#include <array>
#include <string>
#include <vector>
#include <functional>

#include <mpi.h>

#define COORDS(x, y, z, gridSize) ((x) * gridSize[1] * gridSize[2] + (y) * gridSize[2] + (z))

class DESolverMPI {
public:
    using ValueType = double;
    MPI::Datatype MPIValueType = MPI::DOUBLE;

	using NextIterationCallback = std::function<ValueType(
			const std::array<ValueType, 3>& stepSize,
 			int i, int j, int k,
			const ValueType* data,
			std::array<int, 3>& gridSize)>;

	struct SolutionProperties {
		std::array<ValueType, 3> areaStart;
		std::array<ValueType, 3> areaSize;
		std::array<int, 3> gridSize;
	};

    explicit DESolverMPI(const MPI::Comm& comm);

    std::vector<ValueType> Solve(
		NextIterationCallback fnCallback,
		ValueType borderValue,
		ValueType innerValue,
        SolutionProperties properties,
		ValueType eps
	);

	std::vector<ValueType> CreateInitialData(
		std::array<int, 3> gridSize,
		ValueType borderValue,
		ValueType innerValue);

	ValueType ExaminePrecision(
		std::function<ValueType(int x, int y, int z)> referenceFunc,
		const std::vector<ValueType> &data,
		SolutionProperties properties);

private:
	struct SplitInfo {
		std::vector<int> sizes;
		std::vector<int> offsets;
	};

private:
	std::array<ValueType, 3> GetStepSize(const SolutionProperties& properties);

	SplitInfo GenerateSplitInfo(std::array<int, 3> gridSize);

    void PrintDebugInfo(SolutionProperties properties, SplitInfo splitInfo,
                        std::vector<DESolverMPI::ValueType> extendedChunk, int Ny, int Nz);

    void SetInitialValues(ValueType *chunk, ValueType borderValue,
                          ValueType innerValue, std::array<int, 3> gridSize, int xStart, int xEnd);

	ValueType UpdatePlane(int x, int offset, const ValueType* srcChunk, ValueType* dstChunk, NextIterationCallback fn);

private:
    const MPI::Comm& mCommWorld;
    int mProcRank;
    int mWorldSize;

	std::array<int, 3> mGridSize;
	std::array<ValueType, 3> mStepSize;
};


#endif //DESOLVERMPI_H
