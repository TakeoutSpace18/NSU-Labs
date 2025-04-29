#include <iostream>
#include <fstream>
#include <stdexcept>

#define FORCEINLINE __attribute__((always_inline)) inline

namespace Utils {

template <class T>
struct Vec2
{
    T x;
    T y;
};

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;

static void writeToFile(const std::string& filename, const char* data, size_t size)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    file.write(data, size);
    file.close();
}

} // namespace Utils
