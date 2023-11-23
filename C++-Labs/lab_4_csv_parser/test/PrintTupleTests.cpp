#include <gtest/gtest.h>
#include "PrintTuple.h"
#include <tuple>

TEST(PrintTupleTests, TuplePrints) {
    std::stringstream ss;

    std::cout << ss.str();
}