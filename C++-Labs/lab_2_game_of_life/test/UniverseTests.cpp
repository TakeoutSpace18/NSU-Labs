#include <gtest/gtest.h>

#include "core/Universe.h"

TEST(UniverseTests, TickWorks) {
    Field field(5, 5);
    Universe uni(std::move(field));

    std::set<uint32_t> vec = {1, 2, 3};
    std::set<float> vec2 = {1, 2, 3};
    std::set<uint8_t> vec3 = {1, 2, 3};

    uni.setRules(vec3, vec3);
    uni.setRules(vec, vec);

//    uni.setRules(std::move(field), std::move(field));
//    uni.setRules(vec2, vec2);
}