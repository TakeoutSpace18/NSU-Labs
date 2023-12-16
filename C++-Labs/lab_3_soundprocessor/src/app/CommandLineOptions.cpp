#include "CommandLineOptions.h"

#include <filesystem>

#include "ConfigurationFile.h"
#include "convert/ConverterFactory.h"


CommandLineOptions::CommandLineOptions(int argc, char* argv[]) :
m_description("General options")
{
    m_description.add_options()
            ("help,h", "print help message")
            ("config,c", po::value<fs::path>()->required(),"specify configuration file")
            ("output,o", po::value<fs::path>()->default_value("out.wav"), "output file name")
            ("input,i", po::value<std::vector<fs::path>>()->required(), "output file name");

    po::positional_options_description pos_desc;
    pos_desc.add("output", 1);
    pos_desc.add("input", -1);

    po::store(po::command_line_parser(argc, argv)
        .options(m_description)
        .positional(pos_desc)
        .run(),
        m_variables_map);

    try
    {
        po::notify(m_variables_map);
    }
    catch (const po::required_option& e)
    {
        // ignore required options if --help needed
        if (!m_variables_map.contains("help"))
        {
            throw;
        }
    }
}

bool CommandLineOptions::printHelpIfRequired(std::ostream& os) const
{
    if (m_variables_map.contains("help")) {
        os << m_description << '\n';
        ConfigurationFile::printHelpMessage(os);
        os << '\n';
        ConverterFactory::printDescriptions(os);
        return true;
    }

    return false;
}

fs::path CommandLineOptions::getOutputFileName() const
{
    return m_variables_map["output"].as<fs::path>();
}

fs::path CommandLineOptions::getConfigurationFileName() const
{
    return m_variables_map["config"].as<fs::path>();
}

std::vector<fs::path> CommandLineOptions::getInputFileNames() const
{
    return m_variables_map["input"].as<std::vector<fs::path>>();
}
