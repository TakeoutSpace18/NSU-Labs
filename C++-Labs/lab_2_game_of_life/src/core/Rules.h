#ifndef LAB_2_GAME_OF_LIFE_RULES_H
#define LAB_2_GAME_OF_LIFE_RULES_H

#include <initializer_list>
#include <cstdint>
#include <set>
#include <string>

class Rules {
public:
    Rules() = delete;

    Rules(std::set<uint8_t>&& neighbours_to_born, std::set<uint8_t>&& neighbours_to_survive);
    Rules(const std::set<uint8_t>& neighbours_to_born, const std::set<uint8_t>& neighbours_to_survive);

    /**
     * Create Rules object by parsing MCell notation.
     * @example Rules::FromMCellNotation("B123/S456")
     * @example Rules::FromMCellNotation("123/456")
     * @throws std::runtime_error if no '/' separator found in string.
     */
    static Rules FromMCellNotation(const std::string& notation);
    static Rules ConwayGameOfLife();

    [[nodiscard]] std::string toMCellNotation() const;

    [[nodiscard]] bool canBorn(uint8_t neighbours_count) const;
    [[nodiscard]] bool canSurvive(uint8_t neighbours_count) const;

    friend bool operator==(const Rules& lhs, const Rules& rhs);

private:
    std::set<uint8_t> neighbours_to_born_;
    std::set<uint8_t> neighbours_to_survive_;
};


#endif //LAB_2_GAME_OF_LIFE_RULES_H
