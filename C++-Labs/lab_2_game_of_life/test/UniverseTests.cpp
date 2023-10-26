#include <gtest/gtest.h>

#include "core/Universe.h"

TEST(UniverseTests, TickWorks) {
    Universe universe({
        {1, 0, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 0, 1}
    });

    std::set<uint8_t> born = {3};
    std::set<uint8_t> survive = {2, 3};
    universe.setRules(std::move(born), std::move(survive));

    const Field& result = universe.tick();

    const Field expected({
        {1, 1, 0, 0, 1},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1}
    });

    EXPECT_EQ(result, expected);
}