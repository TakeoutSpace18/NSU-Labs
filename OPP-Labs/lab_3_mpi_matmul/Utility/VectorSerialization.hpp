#ifndef VECTORSERIALIZATION_HPP
#define VECTORSERIALIZATION_HPP

#include <fstream>

template <typename T>
std::vector<T> LoadVectorFromFile(const std::string& filename, size_t valuesCount)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to open " + filename);
    }
    std::vector<T> loaded(valuesCount);
    const auto fileSize = static_cast<std::streamsize>(sizeof(T) * valuesCount);
    input.read(reinterpret_cast<char *>(loaded.data()), fileSize);
    if (!input.good())
    {
        throw std::runtime_error("Failed to read " + std::to_string(fileSize) + " bytes from " + filename);
    }
    return loaded;
}

template <typename T>
void SaveVectorToFile(const std::vector<T>& data, const std::string& filename)
{
    std::ofstream output(filename, std::ios::binary);
    const auto fileSize = static_cast<std::streamsize>(sizeof(T) * data.size());
    output.write(reinterpret_cast<const char *>(data.data()), fileSize);
}

#endif //VECTORSERIALIZATION_HPP
