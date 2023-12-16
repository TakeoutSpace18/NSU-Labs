#include "ConverterFactory.h"
#include "mute/MuteConverter.h"
#include "dummy/DummyConverter.h"

#include <boost/program_options.hpp>
#include <fmt/format.h>

std::map<const std::string, std::shared_ptr<AbstractConverterCreator>> ConverterFactory::m_creators = {
    {"dummy", std::make_shared<DummyConverterCreator>()},
    {"mute", std::make_shared<MuteConverterCreator>()}
};

std::unique_ptr<Converter> ConverterFactory::create(const std::string& name, const std::string& args)
{
    if (m_creators.contains(name))
    {
        return m_creators[name]->create(args);
    }

    throw ConverterError(fmt::format("Unknown converter name: {}", name));
}

void ConverterFactory::printDescriptions(std::ostream& os)
{
    os << "Available converters:\n";
    for (auto& [name, creator]: m_creators)
    {
        os << fmt::format("-------------------------- {} --------------------------\n", name);
        creator->printDescription(os);
        os << '\n';
    }
}
