#ifndef CONVERTER_H
#define CONVERTER_H
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "app/ConfigurationFile.h"
#include "audio/AudioFile.h"

namespace po = boost::program_options;

class Converter
{
public:
    virtual ~Converter() = default;

    virtual void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) = 0;

    virtual void printDescription(std::ostream& os) = 0;

    static std::unique_ptr<Converter> create(const std::string& name, const std::string& args);

    struct ConverterError : std::runtime_error
    {
        explicit ConverterError(const std::string& msg);
    };

    struct WrongArgumentsCount final : ConverterError
    {
        explicit WrongArgumentsCount(std::size_t expected, std::size_t actual, std::string converter_name);
    };

protected:
    std::unique_ptr<po::variables_map> parseArguments(const po::options_description& desc,
                                                      const po::positional_options_description& pos_desc,
                                                      const std::string& args);
};


#endif //CONVERTER_H
