#include "InputFilesLoader.h"

#include <fmt/format.h>

#include "audio/pcm_s16le/Pcm_s16le_impl.h"

InputFilesLoader::InputFilesLoader(const std::vector<fs::path>& file_paths) : m_file_paths(file_paths),
    m_holder(m_file_paths.size(), nullptr)
{
}

InputFilesLoader::InputFilesLoader(std::vector<fs::path>&& file_paths) : m_file_paths(std::move(file_paths)),
    m_holder(m_file_paths.size(), nullptr)
{
}

std::shared_ptr<AudioFile> InputFilesLoader::getByIndex(InputFileRef ref)
{
    return getByIndex(ref.index());
}

std::shared_ptr<AudioFile> InputFilesLoader::getByIndex(const std::size_t index)
{
    if (index >= m_file_paths.size())
    {
        throw std::invalid_argument(fmt::format("input filename with index {} was not found", index));
    }

    if (!m_holder[index])
    {
        m_holder[index] = std::make_shared<Pcm_s16le_AudioFile>(m_file_paths[index]);
    }

    return m_holder[index];
}
