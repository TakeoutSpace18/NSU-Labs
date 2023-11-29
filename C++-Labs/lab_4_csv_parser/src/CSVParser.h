#ifndef CSVPARSER_H
#define CSVPARSER_H
#include <istream>
#include <limits>

#include <ranges>
#include <utility>
#include <fmt/format.h>

template<class CharT, class Traits, class... Types>
class basic_CSVParser {
public:
    struct InputIterator;

    class FieldExtractor {
    public:
        FieldExtractor(std::basic_string_view<CharT, Traits> row, basic_CSVParser& parser)
        : remained_row_(row), parser_(parser){}

        template <class T>
        T convertFieldString(std::basic_string<CharT, Traits> field) {
            T value;
            std::basic_stringstream<CharT, Traits> ss(field);
            ss >> value;
            if (ss.bad()) {
                throw TypeMismatchError(parser_.current_row_number_, current_column_number_, field, typeid(T).name());
            }
            return value;
        }

        std::basic_string<CharT, Traits> extractNextFieldInternal() {
            std::basic_string<CharT, Traits> extracted_field;

            for (size_t delim_idx = remained_row_.find(parser_.config_.column_delimiter);
                delim_idx != std::string::npos;
                delim_idx = remained_row_.find(parser_.config_.column_delimiter))
            {
                if (remained_row_[delim_idx - 1] == parser_.config_.escape_character) {
                    // delimiter with escape symbol found
                    auto line_part = std::string(remained_row_.substr(0, delim_idx - 1)) + parser_.config_.column_delimiter;
                    remained_row_ = remained_row_.substr(delim_idx + 1);
                    extracted_field += line_part;
                }
                else { // field ends
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

        bool finished() {
            return finished_;
        }

        template <class T>
        T extractNext() {
            if (finished()) {
                throw ColumnNumberError(parser_.current_row_number_, sizeof...(Types));
            }
            T ret = convertFieldString<T>(extractNextFieldInternal());
            ++current_column_number_;
            return ret;
        }


    private:
        std::basic_string_view<CharT, Traits> remained_row_;
        size_t current_column_number_ = 0;
        bool finished_ = false;
        basic_CSVParser& parser_;
    };

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
        if (eofReached()) {
            return *this;
        }

        std::basic_string<CharT, Traits> line;
        std::getline(istream_, line, config_.line_delimiter);

        FieldExtractor fields{line, *this};
        std::apply([&](auto&... elems) {
            ((elems = fields.template extractNext<Types>()), ...);
        }, tuple);

        ++current_row_number_;
        return *this;
    }

    bool eofReached() {
        return istream_.eof();
    }

    auto begin() {
        return InputIterator(this);
    }

    auto end() const {
        return InputIterator();
    }

private:
    class ColumnNumberError : public std::runtime_error {
    public:
        ColumnNumberError(const size_t row, const size_t expected_number) :
            runtime_error(fmt::format("Wrong column number at row {}. Expected {}", row, expected_number))
        {}
    };

    class TypeMismatchError : public std::runtime_error {
    public:
        TypeMismatchError(const size_t row, const size_t col, const std::string& field, const std::string& desired_type) :
            runtime_error(fmt::format("Type mismatch at row {} column {}. Failed to convert \"{}\" to type {}",
                row, col, field, desired_type))
        {}
    };


    std::basic_istream<CharT, Traits>&istream_;
    const Config config_;
    size_t current_row_number_ = 0;
};



template<class CharT, class Traits, class... Types>
struct basic_CSVParser<CharT, Traits, Types...>::InputIterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Types...>;
    using pointer = value_type*;
    using reference = value_type&;

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
        if (parser_->eofReached()) {
            parser_ = nullptr;
        }
        return *this;
    }

    InputIterator operator++(int) {
        InputIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const InputIterator& a, const InputIterator& b) {
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
