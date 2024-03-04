#ifndef MIXCONVERTER_H
#define MIXCONVERTER_H

#include "convert/Converter.h"

class MixConverterCreator final : public AbstractConverterCreator
{
public:
    MixConverterCreator();

    std::unique_ptr<Converter> create(const std::string& args,
                                      InputFilesLoader& input_files) override;

    void printDescription(std::ostream& os) override;

    [[nodiscard]] std::string name() const override;
};


class MixConverter final : public Converter
{
public:
    explicit MixConverter(std::shared_ptr<AudioFile> input_file, int64_t start_second, int64_t end_second);

    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;

private:
    std::shared_ptr<AudioFile> m_mix_with;
    std::uint32_t m_start_second;
    std::uint32_t m_stop_second;
};


#endif //MIXCONVERTER_H
