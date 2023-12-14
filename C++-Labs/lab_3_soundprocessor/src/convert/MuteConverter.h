#ifndef MUTECONVERTER_H
#define MUTECONVERTER_H
#include "Converter.h"


class MuteConverter : public Converter {
public:
    explicit MuteConverter(const ConfigurationFile::ConverterArgs& args);

    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;
};



#endif //MUTECONVERTER_H
