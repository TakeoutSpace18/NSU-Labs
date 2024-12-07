#include "proxy.h"

#include <stdio.h>

#define BUFSIZE 512

void attribute_noreturn() proxy_main(void)
{
    char client_buf[BUFSIZE];
    fdwatcher_t client_watcher;

    client_watcher = client_fdwatcher();

    for (;;) {
        client_fd_setevents(client_watcher, EV_READ);

        ssize_t ret = client_recv(client_watcher, client_buf, BUFSIZE, 0);
        if (ret == 0) {
            client_drop();
        }
        if (ret < 0) {
            client_log_error("client_recv() failed: %s", strerror(errno));
            client_drop();
        }
        fwrite(client_buf, 1, ret, stdout);
        fflush(stdout);
    }

    unreachable();
}
