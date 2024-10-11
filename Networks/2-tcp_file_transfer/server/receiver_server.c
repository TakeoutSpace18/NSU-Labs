#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "client_thread.h"
#include "log.h"
#include "error.h"
#include "common.h"

const char *storage_dir = "./uploads";

static char *port = "6666";

static bool is_interrupted = false;

static void sigint_handler(int signo);
static Result_t setup_signal_handlers(void);
static Result_t create_listening_socket(char *port, int *sockfd);
static Result_t run_accept_clients_loop(int sockfd, ActiveClients_t *clients);

static void
do_help(void)
{
    printf("A server for recieving files with tcp protocol\n");
    printf("Usage:\n");
    printf("\t--dir,-d\ta directory where recieved files are put "
          "('uploads/' by default)\n");
    printf("\t--port,-p\tserver port\n");
    printf("\t--log-level,-l\tset log level (0 - highest, 5 - lowest)\n");
    printf("\t--help,-h\n");
}

int
main(int argc, char** argv)
{
    log_debug("Process PID: %i", getpid());

    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {"dir", required_argument, NULL, 'd'},
        {"log-level", required_argument, NULL, 'l'},
        {NULL, 0, NULL, 0}
    };

    int c;
    int option_index;

    while ((c = getopt_long(argc, argv, "p:d:h:l",
                            long_options, &option_index)) != -1) {
        switch (c) {
            case 'p':
                port = strdup(optarg);
                break;
            case 'd':
                storage_dir = strdup(optarg);
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

    int sockfd = -1;

    ActiveClients_t clients;
    clients_list_init(&clients);

    if (create_listening_socket(port, &sockfd) != OK)
        goto error;

    if (setup_signal_handlers() != OK)
        goto error;

    log_info("Waiting for client connections on port %s...", port);

    if (run_accept_clients_loop(sockfd, &clients) != OK)
        goto error;

    log_info("Shutting down the server...");

    if (shutdown_active_clients(&clients) != OK)
        goto error;

    close(sockfd);
    return EXIT_SUCCESS;

error:
    if (sockfd >= 0)
        close(sockfd);
    return EXIT_FAILURE;
}

static void
sigint_handler(int signo)
{
    if (signo == SIGINT)
        is_interrupted = true;
}

static Result_t
setup_signal_handlers(void)
{
    struct sigaction sigint_sa = { 0 };
    sigint_sa.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &sigint_sa, NULL) != 0) {
        log_error("Failed to setup SIGINT handler: %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
create_listening_socket(char *port, int *sockfd)
{
    log_debug("Creating listening socket on port %s...", port);

    struct addrinfo hints = { 0 };
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_protocol = 0;           /* Any protocol */
    hints.ai_flags = AI_PASSIVE;     /* For accepting connections */

    struct addrinfo *found_addrinfo;
    int err;
    if ((err = getaddrinfo(NULL, port, &hints, &found_addrinfo)) != 0) {
        log_error("getaddrinfo() failed: %s", gai_strerror(err));
        *sockfd = -1;
        return ERROR;
    }

    struct addrinfo *ai;
    for (ai = found_addrinfo; ai != NULL; ai = ai->ai_next) {
        *sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (*sockfd == -1)
            continue;

        if (bind(*sockfd, ai->ai_addr, ai->ai_addrlen) != 0)
            continue;

        int backlog_size = 128;
        if (listen(*sockfd, backlog_size) == 0)
            break; /* Success */

        close(*sockfd);
    }

    freeaddrinfo(found_addrinfo);
    
    if (ai == NULL) {
        log_error("Could not create listening socket: %s", strerror(errno));
        *sockfd = -1;
        return ERROR;
    }

    return OK;
}

static Result_t
run_accept_clients_loop(int sockfd, ActiveClients_t *clients)
{
    while (!is_interrupted) {
        int client_sockfd;
        struct sockaddr_storage client_sa;
        socklen_t client_sa_len;

        client_sockfd = accept(sockfd, (struct sockaddr *) &client_sa,
                               &client_sa_len);

        if (client_sockfd == -1 && errno == EINTR)
            break;

        if (client_sockfd == -1) {
            log_error("Failed to accept client connection: %s",
                      strerror(errno));
            return ERROR;
        }

        char client_descr[SOCKADDR2STR_MAX_BUFSIZE];
        sockaddr2str((struct sockaddr *) &client_sa, client_sa_len, client_descr);

        Result_t status = create_client_thread(clients, client_sockfd, client_descr);
        if (status == CLIENTS_LIMIT_REACHED) {
            log_error("Couldn't accept connection from %s: "
                      "clients limit reached", client_descr);
            close(sockfd);
            continue;
        }
        else if (status != OK) {
            close(client_sockfd);
            return ERROR;
        }

        log_info("%s: New client connected.", client_descr);
    }

    return OK;
}
