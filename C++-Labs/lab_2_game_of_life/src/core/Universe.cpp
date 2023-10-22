#include "Universe.h"

void Universe::tick() {
    Field old_field = field_;

    for (size_t x = 0; x < this->width(); ++x) {
        for (size_t y = 0; y < this->height(); ++y) {
            uint8_t neighbours_count = old_field.getNeighboursCount(x, y);
            if (neighbours_to_born_.contains(neighbours_count)) {
                field_[x][y] = true;
            }
            else if (!neighbours_to_survive_.contains(neighbours_count)) {
                field_[x][y] = false;
            }
        }
    }
}

size_t Universe::height() const {
    return field_.height();
}

size_t Universe::width() const {
    return field_.width();
}

Universe::Universe(const Field &field) : field_(field){

}

Universe::Universe(Field &&field) : field_(field){

}

