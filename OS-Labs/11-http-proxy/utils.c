#include "utils.h"

#include <poll.h>

#include "c.h"
#include "log.h"

size_t strtoull_n(const char *str, size_t n, size_t base) {
    if (str == NULL || n == 0) {
        return 0;
    }

    char temp[64];
    assert(n < sizeof(temp));

    memcpy(temp, str, n);
    temp[n] = '\0';

    return strtoull(temp, NULL, base);
}

void proper_socket_close(int sfd)
{
    /* https://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable */
    ssize_t err;
    char buffer[4096];
    shutdown(sfd, SHUT_WR);
    for(;;) {
        struct pollfd pollfd;
        pollfd.fd = sfd;
        pollfd.events = POLLIN;

        err = poll(&pollfd, 1, -1);
        if (err < 0) {
            log_error("poll(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        err = read(sfd, buffer, 4096);
        if (err < 0) {
            log_error("read(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (err == 0) {
            break;
        }
    }
    close(sfd);
}
