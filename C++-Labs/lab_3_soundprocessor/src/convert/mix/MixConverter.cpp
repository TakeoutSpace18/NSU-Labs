#include "MixConverter.h"

#include <iostream>
#include <boost/program_options.hpp>
#include <fmt/format.h>

#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"
#include "convert/InputFileRef.h"

MixConverterCreator::MixConverterCreator()
{
    namespace po = boost::program_options;
    m_options_description.add_options()
            ("mix-with", po::value<InputFileRef>()->required(),
             "input file reference in format '$<n>', where n is the index of specified input files")
            ("start", po::value<int64_t>()->default_value(0), "time (in seconds) from when to start mixing")
            ("stop", po::value<int64_t>()->default_value(-1), "time (in seconds) when to stop mixing");

    m_positional_description.add("mix-with", 1);
    m_positional_description.add("start", 1);
    m_positional_description.add("stop", 1);
}

std::unique_ptr<Converter> MixConverterCreator::create(const std::string& args,
                                                       InputFilesLoader& input_files)
{
    auto vm = parseArguments(args);

    InputFileRef mix_with = (*vm)["mix-with"].as<InputFileRef>();
    int64_t start_second = (*vm)["start"].as<int64_t>();
    int64_t stop_second = (*vm)["stop"].as<int64_t>();

    std::shared_ptr<AudioFile> mix_with_file;
    try
    {
        mix_with_file = input_files.getByIndex(mix_with);
    } catch (const std::invalid_argument& e)
    {
        throw ConverterError(fmt::format("Failed to create mix converter: {}",e.what()));
    }
    return std::make_unique<MixConverter>(mix_with_file, start_second,
                                          stop_second);
}

void MixConverterCreator::printDescription(std::ostream& os)
{
    os << "Mix converter\n";
    os << m_options_description;
}

std::string MixConverterCreator::name() const
{
    return "mix";
}

MixConverter::MixConverter(std::shared_ptr<AudioFile> mix_with,
                           int64_t start_second,
                           int64_t stop_second)
    : m_start_second(start_second),
      m_stop_second(stop_second),
      m_mix_with(mix_with)
{
}

void MixConverter::apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output)
{
    const int64_t mix_start_sample = input->properties().sample_rate * m_start_second;
    const int64_t mix_stop_sample = input->properties().sample_rate * m_stop_second;

    auto mix_with_input = m_mix_with->beginReading();

    if (std::memcmp(&input->properties(), &mix_with_input->properties(), sizeof(AudioFile::Properties)) != 0)
    {
        throw ConverterError("mixing audio with different properties is not supported");
    }

    int64_t processed_samples_count = 0;
    while (input->hasData())
    {
        auto channels = input->readNextSamplesChunk();
        auto mix_with_channels = mix_with_input->readNextSamplesChunk();
        auto chunk_size = static_cast<int64_t>(channels[0].size());


        if (mix_start_sample <= processed_samples_count + chunk_size && processed_samples_count < mix_stop_sample)
        {
            const int64_t chunk_mix_start = std::max(0l, mix_start_sample - processed_samples_count);
            const int64_t chunk_mix_stop = std::min(chunk_size, mix_stop_sample - processed_samples_count);

            for (int i = 0; i < channels.size(); ++i)
            {
                for (std::size_t j = chunk_mix_start; j < chunk_mix_stop; ++j)
                {
                    channels[i][j] += mix_with_channels[i][j];
                    channels[i][j] /= 2;
                }
            }
        }

        output->writeNextSamplesChunk(channels);
        processed_samples_count += chunk_size;
    }
}
