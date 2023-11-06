#include <stdexcept>
#include <algorithm>
#include "Rules.h"

Rules::Rules(const std::set<uint8_t>& neighbours_to_born, const std::set<uint8_t>& neighbours_to_survive)
 : neighbours_to_born_(neighbours_to_born),
   neighbours_to_survive_(neighbours_to_survive) {}

Rules::Rules(std::set<uint8_t>&& neighbours_to_born, std::set<uint8_t>&& neighbours_to_survive)
 : neighbours_to_born_(std::move(neighbours_to_born)),
   neighbours_to_survive_(std::move(neighbours_to_survive)) {}

bool Rules::canBorn(uint8_t neighbours_count) const {
    return neighbours_to_born_.contains(neighbours_count);
}

bool Rules::canSurvive(uint8_t neighbours_count) const {
    return neighbours_to_survive_.contains(neighbours_count);
}

Rules Rules::FromMCellNotation(const std::string &notation) {
    size_t separator_pos = notation.find('/');
    if (separator_pos == std::string::npos) {
        throw std::runtime_error("Error while parsing MCell notation: separator not found");
    }

    auto charToDigit = [](char c) {return c - '0';};

    std::set<uint8_t> neighbours_to_born;
    std::set<uint8_t> neighbours_to_survive;

    std::for_each_n(notation.cbegin(),separator_pos, [&](char c) {
        if (std::isdigit(c)) {
            neighbours_to_born.insert(charToDigit(c));
        }
    });

    std::for_each(notation.cbegin() + separator_pos, notation.cend(), [&](char c) {
        if (std::isdigit(c)) {
            neighbours_to_survive.insert(charToDigit(c));
        }
    });

    return {std::move(neighbours_to_born), std::move(neighbours_to_survive)};
}

bool operator==(const Rules& lhs, const Rules& rhs) {
    return lhs.neighbours_to_born_ == rhs.neighbours_to_born_
     && lhs.neighbours_to_survive_ == rhs.neighbours_to_survive_;
}

Rules Rules::ConwayGameOfLife() {
    return Rules({3}, {2, 3});
}

std::string Rules::toMCellNotation() const {
    std::string notation = "B";
    for (uint8_t i : neighbours_to_born_) {
        notation.append(std::to_string(i));
    }
    notation.append("/S");
    for (uint8_t i : neighbours_to_survive_) {
        notation.append(std::to_string(i));
    }
    return notation;
}
