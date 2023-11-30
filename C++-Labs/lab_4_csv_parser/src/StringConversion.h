#ifndef STRINGCONVERSION_H
#define STRINGCONVERSION_H
#include <codecvt>
#include <locale>
#include <string>

template<class CharT, class Traits>
std::string convert_basic_string_to_char_string(std::basic_string<CharT, Traits> str) {
    if constexpr (!std::is_same<CharT, char>()) {
        using convert_type = std::codecvt_utf8<CharT>;
        std::wstring_convert<convert_type, CharT> converter;
        return converter.to_bytes(str);
    }
    else {
        return str;
    }
}

#endif //STRINGCONVERSION_H
