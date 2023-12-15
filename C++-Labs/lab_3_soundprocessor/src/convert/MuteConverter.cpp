#include "MuteConverter.h"

#include <fmt/format.h>
#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "ConverterArgumentsUtils.h"
#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"

MuteConverter::MuteConverter(const std::string& args)
{
    namespace po = boost::program_options;
    m_options_description.add_options()
            ("start", po::value<int64_t>()->required(),"time (in seconds) from when to start muting")
            ("stop", po::value<int64_t>()->required(), "time (in seconds) when to end muting");

    po::positional_options_description pos_desc;
    pos_desc.add("start", 1);
    pos_desc.add("stop", 1);

    auto vm = parseArguments(m_options_description, pos_desc, args);

    m_start_second = (*vm)["start"].as<int64_t>();
    m_stop_second = (*vm)["stop"].as<int64_t>();
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

            for (auto& ch : channels)
            {
                std::fill(ch.begin() + chunk_mute_start, ch.begin() + chunk_mute_stop, 0);
            }
        }

        output->writeNextSamplesChunk(channels);
        processed_samples_count += chunk_size;
    }
}

void MuteConverter::printDescription(std::ostream& os)
{
    os << m_options_description << '\n';
}
