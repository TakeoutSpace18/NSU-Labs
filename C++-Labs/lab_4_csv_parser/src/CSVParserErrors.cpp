#include "CSVParserErrors.h"

CSVParserErrors::WrongColumnCount::WrongColumnCount(const size_t row, const size_t expected_number): runtime_error(
    fmt::format("Wrong column number at row {}. Expected {}", row, expected_number)) {
}

CSVParserErrors::TypeMismatch::TypeMismatch(const size_t row, const size_t col, const std::string& field,
    const std::string& desired_type): runtime_error(fmt::format(
    "Type mismatch at row {} column {}. Failed to convert \"{}\" to type {}",
    row, col, field, desired_type)) {
}

CSVParserErrors::EmptyField::EmptyField(const size_t row, const size_t col, const std::string& desired_type): runtime_error(fmt::format(
    "Empty field at row {} column {}. Failed to convert to type {}",
    row, col, desired_type)) {
}
