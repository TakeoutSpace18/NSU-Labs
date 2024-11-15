#include <getopt.h>

#include "c.h"
#include "socks5.h"
#include "server.h"
#include "log.h"

#define PORT_DEFAULT 1080

static void
do_help(void)
{
    printf("SOCKS5 Proxy");
    printf("Options:\n");
    printf("\t-p, --port\tproxy server port (1080 by default)\n");
    printf("\t-l, --log-level\tproxy logging level (0 - TRACE, 5 - FATAL)\n");
    printf("\t-h, --help\n");
}

int
main(int argc, char** argv)
{
    log_debug("Process PID: %i", getpid());

    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {"log-level", required_argument, NULL, 'l'},
        {NULL, 0, NULL, 0}
    };

    uint16_t port = PORT_DEFAULT;

    int c;
    int option_index;

    while ((c = getopt_long(argc, argv, "hp:l:",
                            long_options, &option_index)) != -1) {
        switch (c) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'l':
                log_set_level(atoi(optarg));
                break;
            case 'h':
                do_help();
                exit(EXIT_SUCCESS);
            default:
                /* getopt_long already issued a suitable error message */
                exit(EXIT_FAILURE);
        }
    }

    server_t server;

    if (server_init(&server, port, socks5_main) == -1) {
        return EXIT_FAILURE;
    }

    log_info("Started SOCKS5 Proxy on port %u", port);

    server_run(&server);
    return EXIT_SUCCESS;

}

