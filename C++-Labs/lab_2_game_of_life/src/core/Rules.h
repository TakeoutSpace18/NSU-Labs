#ifndef LAB_2_GAME_OF_LIFE_RULES_H
#define LAB_2_GAME_OF_LIFE_RULES_H

#include <initializer_list>
#include <cstdint>
#include <set>
#include <string>
#include <bitset>

class Rules {
public:
    Rules() = delete;

    Rules(const std::bitset<9>& neighbours_to_born,
          const std::bitset<9>& neighbours_to_survive);

    Rules(const std::initializer_list<uint8_t>& neighbours_to_born,
          const std::initializer_list<uint8_t>& neighbours_to_survive);

    [[nodiscard]] const std::bitset<9>& bornFlags() const;

    [[nodiscard]] const std::bitset<9>& surviveFlags() const;

    [[nodiscard]] bool canBorn(uint8_t neighbours_count) const;
    [[nodiscard]] bool canSurvive(uint8_t neighbours_count) const;

    [[nodiscard]] std::string toMCellNotation() const;

    /**
     * Create Rules object by parsing MCell notation.
     * @example Rules::FromMCellNotation("B123/S456")
     * @example Rules::FromMCellNotation("123/456")
     * @throws std::runtime_error if no '/' separator found in string.
     */
    static Rules FromMCellNotation(const std::string& notation);
    static Rules ConwayGameOfLife();

    friend bool operator==(const Rules& lhs, const Rules& rhs);

private:
    std::bitset<9> neighbours_to_born_;
    std::bitset<9> neighbours_to_survive_;
};


#endif //LAB_2_GAME_OF_LIFE_RULES_H
