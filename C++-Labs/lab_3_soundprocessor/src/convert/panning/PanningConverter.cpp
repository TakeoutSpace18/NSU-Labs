#include "PanningConverter.h"

#include <fmt/format.h>
#include <iostream>
#include <boost/program_options.hpp>

#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"

PanningConverterCreator::PanningConverterCreator()
{
    namespace po = boost::program_options;
    m_options_description.add_options()
            ("ear-dist,d", po::value<double>()->required(),
             "distance beetwen ears and circle diameter ratio, value from 0 to 100 percent")
            ("speed,s", po::value<double>()->required(), "speed of sound panning (rotations per second)");

    m_positional_description.add("ear-dist", 1);
    m_positional_description.add("speed", 1);
}

std::unique_ptr<Converter> PanningConverterCreator::create(const std::string& args, InputFilesLoader& input_files)
{
    auto vm = parseArguments(args);

    auto ear_dist = (*vm)["ear-dist"].as<double>();
    auto speed = (*vm)["speed"].as<double>();
    return std::make_unique<PanningConverter>(ear_dist, speed);
}

void PanningConverterCreator::printDescription(std::ostream& os)
{
    os << "Panning converter\n";
    os << "\tpans stereo sound arround a circle\n";
    os << m_options_description;
}

std::string PanningConverterCreator::name() const
{
    return "panning";
}

PanningConverter::PanningConverter(const double ear_dist, const double speed) :
    m_ear_dist_factor(ear_dist / 100), // convert percentage to fraction
    m_speed(speed)
{
    if (ear_dist < 0 || ear_dist > 100)
    {
        throw ConverterError("Panning converter: ear distance value should be in [0, 100] bounds.")
    }
}

void PanningConverter::apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output)
{
    double cur_angle = 0.0;
    double angle_addition = 2 * std::numbers::pi * m_speed / input->properties().sample_rate;
    int64_t processed_samples_count = 0;

    if (input->properties().num_channels != 2)
    {
        throw ConverterError("panning converter supports only audio with 2 channels");
    }

    while (input->hasData())
    {
        auto channels = input->readNextSamplesChunk();
        auto chunk_size = channels[0].size();

        for (std::size_t cur_sample = 0; cur_sample < chunk_size; ++cur_sample)
        {
            const double left_factor = std::sqrt(
                (1.0 + m_ear_dist_factor * m_ear_dist_factor + 2 * m_ear_dist_factor * std::cos(cur_angle)) /
                (1.0 + m_ear_dist_factor * m_ear_dist_factor + 2 * m_ear_dist_factor));

            const double right_factor = std::sqrt(
                (1.0 + m_ear_dist_factor * m_ear_dist_factor - 2 * m_ear_dist_factor * std::cos(cur_angle)) /
                (1.0 + m_ear_dist_factor * m_ear_dist_factor + 2 * m_ear_dist_factor));

            channels[0][cur_sample] *= left_factor;
            channels[1][cur_sample] *= right_factor;

            cur_angle += angle_addition;
        }

        while (std::abs(cur_angle) >= 2 * std::numbers::pi)
        {
            cur_angle -= cur_angle > 0 ? 2 * std::numbers::pi : -2 * std::numbers::pi;
        }

        processed_samples_count += chunk_size;
        output->writeNextSamplesChunk(channels);
    }
}
