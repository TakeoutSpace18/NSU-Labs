#include "Pcm_s16le_AudioFile.h"

#include <fstream>
#include <fmt/format.h>

void Pcm_s16le_AudioFile::parseHeader()
{
    m_input_stream->read(reinterpret_cast<char *>(&m_header), sizeof(m_header));
    if (m_input_stream->fail())
    {
        throw AudioFileError(fmt::format("Failed to read header in file {}", m_path.string()));
    }

    // skip uninteresting chunks until data is found
    while (std::memcmp(m_header.subchunk2Id, "data", 4) != 0)
    {
        // TODO: save unused chunks
        m_input_stream->seekg(m_header.subchunk2Size, std::ios::cur);
        m_input_stream->read(m_header.subchunk2Id, sizeof(m_header.subchunk2Id));
        m_input_stream->read(reinterpret_cast<char *>(&m_header.subchunk2Size), sizeof(m_header.subchunk2Size));
    }
}

void Pcm_s16le_AudioFile::open(const fs::path& file_path)
{
    AudioFile::open(file_path);
    parseHeader();
}

Pcm_s16le_AudioFile::Pcm_s16le_AudioFile(const fs::path& file_path)
{
    Pcm_s16le_AudioFile::open(file_path);
}

void Pcm_s16le_AudioFile::save(const fs::path& file)
{
}
