#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_PORT 8686

#define IM_ALIVE_MSG 0xA11A
#define UPDATE_TRIGGER_MSG 0xB22B

typedef enum {
    OK,
    ERROR
} Result_t;

#define ERR_DESCR_BUFSIZE 512
static char err_descr[ERR_DESCR_BUFSIZE];

static void
print_usage()
{
    printf("self_discovery <IPv4|IPv6 multicast group address> [port]\n");
}

static Result_t
bind_ipv4(int sfd, char *host, char *port)
{
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    if (port)
        addr.sin_port = htons(atoi(port));
    else
        addr.sin_port = htons(DEFAULT_PORT);

    if (inet_pton(AF_INET, host, &addr.sin_addr) == 0) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "multicast address is invalid");
        return ERROR;
    }

    int err = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
    if (err == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "bind(): %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
bind_ipv6(int sfd, char *host, char *port)
{
    struct sockaddr_in6 addr = { 0 };
    addr.sin6_family = AF_INET6;
    if (port)
        addr.sin6_port = htons(atoi(port));
    else
        addr.sin6_port = htons(DEFAULT_PORT);

    if (inet_pton(AF_INET6, host, &addr.sin6_addr) == 0) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "multicast address is invalid");
        return ERROR;
    }

    int err = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
    if (err == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "bind(): %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
broadcast_message(int sockfd, int msg)
{
    ssize_t sent = 0;
    while (sent < sizeof(msg)) {
        ssize_t ret = write(sockfd, (char *) &msg + sent, sizeof(msg) - sent);
        if (ret == -1) {
            snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                     "write(): %s", strerror(errno));
            return ERROR;
        }
    }

    return OK;
}

static Result_t
recieve_message(int sockfd, int *msg,
                struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t recieved = 0;
    while (recieved < sizeof(int)) {
        ssize_t ret = recvfrom(sockfd, (char *) msg + recieved,
                               sizeof(int) - recieved, 0, src_addr, addrlen);
        if (ret == -1) {
            snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                     "recvfrom(): %s", strerror(errno));
            return ERROR;
        }
    }

    return OK;
}

int
main(int argc, char **argv)
{
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "wrong arguments count!\n");
        print_usage();
        return EXIT_FAILURE;
    }

    char *multicast_addr = argv[1];

    char *port;
    if (argc == 3)
        port = argv[2];
    else
        port = NULL;

    int ipmode;
    if (strchr(multicast_addr, '.'))
        ipmode = AF_INET;
    else
        ipmode = AF_INET6;

    int sfd;
    sfd = socket(ipmode, SOCK_DGRAM, 0);

    if (sfd < 0) {
        perror("socket()");
        return EXIT_FAILURE;
    } 

    Result_t ret;
    switch (ipmode) {
        case AF_INET:
            ret = bind_ipv4(sfd, multicast_addr, port);
            break;
        case AF_INET6:
            ret = bind_ipv6(sfd, multicast_addr, port);
            break;
    }

    if (ret != OK) {
        fputs(err_descr, stderr);
        return EXIT_FAILURE;
    }

    /* Notify other instances of the app of your appearance 
     * and ask them to broadcast their state */
    ret = broadcast_message(sfd, UPDATE_TRIGGER_MSG);
    if (ret != OK) {
        fputs(err_descr, stderr);
        return EXIT_FAILURE;
    }

    for (;;) {
        int msg;
        struct sockaddr *src = malloc(ipmode == AF_INET ?
                                      sizeof(struct sockaddr_in) :
                                      sizeof(struct sockaddr_in6));

        if (!src) {
            fputs("out of memory\n", stderr);
            abort();
        }

        socklen_t srclen;
        ret = recieve_message(sfd, &msg, src, &srclen);
        if (ret != OK) {
            fputs(err_descr, stderr);
            return EXIT_FAILURE;
        }

        if (msg == UPDATE_TRIGGER_MSG) {
            ret = broadcast_message(sfd, IM_ALIVE_MSG);
            if (ret != OK) {
                fputs(err_descr, stderr);
                return EXIT_FAILURE;
            }
        }

        if (msg == IM_ALIVE_MSG) {
            char addrbuf[64];
            if (!inet_ntop(ipmode, src, addrbuf, sizeof(addrbuf))) {
                perror("inet_ntop()");
                return EXIT_FAILURE;
            }

            printf("%s is alive\n", addrbuf);
        }
    }
}
