#include <fstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <codecvt>

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
    std::wifstream input(filename);

    if (!filename.ends_with(".txt")) {
        throw InputException("Wrong input file extension: only .txt files are allowed");
    }

    if (!input.is_open()) {
        throw InputException("Couldn't open file");
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

void WordCounter::writeToCSV(const std::string &filename) {
    if (!filename.ends_with(".csv")) {
        throw InputException("Wrong output file extension: only csv is allowed");
    }

    std::wofstream output(filename);

    for (const std::wstring& word : words_) {
        float word_percentage = static_cast<float>(words_frequency_[word]) * 100 / static_cast<float>(total_words_count_);
        output << word << "," << words_frequency_[word] << "," << std::setprecision(3) << word_percentage << std::endl;
    }
}

WordCounter::InputException::InputException(const char *message) : message_(message) {}

const char *WordCounter::InputException::what() const noexcept {
    return message_;
}
