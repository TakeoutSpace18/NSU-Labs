#ifndef PRINTTUPLE_H
#define PRINTTUPLE_H

#include <ostream>
#include <tuple>

template <class Ch, class Tr, class... Args>
auto &operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& tup) {
    [&] <size_t... Is> (std::index_sequence<Is...>)
    {
        os << "[";
        ((os << std::get<Is>(tup) << (Is != sizeof...(Args) - 1 ? ", " : "")), ...);
        os << "]";
    }(std::index_sequence_for<Args...>());
    return os;
}


#endif //PRINTTUPLE_H
