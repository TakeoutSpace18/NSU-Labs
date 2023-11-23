#ifndef PRINTTUPLE_H
#define PRINTTUPLE_H

#include <ostream>
#include <tuple>

template <class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& tup) {
    os << std::get<>;
}

template <class Ch, class Tr>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<> const& tup) {}

template <class Ch, class Tr, class A0, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& tup) {
    os << std::get<A0>;
}

#endif //PRINTTUPLE_H
