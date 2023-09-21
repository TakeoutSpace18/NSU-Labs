#ifndef LAB_0B_WORD_COUNTER_WORDCOUNTER_H
#define LAB_0B_WORD_COUNTER_WORDCOUNTER_H

#include <string>
#include <vector>
#include <unordered_map>

class WordCounter {
public:
    class InputException : public std::exception {
    public:
        explicit InputException(const char *message);
        [[nodiscard]] const char * what() const noexcept override;
    private:
        const char* message_;
    };

    static void WriteFileWordFrequencyToCSV(const std::string& input_filename, const std::string& output_filename);

private:
    WordCounter();

    void parseFile(const std::string &filename);
    void writeToCSV(const std::string &filename);

    static std::vector<std::wstring> splitLineToWords(const std::wstring &line);

    size_t total_words_count_;
    std::unordered_map<std::wstring, size_t> words_frequency_;
    std::vector<std::wstring> words_;
};

#endif //LAB_0B_WORD_COUNTER_WORDCOUNTER_H
