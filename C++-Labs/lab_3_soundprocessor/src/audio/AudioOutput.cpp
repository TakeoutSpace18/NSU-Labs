#include "AudioOutput.h"

std::ostream& AudioOutput::getOutputHandle()
{
    return *m_associated_audio_file->m_output_handle;
}

AudioOutput::AudioOutput(AudioFile* audio_file) : m_associated_audio_file(audio_file)
{
    m_associated_audio_file->m_state = AudioFile::State::Writing;
}

AudioOutput::~AudioOutput()
{
    m_associated_audio_file->open(m_associated_audio_file->m_path);
    m_associated_audio_file->m_state = AudioFile::State::Free;
}
