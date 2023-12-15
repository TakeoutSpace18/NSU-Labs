#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "audio/AudioFile.h"


class AudioOutput {
public:
    virtual ~AudioOutput();

    virtual void writeNextChannels(const std::vector<AudioFile::SampleBuffer>& channels) = 0;

    [[nodiscard]] const AudioFile::Properties& properties() const;

protected:
    explicit AudioOutput(AudioFile* audio_file);

    std::ostream& getOutputHandle();

    AudioFile* m_associated_audio_file;
};



#endif //AUDIOOUTPUT_H
