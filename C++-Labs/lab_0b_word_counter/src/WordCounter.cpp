#include <fstream>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "WordCounter.h"

WordCounter::WordCounter() : total_words_count_(0) {

}

void WordCounter::WriteFileWordFrequencyToCSV(const std::string& input_filename, const std::string& output_filename) {
    WordCounter counter;
    counter.parseFile(input_filename);
    counter.writeToCSV(output_filename);
}

std::vector<std::wstring> WordCounter::splitLineToWords(const std::wstring &line) {
    std::vector<std::wstring> words;
    std::wstring curr_word;

    for (wchar_t c : line) {
        if (std::isalnum(c, std::locale()) || c == '\'') {
            curr_word += std::tolower(c, std::locale());
        }
        else if (!curr_word.empty()) {
            words.push_back(curr_word);
            curr_word = L"";
        }
    }

    if (!curr_word.empty()) {
        words.push_back(curr_word);
    }

    return words;
}
void WordCounter::parseFile(const std::string &filename) {
    validateFilenameExtension(filename, "txt");

    std::wifstream input(filename);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open input file");
    }

    for (std::wstring line; std::getline(input, line); ) {
        for (const std::wstring& word : splitLineToWords(line)) {
            if (!words_frequency_.contains(word)) {
                words_.push_back(word);
            }

            words_frequency_[word]++;
            total_words_count_++;
        }
    }

    std::sort(words_.begin(), words_.end(),
              [&freq = words_frequency_](const auto& w1, const auto& w2)
              { return freq[w1] > freq[w2]; });
}

void WordCounter::writeToCSV(const std::string &filename) const {
    validateFilenameExtension(filename, "csv");

    std::wofstream output(filename);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open output file");
    }

    for (const std::wstring& word : words_) {
        size_t freq = words_frequency_.at(word);
        float word_percentage = static_cast<float>(freq) * 100 / static_cast<float>(total_words_count_);
        output << word << "," << freq << "," << std::setprecision(3) << word_percentage << std::endl;
    }
}

void WordCounter::validateFilenameExtension(const std::string &filename, const std::string &extension) {
    if (!filename.ends_with("." + extension)) {
        throw std::invalid_argument("Wrong extension in " + filename + ": ." + extension + " expected.");
    }
}
