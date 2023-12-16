#ifndef CONVERTER_H
#define CONVERTER_H
#include <boost/program_options.hpp>

#include "audio/AudioFile.h"

namespace po = boost::program_options;

class Converter
{
public:
    virtual ~Converter() = default;

    virtual void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) = 0;
};

class AbstractConverterCreator
{
public:
    virtual ~AbstractConverterCreator() = default;

    virtual std::unique_ptr<Converter> create(const std::string& args) = 0;
    virtual void printDescription(std::ostream& os) = 0;
    virtual std::string name() const = 0;


protected:
    [[nodiscard]] std::unique_ptr<po::variables_map> parseArguments(const std::string& args) const;

    // supposed to be initialized by derived converters
    po::options_description m_options_description;
    po::positional_options_description m_positional_description;
};

struct ConverterError : std::runtime_error
{
    explicit ConverterError(const std::string& msg);
};


#endif //CONVERTER_H
