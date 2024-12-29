#include "utils.h"
#include "c.h"

size_t strtoull_n(const char *str, size_t n) {
    if (str == NULL || n == 0) {
        return 0;
    }

    char temp[64];
    assert(n < sizeof(temp));

    memcpy(temp, str, n);
    temp[n] = '\0';

    return strtoull(temp, NULL, 10);
}
