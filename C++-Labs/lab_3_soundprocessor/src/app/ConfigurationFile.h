#ifndef CONFIGURATIONFILE_H
#define CONFIGURATIONFILE_H
#include <filesystem>
#include <optional>
#include <vector>

#include "ConfigurationFile.h"


class ConfigurationFile
{
public:
    ConfigurationFile() = delete;
    explicit ConfigurationFile(const std::filesystem::path& path);
    explicit ConfigurationFile(std::unique_ptr<std::istream> stream);

    using ConverterName = std::string;
    using ConverterArgs = std::string;

    [[nodiscard]] std::optional<std::pair<ConverterName, ConverterArgs>> getNextCommand() const;

    struct CantOpenFileError final : std::runtime_error
    {
        explicit CantOpenFileError(const std::filesystem::path& path);
    };

    class InputIterator;

    InputIterator begin();
    InputIterator end();

private:
    std::unique_ptr<std::istream> m_stream;
};

class ConfigurationFile::InputIterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<ConverterName, ConverterArgs>;
    using pointer = value_type *;
    using reference = value_type &;

    // iterator pointing to end is created by default
    InputIterator() = default;
    explicit InputIterator(ConfigurationFile* configuration_file);

    reference operator*();
    pointer operator->();
    InputIterator& operator++();
    InputIterator operator++(int);

    friend bool operator==(const InputIterator &a, const InputIterator &b);

private:
    ConfigurationFile* m_configuration_file = nullptr;
    std::tuple<ConverterName, ConverterArgs> m_current_command;
};

#endif //CONFIGURATIONFILE_H
