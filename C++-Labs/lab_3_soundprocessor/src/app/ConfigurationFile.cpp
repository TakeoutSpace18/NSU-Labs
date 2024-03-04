#include "ConfigurationFile.h"

#include <fstream>
#include <fmt/format.h>

ConfigurationFile::ConfigurationFile(const std::filesystem::path &path)
{
    auto file_stream = std::make_unique<std::ifstream>(path);
    if (!file_stream->is_open()) {
        throw ConfigurationFileError(fmt::format("Can't open configuration file {}", path.string()));
    }
    m_stream = std::move(file_stream);
}

ConfigurationFile::ConfigurationFile(std::unique_ptr<std::istream> stream)
{
    m_stream = std::move(stream);
}

std::optional<std::pair<ConfigurationFile::ConverterName,ConfigurationFile::ConverterArgs>>
ConfigurationFile::getNextCommand() const
{
    std::string line;
    do {
        std::getline(*m_stream, line);
    } while (m_stream->good() && (line.starts_with('#') || line.empty())); // skip comments

    if (!m_stream->good() && (line.starts_with('#') || line.empty())) {
        return std::nullopt;
    }

    auto first_delim = line.find(' ');

    ConverterName name = line.substr(0, first_delim);
    ConverterArgs args = line.substr(first_delim + 1);

    return std::make_pair(std::move(name), std::move(args));
}


ConfigurationFile::InputIterator ConfigurationFile::end()
{
    return InputIterator();
}

void ConfigurationFile::printHelpMessage(std::ostream& os)
{
    os << "Configuration file syntax:\n";
    os << "\t<converter name> <converter args...>\n";
    os << "\t# comments are supported\n";
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

ConfigurationFileError::ConfigurationFileError(const std::string& message) : std::runtime_error(message)
{
}
