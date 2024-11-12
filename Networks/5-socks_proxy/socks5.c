#include "socks5.h"

#include "c.h"
#include "log.h"
#include "server.h"

/* Simple echo server for test */
void attribute_noreturn() socks5_main(client_t *client)
{
    log_debug("Hello from socks5_main coroutine");
    char msg_buf[4096];
    ssize_t res;
    
    for(;;) {
        res = read(server_client_sockfd(client), msg_buf, sizeof(msg_buf));

        if (!res) {
            server_drop_client(client);
        }

        if (res < 0) {
            if (errno == EAGAIN) {
                server_yield(client);
                continue;
            }

            log_error("read() failed: %s", strerror(errno));
            server_drop_client(client);
        }

        res = write(server_client_sockfd(client), msg_buf, (size_t)res);
        if (!res || res < 0) {
            log_error("write() failed: %s", strerror(errno));
            server_drop_client(client);
        }
    }

    unreachable();
}
