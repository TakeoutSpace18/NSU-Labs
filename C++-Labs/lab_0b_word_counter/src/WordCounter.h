#ifndef LAB_0B_WORD_COUNTER_WORDCOUNTER_H
#define LAB_0B_WORD_COUNTER_WORDCOUNTER_H

#include <string>
#include <vector>
#include <unordered_map>

class WordCounter {
public:
    static void WriteFileWordFrequencyToCSV(const std::string& input_filename, const std::string& output_filename);

private:
    WordCounter();

    void parseFile(const std::string &filename);
    void writeToCSV(const std::string &filename);

    static std::vector<std::string> splitLineToWords(const std::string &line);

    size_t total_words_count_;
    std::unordered_map<std::string, size_t> words_frequency_;
    std::vector<std::string> words_;
};

#endif //LAB_0B_WORD_COUNTER_WORDCOUNTER_H
