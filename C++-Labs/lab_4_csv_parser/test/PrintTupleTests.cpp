#include <gtest/gtest.h>
#include "PrintTuple.h"
#include <tuple>

TEST(PrintTupleTests, TuplePrints) {
    testing::internal::CaptureStdout();
    constexpr auto tuple1 = std::make_tuple(1, 2, 4.5f, "abcd");
    std::cout << tuple1;
    std::string str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(str, "[1, 2, 4.5, abcd]");

    testing::internal::CaptureStdout();
    constexpr auto tuple2 = std::make_tuple();
    std::cout << tuple2;
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(str, "[]");

    testing::internal::CaptureStdout();
    constexpr auto tuple3 = std::make_tuple(true, false, 123);
    std::cout << tuple3;
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(str, "[1, 0, 123]");
}