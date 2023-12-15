#include "AudioInput.h"

#include <utility>

AudioInput::AudioInput(AudioFile* audio_file) : m_associated_audio_file(audio_file)
{
    m_start_pos = m_associated_audio_file->m_input_handle->tellg();
    m_associated_audio_file->m_state = AudioFile::State::Reading;
}

AudioInput::~AudioInput()
{
    m_associated_audio_file->m_input_handle->seekg(m_start_pos);
    m_associated_audio_file->m_state = AudioFile::State::Free;
}

bool AudioInput::hasData() const
{
    return m_associated_audio_file->m_input_handle->rdbuf()->in_avail() != 0;
}

const AudioFile::Properties& AudioInput::properties() const
{
    return m_associated_audio_file->getProperties();
}

std::istream& AudioInput::getInputHandle()
{
    return *m_associated_audio_file->m_input_handle;
}
