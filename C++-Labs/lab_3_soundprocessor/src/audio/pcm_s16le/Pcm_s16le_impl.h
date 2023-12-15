#ifndef WAVPCMAUDIOFILE_H
#define WAVPCMAUDIOFILE_H

#include "audio/AudioFile.h"
#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"


class Pcm_s16le_AudioOutput;
class Pcm_s16le_AudioInput;

class Pcm_s16le_AudioFile : public AudioFile
{
public:
    /// create empty file (useless until open is called)
    Pcm_s16le_AudioFile() = default;

    /// open file
    explicit Pcm_s16le_AudioFile(const fs::path &file_path);

    void open(const fs::path &file_path) override;

    std::unique_ptr<AudioFile> copyWithoutDataTo(const fs::path& file_path) override;

    std::unique_ptr<AudioOutput> beginWriting() override;
    std::unique_ptr<AudioInput> beginReading() override;

private:

    void parseHeader();

    struct WavHeader
    {
        char chunkId[4]; // "RIFF"
        uint32_t chunkSize; // size without chunkId and chunkSize

        char format[4]; // "WAVE"
        char subchunk1Id[4]; // "fmt "
        uint32_t subchunk1Size; // 16 for PCM format
        uint16_t audioFormat; // 1 for PCM
        uint16_t numChannels;
        uint32_t sampleRate; // in Hz

        // sampleRate * numChannels * bitsPerSample/8
        uint32_t byteRate;

        // numChannels * bitsPerSample/8
        // Bytes count for one sample, including all channels
        uint16_t blockAlign;
        uint16_t bitsPerSample;

        char subchunk2Id[4]; // "data"
        uint32_t subchunk2Size; // numSamples * numChannels * bitsPerSample/8
    };

    friend Pcm_s16le_AudioInput;
    friend Pcm_s16le_AudioOutput;

    WavHeader m_header;
};

class Pcm_s16le_AudioInput final : public AudioInput
{
public:
    explicit Pcm_s16le_AudioInput(Pcm_s16le_AudioFile* audio_file);

    [[nodiscard]] std::vector<AudioFile::SampleBuffer> readNextSamplesChunk() override;
};

class Pcm_s16le_AudioOutput final : public AudioOutput
{
public:
    explicit Pcm_s16le_AudioOutput(Pcm_s16le_AudioFile* audio_file);

    void writeNextSamplesChunk(const std::vector<AudioFile::SampleBuffer>& channels) override;
};


#endif //WAVPCMAUDIOFILE_H
