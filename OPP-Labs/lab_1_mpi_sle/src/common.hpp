#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

void loadData(float* buf, const std::string& filename, std::size_t size)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to load data file " + filename);
    }
    input.read(reinterpret_cast<char *>(buf), sizeof(float) * size);
}

void saveData(const std::vector<float>& dataToSave, const std::string& filename)
{
    std::ofstream output(filename, std::ios::binary);
    output.write(reinterpret_cast<const char *>(dataToSave.data()), sizeof(float) * dataToSave.size());
}

#endif //COMMON_UTILS_H
