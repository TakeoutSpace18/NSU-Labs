#include <stdexcept>
#include <algorithm>
#include <vector>
#include "Rules.h"

Rules::Rules(const std::initializer_list<uint8_t>& neighbours_to_born,
             const std::initializer_list<uint8_t>& neighbours_to_survive) {
    for (const auto& i : neighbours_to_born) {
        neighbours_to_born_[i] = true;
    }
    for (const auto& i : neighbours_to_survive) {
        neighbours_to_survive_[i] = true;
    }
}

bool Rules::canBorn(uint8_t neighbours_count) const {
    return neighbours_to_born_[neighbours_count];
}

bool Rules::canSurvive(uint8_t neighbours_count) const {
    return neighbours_to_survive_[neighbours_count];
}

Rules Rules::FromMCellNotation(const std::string &notation) {
    size_t separator_pos = notation.find('/');
    if (separator_pos == std::string::npos) {
        throw std::runtime_error("Error while parsing MCell notation: separator not found");
    }

    auto charToDigit = [](char c) {return c - '0';};

    std::bitset<9> neighbours_to_born;
    std::bitset<9> neighbours_to_survive;

    std::for_each_n(notation.cbegin(),separator_pos, [&](char c) {
        if (std::isdigit(c) && c <= '8') {
            neighbours_to_born[charToDigit(c)] = true;
        }
    });

    std::for_each(notation.cbegin() + separator_pos, notation.cend(), [&](char c) {
        if (std::isdigit(c) && c <= '8') {
            neighbours_to_survive[charToDigit(c)] = true;
        }
    });

    return {neighbours_to_born, neighbours_to_survive};
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
    for (int i = 0; i <= 8; ++i) {
        if (neighbours_to_born_[i]) {
            notation.append(std::to_string(i));
        }
    }
    notation.append("/S");
    for (int i = 0; i <= 8; ++i) {
        if (neighbours_to_survive_[i]) {
            notation.append(std::to_string(i));
        }
    }
    return notation;
}

Rules::Rules(const std::bitset<9> &neighbours_to_born,
             const std::bitset<9> &neighbours_to_survive)
: neighbours_to_born_(neighbours_to_born),
  neighbours_to_survive_(neighbours_to_survive) {}

const std::bitset<9> &Rules::bornFlags() const {
    return neighbours_to_born_;
}

const std::bitset<9> &Rules::surviveFlags() const {
    return neighbours_to_survive_;
}
const std::vector<std::pair<const char*, const Rules>> Rules::InterestingRules = {
        std::make_pair("Life", Rules{{3}, {2, 3}}),
        std::make_pair("HighLife", Rules{{3, 6}, {2, 3}}),
        std::make_pair("Day & Night", Rules{{3, 6, 7, 8}, {3, 4, 6, 7, 8}}),
        std::make_pair("Seeds", Rules{{2}, {}}),
        std::make_pair("Diamoeba", Rules{{3, 5, 6, 7, 8}, {5, 6, 7, 8}}),
        std::make_pair("Anneal", Rules{{4, 6, 7, 8}, {3, 5, 6, 7, 8}}),
        std::make_pair("Morley", Rules{{3, 6, 8}, {2, 4, 5}}),
        std::make_pair("Life without death", Rules{{3}, {0, 1, 2, 3, 4, 5, 6, 7, 8}}),
        std::make_pair("2x2", Rules{{3, 6}, {1, 2, 5}}),
        std::make_pair("Replicator", Rules{{1, 3, 5, 7}, {1, 3, 5, 7}}),
};