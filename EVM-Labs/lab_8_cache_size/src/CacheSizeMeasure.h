#ifndef CACHESIZEMEASURE_H
#define CACHESIZEMEASURE_H

#include <chrono>
#include <cstdint>

class CacheSizeMeasure {
public:
    struct Config {
        size_t array_increase_step_start;
        double array_increase_step_multiplier;
        size_t array_max_size_bytes;
        uint32_t traverse_repeat_count;
        uint32_t measure_repeats;

    };

    CacheSizeMeasure() = delete;
    explicit CacheSizeMeasure(const Config& config);
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

        const auto start = clock::now();
        func(std::forward<Args>(args)...);
        const auto stop = clock::now();
        return stop - start;
    }

    Config config_;
    size_t *array_;

    void prepareForTraverse(size_t N, TraverseType type) const;
    void traverseArray(size_t N, uint32_t repeat_count) const;
};


#endif //CACHESIZEMEASURE_H
