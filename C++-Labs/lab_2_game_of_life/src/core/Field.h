#ifndef LAB_2_GAME_OF_LIFE_FIELD_H
#define LAB_2_GAME_OF_LIFE_FIELD_H

#include <vector>
#include <cstddef>
#include <string>

class Field {
public:
    /// Create field with zero size
    Field() = default;

    /// Create empty field with specified size
    Field(size_t width, size_t height);
    Field(std::initializer_list<std::initializer_list<bool>> matrix);

    const std::vector<bool> &operator[](size_t x) const;
    std::vector<bool> &operator[](size_t x);

    Field& resize(size_t width, size_t height);

    /// Get cell neighbours count in Moore neighbourhood
    [[nodiscard]] uint8_t getNeighboursCount(size_t x, size_t y) const;

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;

    friend bool operator==(const Field& lhs, const Field& rhs);

private:
    static size_t validateSize(size_t size);

    std::vector<std::vector<bool>> field_data_;
};

#endif //LAB_2_GAME_OF_LIFE_FIELD_H
