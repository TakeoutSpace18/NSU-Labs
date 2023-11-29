#include <gtest/gtest.h>
#include "CSVParser.h"
#include "PrintTuple.h"
#include <tuple>
#include <string>


TEST(CSVParserTests, CanParseData) {
    std::stringstream sample_csv(
        "1,abc\n"
              "2,def\n");
    CSVParser<int, std::string> parser(sample_csv);

    std::tuple<int, std::string> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), 1);
    EXPECT_EQ(std::get<1>(line), "abc");

    parser >> line;
    EXPECT_EQ(std::get<0>(line), 2);
    EXPECT_EQ(std::get<1>(line), "def");

}