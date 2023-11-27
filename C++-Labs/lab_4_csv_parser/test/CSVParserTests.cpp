#include <gtest/gtest.h>
#include "CSVParser.h"
#include "PrintTuple.h"
#include <tuple>
#include <string>

const static std::string sample_csv = "1,abc\n"
                                "2,def\n"
                                "3,ghi";

TEST(CSVParserTests, CanParseData) {
    std::stringstream ss(sample_csv);
    CSVParser<int, std::string> parser(ss);

    for (auto& tup : parser) {
        std::cout << tup << "\n";
    }

}