#ifndef LAB_2_GAME_OF_LIFE_UNIVERSE_H
#define LAB_2_GAME_OF_LIFE_UNIVERSE_H

#include <set>

#include "Field.h"

class Universe {
public:
    void tick();
    void tick_n(size_t n);

private:
    Field field_;
    std::string name_;
    std::set<int> neighbours_to_born_;
    std::set<int> neighbours_to_survive_;
};

#endif //LAB_2_GAME_OF_LIFE_UNIVERSE_H
