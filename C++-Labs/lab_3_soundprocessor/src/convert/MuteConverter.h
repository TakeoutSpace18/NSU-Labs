#ifndef MUTECONVERTER_H
#define MUTECONVERTER_H
#include "Converter.h"


class MuteConverter : public Converter {
public:
    explicit MuteConverter(const std::string& args);

    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;

    void printDescription(std::ostream& os) override;

private:
    std::uint32_t m_start_second;
    std::uint32_t m_stop_second;

    po::options_description m_options_description;
};



#endif //MUTECONVERTER_H
