#ifndef IAUDIOFILE_H
#define IAUDIOFILE_H
#include <memory>
#include <filesystem>

#include "AudioStream.h"

namespace fs = std::filesystem;

class AudioFile {
public:
    AudioFile() = default;
    virtual ~AudioFile() = default;

    virtual void open(const fs::path& file_path);
    virtual void save(const fs::path& file) = 0;

    struct AudioFileError : std::runtime_error
    {
        explicit AudioFileError(const std::string& msg);
    };


protected:
    fs::path m_path;
    std::shared_ptr<std::istream> m_input_stream;
};



#endif //IAUDIOFILE_H
