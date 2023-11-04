#ifndef LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H
#define LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H


#include <string>
#include <map>
#include <optional>
#include <sstream>
#include "Logger.h"

class CommandLineArguments {
public:
    CommandLineArguments(int argc, char *argv[]);

    std::optional<std::string_view> getOption(const std::string &option_name);
    std::optional<std::string_view> getOption(const std::string &option_name, const std::string &alias);

    template<class T>
    std::optional<T> getOption(const std::string &option_name) {
        auto option = getOption(option_name);
        if (option) {
            T converted;
            static std::istringstream stream;
            stream.str(std::string(option.value()));
            stream >> converted;
            if (stream.fail()) {
                logger << Logger::Error << "failed to convert option \"" << option_name
                << "\" to " << typeid(T).name() << " type.\n";
                stream.clear();
            }
            return converted;
        }
        return std::nullopt;
    }

    template<class T>
    std::optional<T> getOption(const std::string &option_name, const std::string &alias) {
        auto full_named = getOption<T>(option_name);
        auto alias_named = getOption<T>(alias);
        if (full_named) {
            return full_named;
        }
        if (alias_named) {
            return alias_named;
        }
        return std::nullopt;
    }

private:
    std::map<std::string_view, std::string_view> options_;
};


#endif //LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H
