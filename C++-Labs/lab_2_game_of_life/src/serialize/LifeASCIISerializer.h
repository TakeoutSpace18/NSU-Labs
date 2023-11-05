#ifndef LAB_2_GAME_OF_LIFE_LIFEASCIISERIALIZER_H
#define LAB_2_GAME_OF_LIFE_LIFEASCIISERIALIZER_H

#include <filesystem>
#include <memory>

#include "core/Universe.h"

class LifeASCIISerializer {
public:
    static void writeToFile(const std::filesystem::path& path, const Universe& universe);
    static std::unique_ptr<Universe> readFromFile(const std::filesystem::path& path);
};


#endif //LAB_2_GAME_OF_LIFE_LIFEASCIISERIALIZER_H
