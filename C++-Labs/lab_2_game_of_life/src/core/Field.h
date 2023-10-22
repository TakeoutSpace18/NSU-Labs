#ifndef LAB_2_GAME_OF_LIFE_FIELD_H
#define LAB_2_GAME_OF_LIFE_FIELD_H

#include <vector>
#include <cstddef>
#include <string>

class Field {
public:
    Field() = delete;
    /// Create empty field with specified size
    Field(size_t width, size_t height);

    std::vector<bool> &operator[](size_t x);

    /// Get cell neighbours count in Moore neighbourhood
    [[nodiscard]] uint8_t getNeighboursCount(size_t x, size_t y) const;

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;

private:
    std::vector<std::vector<bool>> field_data_;
    static size_t validateSize(size_t size);
};

#endif //LAB_2_GAME_OF_LIFE_FIELD_H
