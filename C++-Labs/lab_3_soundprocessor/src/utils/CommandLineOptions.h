#ifndef LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H
#define LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H

#include <filesystem>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

class CommandLineOptions
{
public:
    CommandLineOptions(int argc, char* argv[]);

	[[nodiscard]] bool printHelpIfRequired(std::ostream& out) const;
	[[nodiscard]] fs::path getOutputFileName() const;
	[[nodiscard]] fs::path getConfigurationFileName() const;
	[[nodiscard]] std::vector<fs::path> getInputFileNames() const;

private:
	po::options_description m_description;
	po::variables_map m_variables_map;
};


#endif //LAB_2_GAME_OF_LIFE_COMMANDLINEARGUMENTS_H
