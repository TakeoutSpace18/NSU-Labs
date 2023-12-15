#include "Pcm_s16le_impl.h"

#include <cassert>
#include <fstream>
#include <fmt/format.h>

std::unique_ptr<AudioInput> Pcm_s16le_AudioFile::beginReading()
{
    AudioFile::checkAvailableForReading();
    return std::make_unique<Pcm_s16le_AudioInput>(this);
}

void Pcm_s16le_AudioFile::parseHeader()
{
    m_input_handle->read(reinterpret_cast<char *>(&m_header), sizeof(m_header));
    if (m_input_handle->fail())
    {
        throw AudioFileError(fmt::format("Failed to read header in file {}", m_path.string()));
    }

    // skip uninteresting chunks until data is found
    while (std::memcmp(m_header.subchunk2Id, "data", 4) != 0)
    {
        // TODO: save unused chunks
        m_input_handle->seekg(m_header.subchunk2Size, std::ios::cur);
        m_input_handle->read(m_header.subchunk2Id, sizeof(m_header.subchunk2Id));
        m_input_handle->read(reinterpret_cast<char *>(&m_header.subchunk2Size), sizeof(m_header.subchunk2Size));
    }
}

void Pcm_s16le_AudioFile::open(const fs::path& file_path)
{
    m_path = file_path;
    AudioFile::openInputHandle();
    parseHeader();
    m_properties.sample_rate = m_header.sampleRate;
    m_properties.num_channels = m_header.numChannels;
    m_properties.sample_max_value = std::numeric_limits<int16_t>::max();
    m_properties.sample_min_value = std::numeric_limits<int16_t>::min();
    m_state = State::Free;
}

std::unique_ptr<AudioFile> Pcm_s16le_AudioFile::copyWithoutDataTo(const fs::path& file_path)
{
    auto new_file = std::make_unique<Pcm_s16le_AudioFile>();
    new_file->m_path = file_path;
    new_file->openOutputHandle();
    new_file->m_output_handle->write(reinterpret_cast<char *>(&m_header), sizeof(m_header));
    new_file->open(file_path);
    return new_file;
}

std::unique_ptr<AudioOutput> Pcm_s16le_AudioFile::beginWriting()
{
    AudioFile::checkAvailableForWriting();
    openOutputHandle();
    m_state = State::Writing;
    m_output_handle->write(reinterpret_cast<char *>(&m_header), sizeof(m_header));
    return std::make_unique<Pcm_s16le_AudioOutput>(this);
}

Pcm_s16le_AudioFile::Pcm_s16le_AudioFile(const fs::path& file_path)
{
    Pcm_s16le_AudioFile::open(file_path);
}

Pcm_s16le_AudioInput::Pcm_s16le_AudioInput(Pcm_s16le_AudioFile* audio_file) : AudioInput(
    audio_file)
{
}

std::vector<AudioFile::SampleBuffer> Pcm_s16le_AudioInput::readNextSamplesChunk()
{
    auto& props = m_associated_audio_file->getProperties();

    assert(props.num_channels != 0);
    std::vector<AudioFile::SampleBuffer> channels;
    channels.reserve(props.num_channels);
    for (int i = 0; i < props.num_channels; ++i)
    {
        channels.emplace_back();
        channels.back().reserve(props.sample_rate);
    }

    for (std::size_t i = 0; i < props.sample_rate; ++i)
    {
        for (auto& channel: channels)
        {
            int16_t sample;
            getInputHandle().read(reinterpret_cast<char *>(&sample), sizeof(sample));
            if (!getInputHandle().good())
                return channels;

            channel.push_back(sample);
        }
    }
    return channels;
}

Pcm_s16le_AudioOutput::Pcm_s16le_AudioOutput(Pcm_s16le_AudioFile* audio_file) : AudioOutput(
    audio_file)
{
}

void Pcm_s16le_AudioOutput::writeNextSamplesChunk(const std::vector<AudioFile::SampleBuffer>& channels)
{
    assert(!channels.empty());
    for (std::size_t i = 0; i < channels[0].size(); ++i)
    {
        for (auto& channel: channels)
        {
            //TODO: handle different sample types
            getOutputHandle().write(reinterpret_cast<const char *>(&channel[i]), sizeof(int16_t));
        }
    }
}
