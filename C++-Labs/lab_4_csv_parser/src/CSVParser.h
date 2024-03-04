#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <limits>
#include <ranges>
#include <utility>
#include <fmt/format.h>

#include "CSVParserErrors.h"
#include "StringConversion.h"


template<class CharT, class Traits, class... Types>
class basic_CSVParser {
public:
    struct InputIterator;

    struct Config {
        CharT column_delimiter = ',';
        CharT line_delimiter = '\n';
        CharT escape_character = '\\';
    };

    explicit basic_CSVParser(
        std::basic_istream<CharT, Traits> &istream,
        const std::size_t skip_first_lines_count = 0,
        Config config = Config())
        : istream_(istream),
          config_(config) {
        skipLines(skip_first_lines_count);
    }

    void skipLines(std::size_t skip_first_lines_count) {
        for (auto _: std::views::iota(0ul, skip_first_lines_count)) {
            istream_.ignore(std::numeric_limits<std::streamsize>::max(), config_.line_delimiter);
        }
    }

    basic_CSVParser& operator>>(std::tuple<Types...> &tuple) {
        if (eofReached()) {
            return *this;
        }

        std::basic_string<CharT, Traits> line;
        std::getline(istream_, line, config_.line_delimiter);

        FieldExtractor fields{line, *this};
        std::apply([&](auto &... elems) {
            ((elems = fields.template extractNext<Types>()), ...);
        }, tuple);

        ++current_row_number_;

        if (!fields.finished()) {
            throw CSVParserErrors::WrongColumnCount(current_row_number_, sizeof...(Types));
        }
        return *this;
    }

    bool eofReached() {
        return istream_.rdbuf()->in_avail() == 0;
    }

    auto begin() {
        return InputIterator(this);
    }

    auto end() const {
        return InputIterator();
    }

private:
    class FieldExtractor;

    std::basic_istream<CharT, Traits> &istream_;
    const Config config_;
    size_t current_row_number_ = 1;
};


template<class CharT, class Traits, class... Types>
class basic_CSVParser<CharT, Traits, Types...>::FieldExtractor {
public:
    FieldExtractor(std::basic_string_view<CharT, Traits> row, basic_CSVParser &parser)
        : remained_row_(row), parser_(parser) {
    }

    template<class T>
    T convertFieldString(std::basic_string<CharT, Traits> field) {
        if constexpr (std::is_same<T, std::basic_string<CharT, Traits>>()) {
            return field;
        }

        if constexpr (std::is_arithmetic<T>()) {
            if (field.empty()) {
                throw CSVParserErrors::EmptyField(parser_.current_row_number_, current_column_number_,
                                                  typeid(T).name());
            }
        }

        T value;
        std::basic_stringstream<CharT, Traits> ss(field);
        ss >> value;
        if (ss.bad() || !ss.eof()) {
            throw CSVParserErrors::TypeMismatch(parser_.current_row_number_,
                                                current_column_number_,
                                                convert_basic_string_to_char_string(field),
                                                typeid(T).name());
        }
        return value;
    }

    std::basic_string<CharT, Traits> extractNextFieldInternal() {
        std::basic_string<CharT, Traits> extracted_field;

        for (size_t delim_idx = remained_row_.find(parser_.config_.column_delimiter);
             delim_idx != std::string::npos;
             delim_idx = remained_row_.find(parser_.config_.column_delimiter)) {
            if (remained_row_[delim_idx - 1] == parser_.config_.escape_character) {
                // delimiter with escape symbol found
                auto line_part = std::basic_string<CharT, Traits>(remained_row_.substr(0, delim_idx - 1)) + parser_.config_.column_delimiter;
                remained_row_ = remained_row_.substr(delim_idx + 1);
                extracted_field += line_part;
            }
            else {
                // field ends
                auto curr_chunk = remained_row_.substr(0, delim_idx);
                extracted_field += curr_chunk;
                remained_row_ = remained_row_.substr(delim_idx + 1);
                return extracted_field;
            }
        }
        finished_ = true;
        extracted_field += remained_row_;
        return extracted_field;
    }

    bool finished() const {
        return finished_;
    }

    template<class T>
    T extractNext() {
        if (finished()) {
            throw CSVParserErrors::WrongColumnCount(parser_.current_row_number_, sizeof...(Types));
        }
        T ret = convertFieldString<T>(extractNextFieldInternal());
        ++current_column_number_;
        return ret;
    }

private:
    std::basic_string_view<CharT, Traits> remained_row_;
    size_t current_column_number_ = 0;
    bool finished_ = false;
    basic_CSVParser &parser_;
};


template<class CharT, class Traits, class... Types>
struct basic_CSVParser<CharT, Traits, Types...>::InputIterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Types...>;
    using pointer = value_type *;
    using reference = value_type &;

    // iterator pointing to end is created by default
    InputIterator() = default;

    explicit InputIterator(basic_CSVParser* parser) : parser_(parser) {
        if (parser_->eofReached()) {
            parser_ = nullptr;
        }
        else {
            (*parser_) >> cur_data_;
        }
    }

    reference operator*() {
        return cur_data_;
    }

    pointer operator->() {
        return &cur_data_;
    }

    InputIterator& operator++() {
        if (parser_->eofReached()) {
            parser_ = nullptr;
            return *this;
        }
        (*parser_) >> cur_data_;
        return *this;
    }

    InputIterator operator++(int) {
        InputIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const InputIterator &a, const InputIterator &b) {
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
