#include "CommandLineArguments.h"

#include <vector>
#include <optional>

#include "Logger.h"

CommandLineArguments::CommandLineArguments(int argc, char **argv) {
    std::vector<std::string_view> arguments;
    std::copy(argv, argv + argc, std::back_inserter(arguments));

    int curr_arg = 0;
    while (curr_arg < argc) {
        std::string_view name;
        std::string_view value;

        if (arguments[curr_arg].starts_with("--")) {
            name = arguments[curr_arg].substr(2, arguments[curr_arg].size() - 2);
        }
        else if (arguments[curr_arg].starts_with('-')) {
            name = arguments[curr_arg].substr(1, arguments[curr_arg].size() - 1);
        }
        else {
            ++curr_arg;
            continue;
        }

        if (curr_arg < argc - 1 && !arguments[curr_arg + 1].starts_with('-')) {
            value = arguments[curr_arg + 1];
            curr_arg += 2;
        }
        else {
            value = "True";
            ++curr_arg;
        }

        if (!options_.insert(std::make_pair(name, value)).second) {
            logger << Logger::Warning << "option \"" << name << "\" appeared multiple times\n";
        }
    }


}

std::optional<std::string_view> CommandLineArguments::getOption(const std::string &option_name) {
    auto found = options_.find(option_name);
    if (found != options_.end()) {
        return found->second;
    }
    return std::nullopt;
}

std::optional<std::string_view>
CommandLineArguments::getOption(const std::string &option_name, const std::string &alias) {
    auto full_named = getOption(option_name);
    auto alias_named = getOption(alias);
    if (full_named) {
        return full_named;
    }
    if (alias_named) {
        return alias_named;
    }
    return std::nullopt;
}
