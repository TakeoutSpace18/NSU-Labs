#include <gtest/gtest.h>
#include <cstring>
#include <filesystem>

#include "utils/CommandLineArguments.h"

static std::pair<int, char**> createArgcArgv(auto ...args) {
    std::vector<std::string> strings = {args...};
    int argc = sizeof...(args);
    char **argv = new char*[argc];

    for (int i = 0; i < argc; ++i) {
        argv[i] = new char[strings[i].size() + 1];
        std::strcpy(argv[i], strings[i].c_str());
    }

    return {argc, argv};
}

static void freeArgcArgv(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        delete[] argv[i];
    }
    delete[] argv;
}

TEST(CommandLineArgumentsTests, CanObtainOptionValues) {
    auto [argc, argv] = createArgcArgv("--option1", "value1", "--option2", "value2");
    CommandLineArguments cmdArgs(argc, argv);

    EXPECT_EQ(cmdArgs.getOption("option1").value(), "value1");
    EXPECT_EQ(cmdArgs.getOption("option2").value(), "value2");
    EXPECT_EQ(cmdArgs.getOption("option3"), std::nullopt);

    freeArgcArgv(argc, argv);
}

TEST(CommandLineArgumentsTests, CanObtainCastedOptionValues) {
    auto [argc, argv] = createArgcArgv("--option1", "123", "--option2", "1.25");
    CommandLineArguments cmdArgs(argc, argv);

    EXPECT_EQ(cmdArgs.getOption<int>("option1").value(), 123);
    EXPECT_EQ(cmdArgs.getOption<float>("option2").value(), 1.25f);
    EXPECT_EQ(cmdArgs.getOption<int>("option3"), std::nullopt);

    freeArgcArgv(argc, argv);
}

TEST(CommandLineArgumentsTests, CanObtainOptionViaAlias) {
    auto [argc, argv] = createArgcArgv("-o", "123");
    CommandLineArguments cmdArgs(argc, argv);

    EXPECT_EQ(cmdArgs.getOption<int>("option1", "o").value(), 123);

    freeArgcArgv(argc, argv);
}

TEST(CommandLineArgumentsTests, CanObtainBoolOptions) {
    auto [argc, argv] = createArgcArgv("--option1");
    CommandLineArguments cmdArgs(argc, argv);

    EXPECT_TRUE(cmdArgs.getOption("option1"));
    EXPECT_FALSE(cmdArgs.getOption("option2"));

    freeArgcArgv(argc, argv);
}