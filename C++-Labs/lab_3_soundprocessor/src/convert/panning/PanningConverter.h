#ifndef PANNINGCONVERTER_H
#define PANNINGCONVERTER_H

#include "convert/Converter.h"

class PanningConverterCreator final : public AbstractConverterCreator
{
public:
    PanningConverterCreator();

    std::unique_ptr<Converter> create(const std::string& args,
                                      InputFilesLoader& input_files) override;

    void printDescription(std::ostream& os) override;

    [[nodiscard]] std::string name() const override;
};


class PanningConverter final : public Converter
{
public:
    explicit PanningConverter(double ear_dist, double speed);

    void apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output) override;

private:
    double m_ear_dist_factor;
    double m_speed;
};


#endif //PANNINGCONVERTER_H
