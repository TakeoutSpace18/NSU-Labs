#ifndef CACHESIZEMEASURE_H
#define CACHESIZEMEASURE_H

#include <chrono>
#include <cstdint>

class CacheSizeMeasure {
public:
    CacheSizeMeasure();
    ~CacheSizeMeasure();

    int launch();

    void openGNUPlot();
    void doMeasurings();

private:
    enum class TraverseType {
        Direct,
        Reverse,
        Random
    };

    template <class F, class... Args>
    auto measureFunctionRuntime(F func, Args&&... args) {
        using clock = std::chrono::high_resolution_clock;

        auto start = clock::now();
        func(std::forward<Args>(args)...);
        auto stop = clock::now();
        return stop - start;
    }

    static constexpr size_t array_increase_step = 5000;
    static constexpr size_t measuring_count = 1000;

    size_t *array_;
    const char* output_filename_ = "measured_points.dat";
    TraverseType traverse_type_ = TraverseType::Random;

    void prepareForTraverse(size_t N, TraverseType type);
    void traverseArray(size_t N, uint32_t repeat_count) const;
};


#endif //CACHESIZEMEASURE_H
