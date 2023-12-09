#include "ConfigurationFile.h"

#include <fstream>
#include <fmt/format.h>

ConfigurationFile::ConfigurationFile(const std::filesystem::path &path)
{
    auto file_stream = std::make_unique<std::ifstream>(path);
    if (!file_stream->is_open()) {
        throw CantOpenFileError(path);
    }
    m_stream = std::move(file_stream);
}

ConfigurationFile::ConfigurationFile(std::unique_ptr<std::istream> stream)
{
    m_stream = std::move(stream);
}

std::optional<std::tuple<ConfigurationFile::ConverterName,ConfigurationFile::ConverterArgs>>
ConfigurationFile::getNextCommand() const
{
    std::string line;
    do {
        std::getline(*m_stream, line);
    } while (m_stream->good() && (line.starts_with('#') || line.empty())); // skip comments

    if (!m_stream->good()) {
        return std::nullopt;
    }

    std::stringstream ss(line);

    ConverterName name;
    ss >> name;

    ConverterArgs args;
    std::copy(std::istream_iterator<std::string>(ss),
        std::istream_iterator<std::string>(),
        std::back_inserter(args));

    return std::make_tuple(std::move(name), std::move(args));
}

ConfigurationFile::CantOpenFileError::CantOpenFileError(const std::filesystem::path &path) : std::runtime_error(
    fmt::format("Can't open configuration file {}", path.string())) {}

ConfigurationFile::InputIterator ConfigurationFile::end()
{
    return InputIterator();
}

ConfigurationFile::InputIterator ConfigurationFile::begin()
{
    return InputIterator(this);
}


ConfigurationFile::InputIterator::InputIterator(ConfigurationFile* configuration_file): m_configuration_file(configuration_file)
{
    const auto cmd = m_configuration_file->getNextCommand();
    if (cmd.has_value()) {
        m_current_command = cmd.value();
    }
    else {
        m_configuration_file = nullptr;
    }
}

ConfigurationFile::InputIterator::reference ConfigurationFile::InputIterator::operator*()
{
    return m_current_command;
}

ConfigurationFile::InputIterator::pointer ConfigurationFile::InputIterator::operator->()
{
    return &m_current_command;
}

ConfigurationFile::InputIterator& ConfigurationFile::InputIterator::operator++()
{
    const auto cmd = m_configuration_file->getNextCommand();
    if (cmd.has_value()) {
        m_current_command = cmd.value();
    }
    else {
        m_configuration_file = nullptr;
    }
    return *this;
}

ConfigurationFile::InputIterator ConfigurationFile::InputIterator::operator++(int)
{
    InputIterator tmp = *this;
    ++(*this);
    return tmp;
}

bool operator==(const ConfigurationFile::InputIterator& a, const ConfigurationFile::InputIterator& b)
{
    return a.m_configuration_file == b.m_configuration_file;
}
