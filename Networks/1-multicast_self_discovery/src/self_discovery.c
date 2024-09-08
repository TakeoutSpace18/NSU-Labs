#include <asm-generic/socket.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

const char *default_group_ip = "224.0.0.1";
const char *default_port = "8686";

#define IM_ALIVE_MSG 0xA11A
#define UPDATE_TRIGGER_MSG 0xB22B

typedef enum { OK, ERROR } Result_t;

#define ERR_DESCR_BUFSIZE 512
static char err_descr[ERR_DESCR_BUFSIZE];

static void
print_usage() {
    printf("self_discovery <IPv4|IPv6 multicast group address> [port]\n");
}

static Result_t
socket_config(int sfd)
{
    int reuse = 1;
    int err = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
                         (char *) &reuse, sizeof(reuse));
    if (err == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "setsockopt(): %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
bind_multicast_group(const char *ip, const char *port, int *sfd,
                     struct sockaddr_storage *bound_addr,
                     socklen_t *bound_addr_len)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */

    struct addrinfo* result;

    int err = getaddrinfo(ip, port, &hints, &result);
    if (err != 0) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "getaddrinfo(): %s", gai_strerror(err));
        return ERROR;
    }

    struct addrinfo *rp; 
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        *sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (*sfd == -1)
            continue;

        Result_t ret = socket_config(*sfd);
        if (ret != OK) {
            close(*sfd);
            continue;
        }

        if (bind(*sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            memcpy(bound_addr, rp->ai_addr, rp->ai_addrlen);
            *bound_addr_len = rp->ai_addrlen;
            break; /* Success */
        }

        close(*sfd);
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "Could not bind: %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
send_message(int sockfd, int msg,
             struct sockaddr *dest_addr, socklen_t dest_addr_len) {
    ssize_t sent = 0;
    while (sent < sizeof(msg)) {
        ssize_t ret = sendto(sockfd, (char*)&msg + sent, sizeof(msg) - sent,
                             0, dest_addr, dest_addr_len);
        if (ret == -1) {
            snprintf(err_descr, ERR_DESCR_BUFSIZE, "sendto(): %s",
                     strerror(errno));
            return ERROR;
        }

        sent += ret;
    }

    return OK;
}

static Result_t
recieve_message(int sockfd, int* msg, struct sockaddr* src_addr,
                                socklen_t* addrlen) {
    ssize_t recieved = 0;
    while (recieved < sizeof(int)) {
        ssize_t ret = recvfrom(sockfd, (char*)msg + recieved,
                               sizeof(int) - recieved, 0, src_addr, addrlen);
        if (ret == -1) {
            snprintf(err_descr, ERR_DESCR_BUFSIZE, "recvfrom(): %s",
                     strerror(errno));
            return ERROR;
        }

        recieved += ret;
    }

    return OK;
}

static Result_t
print_app_instance_info(struct sockaddr *addr, socklen_t addrlen)
{
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int ret = getnameinfo(addr, addrlen, host, NI_MAXHOST,
                          service, NI_MAXSERV, 0);

    if (ret != 0) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "getnameinfo(): %s", gai_strerror(ret));
        return ERROR;
    }

    printf("%s, %s is alive\n", host, service);

    return OK;
}

int main(int argc, char** argv) {
    if (argc > 3) {
        fprintf(stderr, "wrong arguments count!\n");
        print_usage();
        return EXIT_FAILURE;
    }

    const char* group_ip;
    const char* port;

    if (argc == 3) {
        group_ip = argv[1];
        port = argv[2];
    } else if (argc == 2) {
        group_ip = argv[1];
        port = default_port;
    } else {
        group_ip = default_group_ip;
        port = default_port;
    }

    int                     sfd;
    struct sockaddr_storage groupaddr;
    socklen_t               groupaddr_len;

    Result_t ret = bind_multicast_group(group_ip, port, &sfd,
                                        &groupaddr, &groupaddr_len);
    if (ret != OK) {
        fputs(err_descr, stderr);
        return EXIT_FAILURE;
    }
 
    /* Notify other instances of the app of your appearance 
     * and ask them to broadcast their state */
    ret = send_message(sfd, UPDATE_TRIGGER_MSG,
                       (struct sockaddr *) &groupaddr, groupaddr_len);
    if (ret != OK) {
        fputs(err_descr, stderr);
        return EXIT_FAILURE;
    }

    for (;;) {
        int                     msg;
        struct sockaddr_storage src;
        socklen_t               srclen;

        ret = recieve_message(sfd, &msg, (struct sockaddr *) &src, &srclen);
        if (ret != OK) {
            fputs(err_descr, stderr);
            return EXIT_FAILURE;
        }

        if (msg == UPDATE_TRIGGER_MSG) {
            ret = send_message(sfd, IM_ALIVE_MSG,
                               (struct sockaddr *) &groupaddr, groupaddr_len);
            if (ret != OK) {
                fputs(err_descr, stderr);
                return EXIT_FAILURE;
            }
        }

        if (msg == IM_ALIVE_MSG) {
            Result_t ret = print_app_instance_info((struct sockaddr *) &src,
                                                   srclen);
            if (ret != OK) {
                fputs(err_descr, stderr);
            }
        }
    }
}
