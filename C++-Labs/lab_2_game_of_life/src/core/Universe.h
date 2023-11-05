#ifndef LAB_2_GAME_OF_LIFE_UNIVERSE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSE_H

#include <set>

#include "Field.h"
#include "Rules.h"

class Universe {
public:
    Universe() = delete;

    template<class T1, class T2, class T3>
    explicit Universe(T1&& field, T2&& rules, T3&& name)
            : name_(std::forward<T3>(name)),
              rules_(std::forward<T2>(rules)),
              main_field_(std::forward<T1>(field)),
              twin_field_(main_field_.width(), main_field_.height()){}

    template<class T1, class T2>
    explicit Universe(T1&& field, T2&& rules)
            : name_("Unnamed"),
              rules_(std::forward<T2>(rules)),
              main_field_(std::forward<T1>(field)),
              twin_field_(main_field_.width(), main_field_.height()){}

    template<class T1>
    explicit Universe(T1&& field)
            : name_("Unnamed"),
              rules_(Rules::ConwayGameOfLife()),
              main_field_(std::forward<T1>(field)),
              twin_field_(main_field_.width(), main_field_.height()){}

    const Field& tick();
    const Field& tick_n(size_t n);

    [[nodiscard]] const std::string& name() const;
    [[nodiscard]] const Rules& rules() const;
    [[nodiscard]] const Field& field() const;
    Field& field();

    template<class T>
    void setRules(T&& rules) {
        rules_ = std::forward<T>(rules);
    }

    template<class T>
    void setName(T&& name) {
        name_ = std::forward<T>(name);
    }

private:
    std::string name_;
    Rules rules_;
    Field main_field_;
    Field twin_field_;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSE_H
