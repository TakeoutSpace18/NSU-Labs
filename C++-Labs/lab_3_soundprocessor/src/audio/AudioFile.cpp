#include "audio/AudioFile.h"

#include <fstream>
#include <fmt/format.h>

const AudioFile::Properties& AudioFile::getProperties() const
{
    return m_properties;
}

void AudioFile::openInputHandle()
{
    m_output_handle.reset(nullptr);

    auto file_input = std::make_unique<std::ifstream>(m_path, std::ios::binary);
    if (!file_input->is_open())
    {
        throw AudioFileError(fmt::format("Failed to open audio file {} for reading", m_path.string()));
    }

    m_input_handle = std::move(file_input);
}

void AudioFile::openOutputHandle()
{
    m_input_handle.reset(nullptr);

    auto file_output = std::make_unique<std::ofstream>(m_path, std::ios::binary);
    if (!file_output->is_open())
    {
        throw AudioFileError(fmt::format("Failed to open audio file {} for writing", m_path.string()));
    }

    m_output_handle = std::move(file_output);
}

AudioFile::AudioFileError::AudioFileError(const std::string& msg) :
    std::runtime_error(msg){}
