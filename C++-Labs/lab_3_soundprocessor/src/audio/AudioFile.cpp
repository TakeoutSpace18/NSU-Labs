#include "audio/AudioFile.h"

#include <fmt/format.h>

AudioFile::AudioFileError::AudioFileError(const std::string& msg) :
std::runtime_error(msg){}
