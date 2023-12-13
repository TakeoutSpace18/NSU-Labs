#include "audio/AudioFile.h"

#include <fstream>
#include <fmt/format.h>

void AudioFile::open(const fs::path& file_path)
{
    auto file_stream = std::make_shared<std::ifstream>(file_path, std::ios::binary);
    if (!file_stream->is_open())
    {
        throw AudioFileError(fmt::format("Failed to open audio file {}", file_path.string()));
    }
    m_input_stream = std::move(file_stream);
    m_path = file_path;
}

AudioFile::AudioFileError::AudioFileError(const std::string& msg) :
std::runtime_error(msg){}
