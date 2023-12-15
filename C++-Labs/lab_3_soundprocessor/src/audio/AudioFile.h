#ifndef IAUDIOFILE_H
#define IAUDIOFILE_H
#include <memory>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class AudioInput;
class AudioOutput;

class AudioFile {
public:

    /// generic audio props, supposed to be filled by derived classes
    struct Properties
    {
        int64_t sample_max_value;
        int64_t sample_min_value;
        int64_t sample_rate;
        int64_t num_channels;
    };

    using SampleBuffer = std::vector<int64_t>;

    AudioFile() = default;
    virtual ~AudioFile() = default;

    virtual void open(const fs::path& file_path) = 0;

    virtual std::unique_ptr<AudioFile> copyWithoutDataTo(const fs::path& file_path) = 0;

    /// write to the same file path (overwrite)
    virtual std::unique_ptr<AudioOutput> beginWriting() = 0;
    virtual std::unique_ptr<AudioInput> beginReading() = 0;


    [[nodiscard]] const Properties& getProperties() const;
    struct AudioFileError : std::runtime_error
    {
        explicit AudioFileError(const std::string& msg);
    };

protected:
    enum class State
    {
        Reading,
        Writing,
        Free /// file properties are correctly parsed and file is ready for writing/reading data.
    };

    friend AudioInput;
    friend AudioOutput;

    void openInputHandle();
    void openOutputHandle();

    void checkAvailableForReading() const;
    void checkAvailableForWriting() const;

    fs::path m_path;
    std::unique_ptr<std::istream> m_input_handle;
    std::unique_ptr<std::ostream> m_output_handle;
    Properties m_properties;
    State m_state = State::Free;
};



#endif //IAUDIOFILE_H
