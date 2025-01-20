#ifndef UTILS_H
#define UTILS_H

#include "c.h"

size_t strtoull_n(const char *str, size_t n, size_t base);

void proper_socket_close(int sfd);

#endif /* !UTILS_H */

