#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <sys/cdefs.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

const char *default_group_ip = "224.1.2.3"; /* use ff06::1 for IPv6 */
const char *default_port = "8686";

#define IM_ALIVE_MSG 0xA11A
#define STATUS_REQUEST_MSG 0xB22B

typedef enum {
    OK,
    ERROR,
    TIMED_OUT,
    INTERRUPTED
} Result_t;

#define ERR_DESCR_BUFSIZE 512
static char err_descr[ERR_DESCR_BUFSIZE];

static bool is_interrupted = false;

static void
print_usage() {
    printf("self_discovery <IPv4|IPv6 multicast group address> [port]\n");
}

static void
sigint_handler(int signo)
{
    is_interrupted = true;
}

static Result_t
setup_signal_handlers()
{
    struct sigaction sigint_sa;
    memset(&sigint_sa, 0, sizeof(sigint_sa));
    sigint_sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sigint_sa, NULL) != 0) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "sigaction(): %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
enable_reuse_addr(int sfd)
{
    int reuse = 1;
    int err = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
                         (char *) &reuse, sizeof(reuse));
    if (err == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "Failed to set SO_REUSEADDR, setsockopt(): %s",
                 strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t __attribute_maybe_unused__
disable_multicast_loop(int sfd, int af)
{
    int optlevel, option;

    if (af == AF_INET) {
        optlevel = IPPROTO_IP;
        option = IP_MULTICAST_LOOP;

    } else if (af == AF_INET6) {
        optlevel = IPPROTO_IPV6;
        option = IPV6_MULTICAST_LOOP;
    } else {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "Failed disable multicast loop, unknown protocol family");
        return ERROR;
    }

    bool val = false;
    int err = setsockopt(sfd, optlevel, option, &val, sizeof(val));
    if (err == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "Failed to add multicast membership, setsockopt(): %s",
                 strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
multicast_add_membership(int sfd, int af, struct sockaddr_storage *bound_addr)
{
    char *optval=NULL;
    int   optlevel, option, optlen;

    if (af == AF_INET) {
        struct sockaddr_in *addr = (struct sockaddr_in *) bound_addr;

        struct ip_mreq mreq;
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        mreq.imr_multiaddr = addr->sin_addr;

        optlevel = IPPROTO_IP;
        option = IP_ADD_MEMBERSHIP;
        optval = (char *) &mreq;
        optlen = sizeof(mreq);
    } else if (af == AF_INET6) {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *) bound_addr;

        struct ipv6_mreq mreq6;
        mreq6.ipv6mr_interface = htonl(INADDR_ANY);
        mreq6.ipv6mr_multiaddr = addr->sin6_addr;

        optlevel = IPPROTO_IPV6;
        option = IPV6_ADD_MEMBERSHIP;
        optval = (char *) &mreq6;
        optlen = sizeof(mreq6);
    } else {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "Failed to add multicast membership, unknown protocol family");
        return ERROR;
    }

    int err = setsockopt(sfd, optlevel, option, optval, optlen);
    if (err == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE, 
                 "Failed to add multicast membership, setsockopt(): %s",
                 strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
create_multicast_socket(const char *ip, const char *port, int *sfd,
                     struct sockaddr_storage *bound_addr,
                     socklen_t *bound_addr_len)
{
    Result_t ret;
    int address_family;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
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

        ret = enable_reuse_addr(*sfd);
        if (ret != OK)
            goto error;

        if (bind(*sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            memcpy(bound_addr, rp->ai_addr, rp->ai_addrlen);
            address_family = rp->ai_family;
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

    ret = multicast_add_membership(*sfd, address_family, bound_addr);
    if (ret != OK)
        goto error;

    return OK;

error:
    close(*sfd);
    return ERROR;
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
        if (ret == -1 && errno == EAGAIN)
            return TIMED_OUT;

        if (ret == -1 && errno == EINTR) {
            return INTERRUPTED;
        }

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
    char numeric_host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int ret = getnameinfo(addr, addrlen, numeric_host, NI_MAXHOST,
                          service, NI_MAXSERV,
                          NI_NUMERICSERV | NI_NUMERICHOST);
    if (ret == -1)
        goto getnameinfo_fail;

    ret = getnameinfo(addr, addrlen, host, NI_MAXHOST, NULL, 0, 0);
    if (ret == -1)
        goto getnameinfo_fail;


    printf("%s (%s), port %s is alive\n", host, numeric_host, service);
    return OK;

getnameinfo_fail:
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "getnameinfo(): %s", gai_strerror(ret));
        return ERROR;
}

static void
disable_rcv_timeout(int sfd) {
    struct timeval time = { 0 };
    setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
}

static Result_t
collect_app_instances_info(int sfd, struct timeval timeout) 
{
    int ret = setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO,
                         &timeout, sizeof(timeout));
    if (ret == -1) {
        snprintf(err_descr, ERR_DESCR_BUFSIZE,
                 "setsockopt(): %s", gai_strerror(ret));
        return ERROR;
    }

    char time_str[32];
    time_t now = time(NULL);
    strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", localtime(&now));

    printf("------------ ALIVE INSTANCES (%s) ------------\n", time_str);

    for (;;) {
        int                     msg;
        struct sockaddr_storage src;
        socklen_t               srclen;

        Result_t ret = recieve_message(sfd, &msg,
                                       (struct sockaddr *) &src, &srclen);
        if (ret == TIMED_OUT)
            break;

        if (ret == ERROR)
            goto error;

        if (msg == IM_ALIVE_MSG) {
            if (print_app_instance_info((struct sockaddr *) &src, srclen) != OK)
                goto error;
        }
    }

    disable_rcv_timeout(sfd);
    return OK;

error:
    disable_rcv_timeout(sfd);
    return ERROR;
}


int main(int argc, char** argv) {
    Result_t ret;

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

    ret = setup_signal_handlers();
    if (ret != OK)
        goto error;

    int                     sfd;
    struct sockaddr_storage groupaddr;
    socklen_t               groupaddr_len;

    ret = create_multicast_socket(group_ip, port, &sfd,
                               &groupaddr, &groupaddr_len);
    if (ret != OK)
        goto error;
 
    /* Ask all instances of the app to broadcast their state
     * (this instance itself gets this message too)*/
    ret = send_message(sfd, STATUS_REQUEST_MSG,
                       (struct sockaddr *) &groupaddr, groupaddr_len);
    if (ret != OK)
        goto error;

    while (!is_interrupted) {
        int                     msg;
        struct sockaddr_storage src;
        socklen_t               srclen;

        ret = recieve_message(sfd, &msg, (struct sockaddr *) &src, &srclen);
        if (ret == INTERRUPTED)
            break;

        if (ret != OK)
            goto error;

        if (msg == STATUS_REQUEST_MSG) {
            ret = send_message(sfd, IM_ALIVE_MSG,
                               (struct sockaddr *) &groupaddr, groupaddr_len);
            if (ret != OK)
                goto error;

            struct timeval collect_timeout;
            collect_timeout.tv_sec = 0;
            collect_timeout.tv_usec = 500000;
            ret = collect_app_instances_info(sfd, collect_timeout);

            if (ret != OK)
                fputs(err_descr, stderr);

        } 
    }

    printf("Finishing...");

    /* Notify other instances of your death */
    ret = send_message(sfd, STATUS_REQUEST_MSG,
                       (struct sockaddr *) &groupaddr, groupaddr_len);
    if (ret != OK)
        goto error;

    close(sfd);
    return EXIT_SUCCESS;

error:
    fputs(err_descr, stderr);
    close(sfd);
    return EXIT_FAILURE;
}
