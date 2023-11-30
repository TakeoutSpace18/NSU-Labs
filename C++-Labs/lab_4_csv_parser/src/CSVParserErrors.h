#ifndef CSVPARSERERRORS_H
#define CSVPARSERERRORS_H

#include <stdexcept>
#include <string>
#include <fmt/format.h>

namespace CSVParserErrors {
    class WrongColumnCount : public std::runtime_error {
    public:
        WrongColumnCount(size_t row,
                         size_t expected_number);
    };


    class TypeMismatch : public std::runtime_error {
    public:
        TypeMismatch(size_t row,
                     size_t col,
                     const std::string &field,
                     const std::string &desired_type);
    };

    class EmptyField : public std::runtime_error {
    public:
        EmptyField(size_t row,
                   size_t col,
                   const std::string &desired_type);
    };
}

#endif //CSVPARSERERRORS_H
