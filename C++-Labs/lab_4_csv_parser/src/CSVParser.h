#ifndef CSVPARSER_H
#define CSVPARSER_H
#include <istream>
#include <limits>

#include <ranges>

template<class CharT, class Traits, class... Types>
class basic_CSVParser {
public:
    struct InputIterator;

    struct Config {
        CharT column_delimiter = ',';
        CharT line_delimiter = '\n';
        CharT escape_character = '\\';
    };

    void skipLines(std::size_t skip_first_lines_count) {
        for (auto i: std::views::iota(0ul, skip_first_lines_count)) {
            istream_.ignore(std::numeric_limits<std::streamsize>::max(), config_.line_delimiter);
        }
    }

    explicit basic_CSVParser(
        std::basic_istream<CharT, Traits>&istream,
        const std::size_t skip_first_lines_count = 0,
        Config config = Config())
        : istream_(istream),
          config_(config) {
        skipLines(skip_first_lines_count);
    }

    basic_CSVParser& operator>>(std::tuple<Types...>& tuple) {
        std::basic_string<CharT, Traits> line;
        std::getline(istream_, line, config_.line_delimiter);
        std::vector<std::basic_string<CharT, Traits>> words;
        bool last_delim_was_escaped = false;
        for (size_t delim_idx = line.find(config_.column_delimiter);
            delim_idx != std::string::npos;
            delim_idx = line.find(config_.column_delimiter, delim_idx + 1))
        {
            if (line[delim_idx - 1] == config_.escape_character) {
                auto line_part = line.substr(0, delim_idx - 1) + config_.column_delimiter;
                if (last_delim_was_escaped) {
                    words.back() += line_part;
                }
                else {
                    words.push_back(line_part);
                }
                last_delim_was_escaped = true;
            }
            else { // word ends
                auto line_part = line.substr(0, delim_idx);
                if (last_delim_was_escaped) {
                    words.back() += line_part;
                }
                else {
                    words.push_back(line_part);
                }
                last_delim_was_escaped = false;
            }
            line = line.substr(delim_idx + 1);
        }

        if (words.size() != sizeof...(Types)) {
            throw ParseError(current_line_number_, "columns number doesn't match");
        }
        
        words.push_back(line);
        ++current_line_number_;
        return *this;
    }

    bool fail() {
        return istream_.fail();
    }

    auto begin() {
        return InputIterator(this);
    }

    auto end() const {
        return InputIterator();
    }

private:
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& line, const std::string& message) :
            runtime_error("Error while parsing line \"" + line + "\": " + message)
        {}
    };

    std::basic_istream<CharT, Traits>&istream_;
    const Config config_;
    size_t current_line_number_ = 0;
};



template<class CharT, class Traits, class... Types>
struct basic_CSVParser<CharT, Traits, Types...>::InputIterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Types...>;
    using pointer = value_type *;
    using reference = value_type &;

    InputIterator() = default;

    explicit InputIterator(basic_CSVParser* parser) : parser_(parser) {
        (*parser_) >> cur_data_;
    }

    reference operator*() {
        return cur_data_;
    }

    pointer operator->() {
        return &cur_data_;
    }

    InputIterator& operator++() {
        (*parser_) >> cur_data_;
        if (parser_->fail()) {
            parser_ = nullptr;
        }
        return *this;
    }

    InputIterator operator++(int) {
        InputIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const InputIterator&a, const InputIterator&b) {
        return (a.parser_ == b.parser_);
    };

private:
    basic_CSVParser* parser_ = nullptr;
    std::tuple<Types...> cur_data_;
};

template<class... Ts>
using CSVParser = basic_CSVParser<char, std::char_traits<char>, Ts...>;

template<class... Ts>
using wCSVParser = basic_CSVParser<wchar_t, std::char_traits<wchar_t>, Ts...>;

#endif //CSVPARSER_H
