#ifndef LAB_2_GAME_OF_LIFE_UNIVERSE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSE_H

#include <set>
#include <span>

#include "Field.h"

template <typename T>
concept IsUint8Set = std::same_as<typename std::remove_reference<T>::type, std::set<uint8_t>>;

template <typename T>
concept IsContainerOfUnsignedIntegral = !IsUint8Set<T> && requires(const T& container) {
    std::begin(container);
}
&& std::unsigned_integral<typename std::remove_reference<T>::type::value_type>;

class Universe {
public:
    Universe() = delete;
    explicit Universe(const Field &field);
    explicit Universe(Field &&field);

    void tick();
    void tick_n(size_t n);

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;


    template<IsContainerOfUnsignedIntegral Container>
    void setRules(Container&& neighbours_to_born, Container&& neighbours_to_survive) {
        neighbours_to_born_ = std::set<uint8_t>(neighbours_to_born.begin(), neighbours_to_born.end());
        neighbours_to_survive_ = std::set<uint8_t>(neighbours_to_survive.begin(), neighbours_to_survive.end());
    }

    template<IsUint8Set T>
    void setRules(T&& neighbours_to_born, T&& neighbours_to_survive) {
        neighbours_to_born_ = std::forward<T>(neighbours_to_born);
        neighbours_to_survive_ = std::forward<T>(neighbours_to_survive);
    }

private:
    Field field_;
    std::string name_;
    std::set<uint8_t > neighbours_to_born_;
    std::set<uint8_t> neighbours_to_survive_;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSE_H
