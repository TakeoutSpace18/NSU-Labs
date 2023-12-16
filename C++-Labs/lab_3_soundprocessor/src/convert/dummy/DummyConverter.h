#ifndef DUMMYCONVERTER_H
#define DUMMYCONVERTER_H

#include "convert/Converter.h"
#include "convert/ConverterFactory.h"

class DummyConverterCreator : public AbstractConverterCreator
{
public:
    DummyConverterCreator() = default;

    std::unique_ptr<Converter> create(const std::string& args) override;

    void printDescription(std::ostream& os) override;

    std::string name() const override;
};


class DummyConverter : public Converter
{
public:
    explicit DummyConverter() = default;

    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;
};



#endif //DUMMYCONVERTER_H
