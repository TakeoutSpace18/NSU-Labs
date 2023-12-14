#ifndef CONVERTER_H
#define CONVERTER_H
#include "app/ConfigurationFile.h"
#include "audio/AudioFile.h"


class Converter
{
public:
    virtual ~Converter() = default;

    virtual void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) = 0;

    static std::unique_ptr<Converter> create(const ConfigurationFile::ConverterName& name,
                                                      const ConfigurationFile::ConverterArgs& args);
};


#endif //CONVERTER_H
