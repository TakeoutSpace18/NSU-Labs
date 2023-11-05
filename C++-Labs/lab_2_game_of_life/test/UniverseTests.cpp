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

    universe.setRules(Rules::ConwayGameOfLife());

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