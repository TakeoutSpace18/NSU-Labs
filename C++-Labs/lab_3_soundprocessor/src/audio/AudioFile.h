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

    virtual void write(const fs::path& file) = 0;

    struct AudioFileError : public std::runtime_error
    {
        explicit AudioFileError(const std::string& msg);
    };


protected:
    std::unique_ptr<AudioStream> m_audio_stream;
};



#endif //IAUDIOFILE_H
