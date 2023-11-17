#include <gtest/gtest.h>

#include "core/Field.h"

TEST(FieldTests, GetNeighboursCountWorks) {
    Field field(3, 3);

    uint32_t count = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {

            if (i == 1 && j == 1) {
                continue;
            }

            field[i][j] = true;
            ++count;

            ASSERT_EQ(field.getNeighboursCount(1, 1), count);
        }
    }

}

TEST(FieldTests, CantCreateWithSmallSize) {
    EXPECT_THROW(Field(5, 2), std::invalid_argument);
    EXPECT_THROW(Field(2, 5), std::invalid_argument);
}