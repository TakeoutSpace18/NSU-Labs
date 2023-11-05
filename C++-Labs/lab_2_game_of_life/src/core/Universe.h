#ifndef LAB_2_GAME_OF_LIFE_UNIVERSE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSE_H

#include <set>

#include "Field.h"
#include "Rules.h"

class Universe {
public:
    Universe() = delete;
    explicit Universe(const Field &field);
    explicit Universe(Field &&field);

    const Field& tick();
    const Field& tick_n(size_t n);

    [[nodiscard]] const std::string& name() const;
    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;

    [[nodiscard]] const Rules& rules() const;
    Field& field();

    template<class T>
    void setRules(T&& rules) {
        rules_ = std::forward<T>(rules);
    }

private:
    std::string name_;
    Field main_field_;
    Field twin_field_;
    Rules rules_;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSE_H
