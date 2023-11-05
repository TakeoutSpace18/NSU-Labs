#include "LifeASCIISerializer.h"

#include <fstream>

void LifeASCIISerializer::writeToFile(const std::filesystem::path& path, const Universe& universe) {
    std::ofstream output(path);
    output << "#Life 1.06\n";
    output << "#N " << universe.name() << "\n";
    output << "#R " << universe.rules().toMCellNotation() << "\n";

    const Field& field = universe.field();
    for (size_t x = 0; x < field.width(); ++x) {
        for (size_t y = 0; y < field.height(); ++y) {
            if (field[x][y]) {
                output << x << ' ' << y << '\n';
            }
        }
    }
}

std::unique_ptr<Universe> LifeASCIISerializer::readFromFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open file " + path.string());
    }

    std::string name;

    for (std::string line; std::getline(input, line); ) {
        if (line.starts_with("#N ")) {

        }
    }

    return std::unique_ptr<Universe>();
}
