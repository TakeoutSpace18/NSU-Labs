#include <gtest/gtest.h>
#include <tuple>
#include <string>

#include "CSVParser.h"
#include "PrintTuple.h"

TEST(CSVParserTests, ParsesBasicData) {
    std::stringstream sample_csv(
        "1,a bc\n"
        "2,def\n");
    CSVParser<int, std::string> parser(sample_csv);

    std::tuple<int, std::string> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), 1);
    EXPECT_EQ(std::get<1>(line), "a bc");

    parser >> line;
    EXPECT_EQ(std::get<0>(line), 2);
    EXPECT_EQ(std::get<1>(line), "def");
}

TEST(CSVParserTests, ThrowsOnEmptyArithmeticFields) {
    std::stringstream sample_csv(",\n1,1.2");
    CSVParser<int, double> parser(sample_csv);
    std::tuple<int, double> line;
    EXPECT_THROW(parser >> line, CSVParserErrors::EmptyField);
    parser >> line;
    EXPECT_EQ(std::get<0>(line), 1);
    EXPECT_EQ(std::get<1>(line), 1.2);
}

TEST(CSVParserTests, ParsesEmptyStringFields) {
    std::stringstream sample_csv(",\nabc,def");
    CSVParser<std::string, std::string> parser(sample_csv);

    std::tuple<std::string, std::string> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), "");
    EXPECT_EQ(std::get<1>(line), "");

    parser >> line;
    EXPECT_EQ(std::get<0>(line), "abc");
    EXPECT_EQ(std::get<1>(line), "def");
}

TEST(CSVParserTests, ThrowsOnTypeMismatch) {
    std::stringstream sample_csv("abc,1\n123.12,1\n1,2");
    CSVParser<int, int> parser(sample_csv);
    std::tuple<int, int> line;
    EXPECT_THROW(parser >> line, CSVParserErrors::TypeMismatch);
    EXPECT_THROW(parser >> line, CSVParserErrors::TypeMismatch);
    parser >> line;
    EXPECT_EQ(std::get<0>(line), 1);
    EXPECT_EQ(std::get<1>(line), 2);
}

TEST(CSVParserTests, ThrowsOnTooLongRow) {
    std::stringstream sample_csv("1,2,3\n1,2");
    CSVParser<int, int> parser(sample_csv);
    std::tuple<int, int> line;
    EXPECT_THROW(parser >> line, CSVParserErrors::WrongColumnCount);
    parser >> line;
    EXPECT_EQ(std::get<0>(line), 1);
    EXPECT_EQ(std::get<1>(line), 2);
}

TEST(CSVParserTests, ThrowsOnTooShortRow) {
    std::stringstream sample_csv("1\n1,2");
    CSVParser<int, int> parser(sample_csv);
    std::tuple<int, int> line;
    EXPECT_THROW(parser >> line, CSVParserErrors::WrongColumnCount);
    parser >> line;
    EXPECT_EQ(std::get<0>(line), 1);
    EXPECT_EQ(std::get<1>(line), 2);
}

TEST(CSVParserTests, ParsesWithEscapeSymbol) {
    std::stringstream sample_csv("a\\,b\\,c,def\nabc,d\\,e\\,f");
    CSVParser<std::string, std::string> parser(sample_csv);

    std::tuple<std::string, std::string> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), "a,b,c");
    EXPECT_EQ(std::get<1>(line), "def");

    parser >> line;
    EXPECT_EQ(std::get<0>(line), "abc");
    EXPECT_EQ(std::get<1>(line), "d,e,f");
}

TEST(CSVParserTests, ParsesCustomConfig) {
    std::stringstream sample_csv("abc|def abc|d%|e%|f");
    CSVParser<std::string, std::string> parser(sample_csv, 0, {'|', ' ', '%'});

    std::tuple<std::string, std::string> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), "abc");
    EXPECT_EQ(std::get<1>(line), "def");

    parser >> line;
    EXPECT_EQ(std::get<0>(line), "abc");
    EXPECT_EQ(std::get<1>(line), "d|e|f");
}

TEST(CSVParserTests, SkipLinesWorks) {
    std::stringstream sample_csv("#CSV table\nname: abcd\nabc,def");
    CSVParser<std::string, std::string> parser(sample_csv, 2);

    std::tuple<std::string, std::string> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), "abc");
    EXPECT_EQ(std::get<1>(line), "def");
}

TEST(CSVParserTests, ParsesInRangeBasedFor) {
    int num_iterations = 10;
    std::stringstream sample_csv;
    for (int i = 0; i < num_iterations; ++i) {
        sample_csv << i << "\n";
    }

    CSVParser<int> parser(sample_csv);

    int n = 0;
    for (auto [i] : parser) {
        EXPECT_EQ(i, n);
        ++n;
    }
}

TEST(CSVParserTests, WorksWithWideChars) {
    std::wstringstream sample_csv(L"abc|def abc|d%|e%|f");
    wCSVParser<std::wstring, std::wstring> parser(sample_csv, 0, {'|', ' ', '%'});

    std::tuple<std::wstring, std::wstring> line;
    parser >> line;
    EXPECT_EQ(std::get<0>(line), L"abc");
    EXPECT_EQ(std::get<1>(line), L"def");

    parser >> line;
    EXPECT_EQ(std::get<0>(line), L"abc");
    EXPECT_EQ(std::get<1>(line), L"d|e|f");
}