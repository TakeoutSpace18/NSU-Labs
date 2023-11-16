#include "Field.h"

#include <stdexcept>
#include <algorithm>

Field::Field(size_t width, size_t height)
 : field_data_(validateSize(width), std::vector<bool>(validateSize(height), false)),
 width_(width), height_(height){
}

const std::vector<bool> &Field::operator[](size_t x) const {
    return field_data_[x];
}
std::vector<bool> &Field::operator[](size_t x) {
    return field_data_[x];
}

uint8_t Field::getNeighboursCount(size_t x, size_t y) const {
    size_t leftColumn = (x + this->width() - 1) % this->width();
    size_t rightColumn = (x + 1) % this->width();
    size_t topRow = (y + this->height() - 1) % this->height();
    size_t bottomRow = (y + 1) % this->height();

    uint8_t neighboursCount = 0;
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
    return width_;
}

size_t Field::height() const {
    return height_;
}

size_t Field::validateSize(size_t size) {
    if (size < 3) {
        throw std::invalid_argument("Field size must be >= 3");
    }
    return size;
}

Field::Field(std::initializer_list<std::initializer_list<bool>> matrix)
: width_(validateSize(matrix.size())), height_(validateSize(matrix.begin()->size())){
    std::copy(matrix.begin(), matrix.end(), std::back_inserter(field_data_));
}

bool operator==(const Field &lhs, const Field &rhs) {
    return lhs.field_data_ == rhs.field_data_;
}

Field &Field::resize(size_t width, size_t height) {
    field_data_.resize(validateSize(width));
    for (auto &col : field_data_) {
        col.resize(validateSize(height));
    }
    width_ = width;
    height_ = height;

    return *this;
}
