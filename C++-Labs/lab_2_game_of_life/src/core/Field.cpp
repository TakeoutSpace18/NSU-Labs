#include "Field.h"

Field::Field(size_t width, size_t height)
 : field_data_(width, std::vector<bool>(height, false)){
}

std::vector<bool> &Field::operator[](size_t x) {
    return field_data_[x];
}
