#include "Converter.h"

#include <boost/program_options.hpp>
#include <fmt/format.h>

std::unique_ptr<po::variables_map> AbstractConverterCreator::parseArguments(const std::string& args) const
{
    try
    {
        auto vm = std::make_unique<po::variables_map>();
        po::store(po::command_line_parser(po::split_unix(args))
              .options(m_options_description)
              .positional(m_positional_description)
              .run(),
              *vm);
        po::notify(*vm);
        return vm;

    } catch (const po::error& e)
    {
        throw ConverterError(fmt::format("Failed to create {} converter: {}", name(), e.what()));
    }
}

ConverterError::ConverterError(const std::string& msg) : std::runtime_error(msg)
{
}

