#include "Converter.h"

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <fmt/format.h>

#include "DummyConverter.h"
#include "MuteConverter.h"

std::unique_ptr<Converter> Converter::create(const std::string& name,
                                             const std::string& args)
{
    if (name == "dummy")
    {
        return std::make_unique<DummyConverter>(args);
    }
    if (name == "mute")
    {
        return std::make_unique<MuteConverter>(args);
    }
    throw ConverterError(fmt::format("Unknown converter: {}", name));
}

Converter::ConverterError::ConverterError(const std::string& msg) : std::runtime_error(msg)
{
}

Converter::WrongArgumentsCount::WrongArgumentsCount(std::size_t expected, std::size_t actual,
                                                    std::string converter_name)
    : ConverterError(fmt::format(
        "Error while creating {} converter: expected {} arguments, found {}", converter_name, expected,
        actual))
{
}

std::unique_ptr<po::variables_map> Converter::parseArguments(const po::options_description& desc,
                                                             const po::positional_options_description& pos_desc,
                                                             const std::string& args)
{
    auto vm = std::make_unique<po::variables_map>();
    po::store(po::command_line_parser(po::split_unix(args))
              .options(desc)
              .positional(pos_desc)
              .run(),
              *vm);
    po::notify(*vm);

    return vm;
}
