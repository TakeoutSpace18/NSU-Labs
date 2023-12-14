#include "Converter.h"

#include <fmt/format.h>

#include "DummyConverter.h"

std::unique_ptr<Converter> Converter::create(const ConfigurationFile::ConverterName& name,
                                                      const ConfigurationFile::ConverterArgs& args)
{
    if (name == "dummy")
    {
        return std::make_unique<DummyConverter>(args);
    }
    throw std::runtime_error(fmt::format("Unknown converter: {}", name));
}
