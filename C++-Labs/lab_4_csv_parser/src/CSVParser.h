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
        CharT newline_delimiter = '\n';
        CharT escape_character = '\\';
    };

    explicit basic_CSVParser(
        std::basic_istream<CharT, Traits>&istream,
        std::size_t skip_first_lines_count = 0,
        Config config = Config())
        : istream_(istream),
          config_(config) {
        for (auto i: std::views::iota(0ul, skip_first_lines_count)) {
            istream_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    basic_CSVParser& operator>>(std::tuple<Types...>&tuple) {
        std::basic_string<CharT, Traits> line;
        std::getline(istream_, line, config_.newline_delimiter);

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
        ParseError(const std::string&line, const std::string&message);
    };

    std::basic_istream<CharT, Traits>&istream_;
    const Config config_;
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
