#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <fstream>

std::vector<float> loadData(std::size_t size, const std::string& filename)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to load data file " + filename);
    }
    std::vector<float> loaded(size);
    input.read(reinterpret_cast<char *>(loaded.data()), sizeof(float) * size);
    return loaded;
}

void saveData(const std::vector<float>& dataToSave, const std::string& filename)
{
    std::ofstream output(filename, std::ios::binary);
    output.write(reinterpret_cast<const char *>(dataToSave.data()), sizeof(float) * dataToSave.size());
}

#endif //COMMON_UTILS_H
