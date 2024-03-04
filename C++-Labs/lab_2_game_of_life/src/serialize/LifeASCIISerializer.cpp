#include "LifeASCIISerializer.h"
#include "utils/Logger.h"

#include <fstream>

void LifeASCIISerializer::WriteToFile(const std::filesystem::path& path, const Universe& universe) {
    std::ofstream output(path);
    output << "#Life 1.06\n";
    output << "#N " << universe.name() << "\n";
    output << "#S " << universe.field().width() << " " << universe.field().height() << "\n";
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

std::unique_ptr<Universe> LifeASCIISerializer::ReadFromFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open file " + path.string());
    }

    size_t width = 0;
    size_t height = 0;

    bool name_found = false;
    bool rules_found = false;
    bool size_found = false;

    auto universe = std::make_unique<Universe>();

    for (std::string line; std::getline(input, line); ) {
        if (line.starts_with("#N ")) {
            name_found = true;
            universe->setName(line.substr(3, line.size()));
        }
        else if (line.starts_with("#R ")) {
            rules_found = true;
            try {
                universe->setRules(Rules::FromMCellNotation(line.substr(3, line.size())));
            } catch (std::exception &e) {
                throw ParseError(line, e.what());
            }
        }
        else if (line.starts_with("#S ")) {
            size_found = true;
            std::istringstream sstream(line.substr(3, line.size()));
            sstream >> width >> height;

            if (!sstream.fail()) {
                universe->resize(width, height);
            }
            else {
                throw ParseError(line, "failed to read field size");
            }
        }
        else if (!line.starts_with("#")){
            if (size_found) {
                size_t x, y;
                std::istringstream sstream(line);
                sstream >> x >> y;

                if (!sstream.fail()) {
                    if (x >= width || y >= height) {
                        logger << Logger::Warning << "Cell with coordinates (" << x << ", " << y << ") ignored. Out of field size bounds.";
                        continue;
                    }

                    if (universe->field()[x][y]) {
                        logger << Logger::Warning << "Cell with coordinates (" << x << ", " << y << ") listed multiple times in file " << path << '\n';
                    }
                    universe->field()[x][y] = true;
                }
                else {
                    throw ParseError(line, "failed to read cell coordinates");
                }
            }
            else {
                throw ParseError(line, "no field size specified before cell coordinates");
            }
        }
    }

    if (!name_found) {
        logger << Logger::Warning << "Universe name not found in file " << path << ". Used default.\n";
    }

    if (!rules_found) {
        logger << Logger::Warning << "Universe rules not found in file " << path << ". Used Conway game of life by default.\n";
    }

    return universe;
}

LifeASCIISerializer::ParseError::ParseError(const std::string& line, const std::string& message)
        : runtime_error("Error while parsing line \"" + line + "\": " + message) {

}
