#ifndef CACHESIZEMEASURE_H
#define CACHESIZEMEASURE_H

#include <chrono>
#include <cstdint>

class CacheSizeMeasure {
public:
    struct Config {
        size_t array_increase_step;
        size_t measuring_count;
        int traverse_repeat_count;
    };

    CacheSizeMeasure() = delete;
    CacheSizeMeasure(const Config& config);
    ~CacheSizeMeasure();

    enum class TraverseType {
        Direct,
        Reverse,
        Random
    };

    void doMeasurings(const std::string&output_filename, TraverseType type);

private:

    template <class F, class... Args>
    auto measureFunctionRuntime(F func, Args&&... args) {
        using clock = std::chrono::high_resolution_clock;

        auto start = clock::now();
        func(std::forward<Args>(args)...);
        auto stop = clock::now();
        return stop - start;
    }

    size_t array_increase_step_ = 40;
    size_t measuring_count_ = 530;
    int traverse_repeat_count_ = 50;

    size_t *array_;

    void prepareForTraverse(size_t N, TraverseType type);
    void traverseArray(size_t N, uint32_t repeat_count) const;
};


#endif //CACHESIZEMEASURE_H