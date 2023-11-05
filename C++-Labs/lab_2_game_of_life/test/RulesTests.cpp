#include <gtest/gtest.h>

#include "core/Rules.h"

TEST(RulesTests, CreatesFromMCellNotation) {
    EXPECT_EQ(Rules::FromMCellNotation("B135/S2468"), Rules({1, 3, 5}, {2, 4, 6, 8}));
    EXPECT_EQ(Rules::FromMCellNotation("135/2468"), Rules({1, 3, 5}, {2, 4, 6, 8}));
    EXPECT_EQ(Rules::FromMCellNotation("ABC/DS"), Rules({}, {}));
    EXPECT_THROW(Rules::FromMCellNotation("B1352468"), std::runtime_error);
}

TEST(RulesTests, ConvertsToMCellNotation) {
    EXPECT_EQ(Rules({1, 3, 5}, {2, 4, 6, 8}).toMCellNotation(), "B135/S2468");
    EXPECT_EQ(Rules({}, {}).toMCellNotation(), "B/S");
}