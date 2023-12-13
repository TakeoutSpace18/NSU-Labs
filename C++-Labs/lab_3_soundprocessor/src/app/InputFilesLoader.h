#ifndef INPUTFILESLOADER_H
#define INPUTFILESLOADER_H
#include <string>
#include <vector>

#include "audio/AudioFile.h"

namespace fs = std::filesystem;

class InputFilesLoader {
public:
    explicit InputFilesLoader(const std::vector<fs::path>& file_paths);
    explicit InputFilesLoader(std::vector<fs::path>&& file_paths);

    std::shared_ptr<AudioFile> getByIndex(std::size_t index);

private:
    std::vector<fs::path> m_file_paths;
    std::vector<std::shared_ptr<AudioFile>> m_holder;
};



#endif //INPUTFILESLOADER_H
