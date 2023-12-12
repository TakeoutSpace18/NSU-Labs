#include "CacheAssociativityMeasure.h"

#include <random>
#include <cassert>
#include <fstream>
#include <thread>
#include <immintrin.h>

CacheAssociativityMeasure::CacheAssociativityMeasure(const Config &config)
    : m_config(config),
      m_array(new size_t[(config.max_banks * config.step_offset_bytes) / sizeof(*m_array) + 1])
{
}

CacheAssociativityMeasure::~CacheAssociativityMeasure()
{
    delete[] m_array;
}

void CacheAssociativityMeasure::doMeasurings(const std::string &output_filename)
{
    std::ofstream data_output(output_filename);

    for (std::size_t banks = 1; banks < m_config.max_banks; ++banks)
    {
        fillArray(m_config.num_sets, banks);
        auto time = measureTraverseTime(m_config.num_sets, banks, 5000);
        data_output << banks << "," << time << '\n';
    }

    data_output.close();
}

void CacheAssociativityMeasure::fillArray(std::size_t num_sets, std::size_t num_banks)
{
    const std::size_t step_offset_elems = m_config.step_offset_bytes / sizeof(*m_array);
    for (std::size_t i = 0; i < num_sets; ++i)
    {
        for (std::size_t j = 0; j < num_banks - 1; ++j)
        {
            m_array[i + j * step_offset_elems] = i + (j + 1) * step_offset_elems;
        }
        m_array[i + num_banks * step_offset_elems] = i + 1;
    }
    m_array[num_sets + num_banks * step_offset_elems] = 0;
}

void CacheAssociativityMeasure::traverseArray(std::size_t num_sets, std::size_t num_banks)
{
    std::size_t k = 0;
    for (std::size_t i = 0; i < num_sets * num_banks; ++i)
    {
        k = m_array[k];
    }
}

float CacheAssociativityMeasure::measureTraverseTime(std::size_t num_sets, std::size_t num_banks,
                                                     std::size_t traverse_repeats)
{
    std::size_t min_ticks = std::numeric_limits<std::size_t>::max();
    for (std::size_t i = 0; i < traverse_repeats; ++i)
    {
        std::size_t start = _rdtsc();
        traverseArray(num_sets, num_banks);
        std::size_t stop = _rdtsc();

        std::size_t elapsed_ticks = stop - start;
        min_ticks = std::min(min_ticks, elapsed_ticks);
    }
    return static_cast<float>(min_ticks) / (num_sets * num_banks);
}
