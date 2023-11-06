#ifndef LAB_2_GAME_OF_LIFE_LIFEASCIISERIALIZER_H
#define LAB_2_GAME_OF_LIFE_LIFEASCIISERIALIZER_H

#include <filesystem>
#include <memory>

#include "core/Universe.h"

class LifeASCIISerializer {
public:
    static void WriteToFile(const std::filesystem::path& path, const Universe& universe);
    static std::unique_ptr<Universe> ReadFromFile(const std::filesystem::path& path);

private:
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& line, const std::string& message);
    };
};


#endif //LAB_2_GAME_OF_LIFE_LIFEASCIISERIALIZER_H
