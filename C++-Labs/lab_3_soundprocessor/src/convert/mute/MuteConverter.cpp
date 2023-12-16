#include "MuteConverter.h"

#include <fmt/format.h>
#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"

MuteConverterCreator::MuteConverterCreator()
{
    namespace po = boost::program_options;
    m_options_description.add_options()
            ("start", po::value<int64_t>()->required(), "time (in seconds) from when to start muting")
            ("stop", po::value<int64_t>()->required(), "time (in seconds) when to end muting");

    m_positional_description.add("start", 1);
    m_positional_description.add("stop", 1);

}

std::unique_ptr<Converter> MuteConverterCreator::create(const std::string& args)
{
    auto vm = parseArguments(args);

    int64_t start_second = (*vm)["start"].as<int64_t>();
    int64_t end_second = (*vm)["end"].as<int64_t>();
    return std::make_unique<MuteConverter>(start_second, end_second);
}

void MuteConverterCreator::printDescription(std::ostream& os)
{
    os << "Mute converter\n";
    os << m_options_description;
}

std::string MuteConverterCreator::name() const
{
    return "mute";
}

MuteConverter::MuteConverter(int64_t start_second, int64_t stop_second) : m_start_second(start_second),
                                                                          m_stop_second(stop_second)
{
}

void MuteConverter::apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output)
{
    const int64_t mute_start_sample = input->properties().sample_rate * m_start_second;
    const int64_t mute_stop_sample = input->properties().sample_rate * m_stop_second;

    int64_t processed_samples_count = 0;
    while (input->hasData())
    {
        auto channels = input->readNextSamplesChunk();
        auto chunk_size = static_cast<int64_t>(channels[0].size());

        if (mute_start_sample <= processed_samples_count + chunk_size && processed_samples_count < mute_stop_sample)
        {
            const int64_t chunk_mute_start = std::max(0l, mute_start_sample - processed_samples_count);
            const int64_t chunk_mute_stop = std::min(chunk_size, mute_stop_sample - processed_samples_count);

            for (auto& ch: channels)
            {
                std::fill(ch.begin() + chunk_mute_start, ch.begin() + chunk_mute_stop, 0);
            }
        }

        output->writeNextSamplesChunk(channels);
        processed_samples_count += chunk_size;
    }
}