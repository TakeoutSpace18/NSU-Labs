#ifndef LAB_2_GAME_OF_LIFE_UNIVERSESERIALIZERBASE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSESERIALIZERBASE_H

#include <memory>
#include <filesystem>

#include "core/Universe.h"

class UniverseSerializerBase {
    virtual void write(std::filesystem::path, const Universe &universe) = 0;
    virtual std::unique_ptr<Universe> read(std::filesystem::path file_path) = 0;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSESERIALIZERBASE_H
