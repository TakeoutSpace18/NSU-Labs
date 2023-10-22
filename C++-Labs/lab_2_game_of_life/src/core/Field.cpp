#include "Field.h"

#include <stdexcept>

Field::Field(size_t width, size_t height)
 : field_data_(validateSize(width), std::vector<bool>(validateSize(height), false)){
}

std::vector<bool> &Field::operator[](size_t x) {
    return field_data_[x];
}

uint32_t Field::getNeighboursCount(size_t x, size_t y) const {
    size_t leftColumn = (x + this->width() - 1) % this->width();
    size_t rightColumn = (x + 1) % this->width();
    size_t topRow = (y + this->height() - 1) % this->height();
    size_t bottomRow = (y + 1) % this->height();

    uint32_t neighboursCount = 0;
    neighboursCount += field_data_[leftColumn][topRow];
    neighboursCount += field_data_[leftColumn][y];
    neighboursCount += field_data_[leftColumn][bottomRow];

    neighboursCount += field_data_[x][topRow];
    neighboursCount += field_data_[x][bottomRow];

    neighboursCount += field_data_[rightColumn][topRow];
    neighboursCount += field_data_[rightColumn][y];
    neighboursCount += field_data_[rightColumn][bottomRow];

    return neighboursCount;
}

size_t Field::width() const {
    return this->field_data_.size();
}

size_t Field::height() const {
    return this->field_data_[0].size();
}

size_t Field::validateSize(size_t size) {
    if (size < 3) {
        throw std::invalid_argument("Field size must be >= 3");
    }
    return size;
}
