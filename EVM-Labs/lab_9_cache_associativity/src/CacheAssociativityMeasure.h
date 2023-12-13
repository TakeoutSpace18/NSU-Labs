#ifndef CACHESIZEMEASURE_H
#define CACHESIZEMEASURE_H

#include <chrono>
#include <cstdint>

class CacheAssociativityMeasure
{
public:
    struct Config
    {
        std::size_t step_offset_bytes;
        std::size_t max_banks;
        std::size_t num_sets;
        std::size_t traverse_repeats;
    };

    CacheAssociativityMeasure() = delete;

    explicit CacheAssociativityMeasure(const Config &config);

    ~CacheAssociativityMeasure();

    void doMeasurings(const std::string &output_filename);

private:

    Config m_config;
    volatile size_t* m_array;

    void fillArray(std::size_t num_sets, std::size_t num_banks);
    void traverseArray(std::size_t num_sets, std::size_t num_banks);

    float measureTraverseTime(std::size_t num_sets, std::size_t num_banks, std::size_t traverse_repeats);
};


#endif //CACHESIZEMEASURE_H
