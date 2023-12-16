#ifndef MUTECONVERTER_H
#define MUTECONVERTER_H
#include "convert/Converter.h"

class MuteConverterCreator final : public AbstractConverterCreator
{
public:
    MuteConverterCreator();

    std::unique_ptr<Converter> create(const std::string& args) override;

    void printDescription(std::ostream& os) override;

    std::string name() const override;
};


class MuteConverter final : public Converter {
public:
    explicit MuteConverter(int64_t start_second, int64_t end_second);

    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;

private:
    std::uint32_t m_start_second;
    std::uint32_t m_stop_second;

};



#endif //MUTECONVERTER_H
