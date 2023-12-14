#ifndef DUMMYCONVERTER_H
#define DUMMYCONVERTER_H
#include "Converter.h"


class DummyConverter : public Converter
{
public:
    explicit DummyConverter(const ConfigurationFile::ConverterArgs& args);
    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;
};



#endif //DUMMYCONVERTER_H
