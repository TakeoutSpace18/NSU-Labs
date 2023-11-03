#include "CommandLineArguments.h"

#include <vector>

#include "Logger.h"

CommandLineArguments::CommandLineArguments(int argc, char **argv) {
    std::vector<std::string_view> arguments;
    std::copy(argv, argv + argc, std::back_inserter(arguments));

    int curr_arg= 0;
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

        if (!arguments[curr_arg + 1].starts_with('-')) {
            value = arguments[curr_arg + 1];
            curr_arg += 2;
        }

        if (!options_.insert(std::make_pair(name, value)).second) {
            logger << Logger::Info << "option \"" << name << "\" appeared multiple times\n";
        }

        ++curr_arg;
    }


}