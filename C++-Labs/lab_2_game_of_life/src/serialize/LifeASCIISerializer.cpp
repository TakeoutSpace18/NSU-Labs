#include "LifeASCIISerializer.h"

#include <fstream>

void LifeASCIISerializer::writeToFile(const std::filesystem::path& path, const Universe& universe) {
    std::ofstream output(path);
    output << "#Life 1.06\n";

}

std::unique_ptr<Universe> LifeASCIISerializer::readFromFile(const std::filesystem::path& path) {
    return std::unique_ptr<Universe>();
}
