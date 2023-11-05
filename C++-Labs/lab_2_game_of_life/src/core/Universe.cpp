#include "Universe.h"

#include <ranges>

const Field& Universe::tick() {
    for (size_t x = 0; x < main_field_.width(); ++x) {
        for (size_t y = 0; y < main_field_.height(); ++y) {
            uint8_t neighbours_count = main_field_.getNeighboursCount(x, y);
            if (rules_.canBorn(neighbours_count)) {
                twin_field_[x][y] = true;
            }
            else if (rules_.canSurvive(neighbours_count)) {
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

Field& Universe::field() {
    return main_field_;
}

const std::string &Universe::name() const {
    return name_;
}

const Field &Universe::field() const {
    return main_field_;
}

const Rules &Universe::rules() const {
    return rules_;
}
