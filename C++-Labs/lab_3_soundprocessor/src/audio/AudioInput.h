#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H
#include <vector>

#include "AudioFile.h"

class AudioInput {
public:
    virtual ~AudioInput();

    [[nodiscard]] virtual std::vector<std::vector<int64_t>> readNextChannels() = 0;
    [[nodiscard]] virtual bool hasData() const;

    [[nodiscard]] const AudioFile::Properties& properties() const;

protected:
    explicit AudioInput(AudioFile* audio_file);

    std::istream& getInputHandle();

    std::istream::pos_type m_start_pos;
    AudioFile* m_associated_audio_file;
};



#endif //AUDIOINPUT_H
