#include "WavPCMAudioFile.h"

#include <fstream>

WavPCMAudioFile::WavPCMAudioFile(const fs::path& file)
{
    std::ifstream input(file, std::ios::binary);
    if (!input.is_open()) {
        throw AudioFileError(file);
    }
    input.read(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    if (input.fail()) {
        // throw
    }
}
