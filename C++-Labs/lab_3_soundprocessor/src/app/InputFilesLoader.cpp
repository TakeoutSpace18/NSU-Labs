#include "InputFilesLoader.h"

#include "audio/Pcm_s16le_AudioFile.h"

InputFilesLoader::InputFilesLoader(const std::vector<fs::path>& file_paths) : m_file_paths(file_paths),
    m_holder(m_file_paths.size(), nullptr)
{
}

InputFilesLoader::InputFilesLoader(std::vector<fs::path>&& file_paths) : m_file_paths(std::move(file_paths)),
    m_holder(m_file_paths.size(), nullptr)
{
}

std::shared_ptr<AudioFile> InputFilesLoader::getByIndex(const std::size_t index)
{
    if (!m_holder[index])
    {
        m_holder[index] = std::make_shared<Pcm_s16le_AudioFile>(m_file_paths[index]);
    }

    return m_holder[index];
}
