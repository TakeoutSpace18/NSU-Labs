#ifndef CACHESIZEMEASURE_H
#define CACHESIZEMEASURE_H

#include <chrono>
#include <cstdint>


class CacheSizeMeasure {
public:
    CacheSizeMeasure();
    ~CacheSizeMeasure();

    int run();

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

    static constexpr size_t array_increase_step = 10;
    static constexpr size_t measuring_count = 150;

    size_t *array_;

    void prepareForTraverse(size_t N, TraverseType type);
    void traverseArray(size_t N, uint32_t repeat_count) const;
};


#endif //CACHESIZEMEASURE_H
