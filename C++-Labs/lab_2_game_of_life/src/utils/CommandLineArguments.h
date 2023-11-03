#ifndef LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H
#define LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H


#include <string>
#include <map>

class CommandLineArguments {
public:
    CommandLineArguments(int argc, char *argv[]);

    std::string getOption(const std::string &option_name);
    std::string getOption(const std::string &option_name, const std::string &alias);

private:
    std::map<std::string_view, std::string_view> options_;
};


#endif //LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H
