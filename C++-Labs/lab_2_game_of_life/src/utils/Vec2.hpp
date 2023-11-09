#ifndef LAB_2_GAME_OF_LIFE_VEC2_HPP
#define LAB_2_GAME_OF_LIFE_VEC2_HPP

#include "imgui.h"

template<class T>
struct Vec2 {
    T x, y;

    constexpr Vec2(T x, T y) : x(x), y(y) {}

    template<typename U>
    constexpr explicit Vec2(const Vec2<U> &vector)
    : x(static_cast<T>(vector.x)),
      y(static_cast<T>(vector.y)) {}

    constexpr Vec2<T> operator-(const Vec2<T> &right) {
        return Vec2<T>(-right.x, -right.y);
    }
};

template<class T>
constexpr Vec2<T> &operator+=(Vec2<T> &left, const Vec2<T> &right) {
    left.x += right.x;
    left.y += right.y;

    return left;
}

template<typename T>
constexpr Vec2<T> &operator-=(Vec2<T> &left, const Vec2<T> &right) {
    left.x -= right.x;
    left.y -= right.y;

    return left;
}

template<typename T>
constexpr Vec2<T> operator+(const Vec2<T> &left, const Vec2<T> &right) {
    return Vec2<T>(left.x + right.x, left.y + right.y);
}

template<typename T>
constexpr Vec2<T> operator-(const Vec2<T> &left, const Vec2<T> &right) {
    return Vec2<T>(left.x - right.x, left.y - right.y);
}

template<typename T>
constexpr Vec2<T> operator*(const Vec2<T> &left, T right) {
    return Vec2<T>(left.x * right, left.y * right);
}

template<typename T>
constexpr Vec2<T> operator*(T left, const Vec2<T> &right) {
    return Vec2<T>(right.x * left, right.y * left);
}

template<typename T>
constexpr Vec2<T> &operator*=(Vec2<T> &left, T right) {
    left.x *= right;
    left.y *= right;

    return left;
}

template<typename T>
constexpr Vec2<T> operator/(const Vec2<T> &left, T right) {
    assert(right != 0 && "Vec2::operator/ cannot divide by 0");
    return Vec2<T>(left.x / right, left.y / right);
}

template<typename T>
constexpr Vec2<T> &operator/=(Vec2<T> &left, T right) {
    assert(right != 0 && "Vec2::operator/= cannot divide by 0");
    left.x /= right;
    left.y /= right;

    return left;
}

template<typename T>
constexpr bool operator==(const Vec2<T> &left, const Vec2<T> &right) {
    return (left.x == right.x) && (left.y == right.y);
}

template<typename T>
constexpr bool operator!=(const Vec2<T> &left, const Vec2<T> &right) {
    return (left.x != right.x) || (left.y != right.y);
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned int>;

#endif //LAB_2_GAME_OF_LIFE_VEC2_HPP
