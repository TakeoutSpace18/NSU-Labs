#include "Universe.h"

#include <ranges>

const Field& Universe::tick() {
    for (size_t x = 0; x < this->width(); ++x) {
        for (size_t y = 0; y < this->height(); ++y) {
            uint8_t neighbours_count = main_field_.getNeighboursCount(x, y);
            if (neighbours_to_born_.contains(neighbours_count)) {
                twin_field_[x][y] = true;
            }
            else if (neighbours_to_survive_.contains(neighbours_count)) {
                twin_field_[x][y] = main_field_[x][y];
            }
            else {
                twin_field_[x][y] = false;
            }
        }
    }
    std::swap(main_field_, twin_field_);
    return this->main_field_;
}

const Field &Universe::tick_n(size_t n) {
    for (size_t _ : std::views::iota(0ul, n)) {
        this->tick();
    }
    return this->main_field_;
}

size_t Universe::height() const {
    return main_field_.height();
}

size_t Universe::width() const {
    return main_field_.width();
}

Universe::Universe(const Field &field)
: main_field_(field), twin_field_(main_field_.width(), main_field_.height()) {

}

Universe::Universe(Field &&field)
: main_field_(std::move(field)), twin_field_(main_field_.width(), main_field_.height()){

}

Field &Universe::field() {
    return main_field_;
}

