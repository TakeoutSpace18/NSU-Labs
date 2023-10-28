#ifndef LAB_2_GAME_OF_LIFE_UNIVERSE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSE_H

#include <set>

#include "Field.h"

class Universe {
public:
    Universe() = delete;
    explicit Universe(const Field &field);
    explicit Universe(Field &&field);

    const Field& tick();
    const Field& tick_n(size_t n);

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;
    Field &field();

    template<class T>
    void setRules(T&& neighbours_to_born, T&& neighbours_to_survive) {
        neighbours_to_born_ = std::forward<T>(neighbours_to_born);
        neighbours_to_survive_ = std::forward<T>(neighbours_to_survive);
    }

private:
    Field main_field_;
    Field twin_field_;
    std::string name_;
    std::set<uint8_t> neighbours_to_born_;
    std::set<uint8_t> neighbours_to_survive_;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSE_H
