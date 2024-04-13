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
			const std::vector<ValueType>& data,
			std::array<int, 3> gridSize)>;

	struct SolutionProperties {
		std::array<int, 3> areaStart;
		std::array<int, 3> areaSize;
		std::array<int, 3> gridSize;
	};

    explicit DESolverMPI(const MPI::Comm& comm);

    std::vector<ValueType> Solve(
		NextIterationCallback fnCallback,
        const std::vector<ValueType> &initialData,
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


private:
    const MPI::Comm& mCommWorld;
    int mProcRank;
    int mWorldSize;
};


#endif //DESOLVERMPI_H
