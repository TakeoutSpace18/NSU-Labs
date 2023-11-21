#ifndef CACHESIZEMEASURE_H
#define CACHESIZEMEASURE_H

#include <chrono>
#include <cstdint>

class CacheSizeMeasure {
public:
    CacheSizeMeasure();
    ~CacheSizeMeasure();

    enum class TraverseType {
        Direct,
        Reverse,
        Random
    };

    void doMeasurings(const char* output_filename, TraverseType type);

private:

    template <class F, class... Args>
    auto measureFunctionRuntime(F func, Args&&... args) {
        using clock = std::chrono::high_resolution_clock;

        auto start = clock::now();
        func(std::forward<Args>(args)...);
        auto stop = clock::now();
        return stop - start;
    }

    static constexpr size_t array_increase_step = 4000;
    static constexpr size_t measuring_count = 530;
    static constexpr int traverse_repeat_count_ = 4;

    size_t *array_;

    void prepareForTraverse(size_t N, TraverseType type);
    void traverseArray(size_t N, uint32_t repeat_count) const;
};


#endif //CACHESIZEMEASURE_H
