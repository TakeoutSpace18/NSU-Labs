#ifndef LAB_2_GAME_OF_LIFE_FIELD_H
#define LAB_2_GAME_OF_LIFE_FIELD_H

#include <vector>
#include <cstddef>
#include <string>

class Field {
public:
    /// Create empty field with specified size
    Field(size_t width, size_t height);

    std::vector<bool> &operator[](size_t x);

private:
    std::vector<std::vector<bool>> field_data_;

};

#endif //LAB_2_GAME_OF_LIFE_FIELD_H
