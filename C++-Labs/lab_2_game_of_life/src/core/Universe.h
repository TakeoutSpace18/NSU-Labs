#ifndef LAB_2_GAME_OF_LIFE_UNIVERSE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSE_H

#include <set>

#include "Field.h"

template <class...>
class Universe {
public:
    Universe() = delete;
    explicit Universe(const Field &field);
    explicit Universe(Field &&field);

    void tick();
    void tick_n(size_t n);

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;


    template<class U, class V>
    void setRules(U&& neighbours_to_born, V&& neighbours_to_survive)
    requires std::assignable_from<decltype((this->neighbours_to_born_)), U> &&
             std::assignable_from<decltype((this->neighbours_to_survive_)), V> {
        neighbours_to_born_ = std::forward<U>(neighbours_to_born);
        neighbours_to_survive_ = std::forward<V>(neighbours_to_survive);
    }

private:
    Field field_;
    std::string name_;
    std::set<uint8_t> neighbours_to_born_;
    std::set<uint8_t> neighbours_to_survive_;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSE_H
