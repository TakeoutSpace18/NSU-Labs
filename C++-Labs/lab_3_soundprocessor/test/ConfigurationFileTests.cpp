#include <gtest/gtest.h>

#include "app/ConfigurationFile.h"

TEST(ConfigurationFileTests, CanParseData)
{
    ConfigurationFile config("files/sample_config.txt");
    auto cmd = config.getNextCommand();
    EXPECT_TRUE(cmd.has_value());
    auto [name, args] = cmd.value();
    EXPECT_EQ(name, "mute");
    EXPECT_EQ(args, (ConfigurationFile::ConverterArgs{"0", "30"}));
}

TEST(ConfigurationFileTests, CanParseSingleLine)
{
    ConfigurationFile config("files/single_line.txt");
    auto cmd = config.getNextCommand();
    EXPECT_TRUE(cmd.has_value());
    auto [name, args] = cmd.value();
    EXPECT_EQ(name, "dummy");
    EXPECT_EQ(args, (ConfigurationFile::ConverterArgs{"1", "2", "3", "4"}));
}

TEST(ConfigurationFileTests, ThrowsOnNonExistentFile)
{
    EXPECT_THROW(
        ConfigurationFile("files/non_existent.txt"),
        ConfigurationFile::CantOpenFileError);
}

TEST(ConfigurationFileTests, IteratorAccessWorks)
{
    ConfigurationFile config("files/sample_config.txt");

    const std::vector<std::tuple<ConfigurationFile::ConverterName, ConfigurationFile::ConverterArgs>> expected = {
        {"mute", {"0", "30"}},
        {"mix", {"$2", "10"}},
        {"mute", {"120", "180"}},
        {"empty", {}}
    };

    int idx = 0;
    for (auto& command : config) {
        EXPECT_EQ(command, expected[idx]);
        ++idx;
    }

    EXPECT_EQ(idx, 4);
}