#define _GNU_SOURCE

#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>

#include "error.h"
#include "common.h"
#include "log.h"
#include "progressbar.h"

#define STATUS_UPDATE_INTERVAL_USEC 500000

static char *host = NULL;
static char *port = NULL;
static char *filepath = NULL;

static bool needs_status_update = false;

static Result_t connect_to_server(char *host, char *port, int *sockfd);
static size_t get_file_size(FILE *file);
static Result_t send_file(int sockfd, char *filepath);
static void update_status(progressbar *pbar, struct timeval *last_update,
                          size_t sent_size);


static void
do_help(void)
{
    printf("Send file to receiving server");
    printf("Short form: send-file <filepath> <hostname:port>");
    printf("Usage:\n");
    printf("\t--file,-f\tpath to file to send\n");
    printf("\t--host,-D\tdestination hostname\n");
    printf("\t--port,-D\tdestination port\n");
    printf("\t--help,-h\n");
}

int
main(int argc, char** argv) {

    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"file", required_argument, NULL, 'f'},
        {"host", required_argument, NULL, 'H'},
        {"port", required_argument, NULL, 'P'},
        {NULL, 0, NULL, 0}
    };

    int c;
    int option_index;

    while ((c = getopt_long(argc, argv, "f:H:P:h",
                            long_options, &option_index)) != -1) {
        switch (c) {
            case 'f':
                filepath = strdup(optarg);
                break;
            case 'H':
                host = strdup(optarg);
                break;
            case 'P':
                port = strdup(optarg);
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

    if (connect_to_server(host, port, &sockfd) != OK)
        goto error;

    if (send_file(sockfd, filepath) != OK)
        goto error;

    close(sockfd);
    return EXIT_SUCCESS;

error:
    if (sockfd >= 0)
        close(sockfd);
    return EXIT_FAILURE;
}

static Result_t
connect_to_server(char *host, char *port, int *sockfd)
{
    log_debug("Connecting to %s:%s...", host, port);

    struct addrinfo hints = { 0 };
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_protocol = 0;           /* Any protocol */

    struct addrinfo *found_addrinfo;
    int err;
    if ((err = getaddrinfo(host, port, &hints, &found_addrinfo)) != 0) {
        log_error("getaddrinfo() failed: %s", gai_strerror(err));
        *sockfd = -1;
        return ERROR;
    }

    struct addrinfo *ai;
    for (ai = found_addrinfo; ai != NULL; ai = ai->ai_next) {
        *sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (*sockfd == -1)
            continue;

        if (connect(*sockfd, ai->ai_addr, ai->ai_addrlen) == 0)
            break; /* Success */

        close(*sockfd);
    }

    freeaddrinfo(found_addrinfo);
    
    if (ai == NULL) {
        log_error("Could not connect to %s:%s, ", host, port, strerror(errno));
        *sockfd = -1;
        return ERROR;
    }

    return OK;
}

static size_t
get_file_size(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    rewind(file);
    return sz;
}

static void
sigalarm_handler(int signo)
{
    if (signo == SIGALRM)
        needs_status_update = true;
}

static Result_t
setup_status_print_timer(void)
{
    struct timeval interval = {
        .tv_sec = 0,
        .tv_usec = STATUS_UPDATE_INTERVAL_USEC
    };

    struct itimerval new = {
        .it_value = interval,
        .it_interval = interval
    };

    if (setitimer(ITIMER_REAL, &new, NULL) != 0) {
        log_error("Failed to setup itimer: %s", strerror(errno));
        return ERROR;
    }

    struct sigaction sa = { 0 };
    sa.sa_handler = sigalarm_handler;

    if (sigaction(SIGALRM, &sa, NULL) != 0) {
        log_error("Failed to setup itimer: %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

static Result_t
send_file(int sockfd, char *filepath)
{
    Result_t status;

    log_debug("Sending file %s...", filepath);

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        log_error("Failed to open file %s", filepath);
        return ERROR;
    }

    const char *filename = basename(filepath);
    size_t      filesize = get_file_size(file);

    disable_send_timeout(sockfd);

    BeginFileTransferDTO_t begin_dto;
    begin_dto.magic = BEGIN_FILE_TRANSFER_MAGIC;
    begin_dto.filename_size = strlen(filename) + 1;
    begin_dto.file_size = filesize;

    status = send_buffer(sockfd, &begin_dto, sizeof(begin_dto));
    if (status != OK) {
        log_error("Failed to send BeginFileTransferDTO: %s", strerror(errno));
        return ERROR;
    }

    log_debug("BeginFileTransferDTO_t sent");

    status = send_buffer(sockfd, filename, strlen(filename) + 1);
    if (status != OK) {
        log_error("Failed to send file name: %s", strerror(errno));
        return ERROR;
    }

    log_debug("File name sent");

    progressbar *pbar = progressbar_new(filename, filesize);

    char file_chunk[FILE_CHUNK_SIZE];

    size_t total_sent_size = 0;
    while (total_sent_size < begin_dto.file_size) {
        size_t read_chunk_sz = fread(file_chunk, sizeof(char), FILE_CHUNK_SIZE, file);

        for (size_t sent = 0; sent < read_chunk_sz; ) {
            ssize_t ret = send(sockfd, file_chunk + sent, read_chunk_sz - sent, 0);
            if (ret == -1 && errno == EINTR)
                goto cancel_send;

            if (ret == -1)
                goto error;

            sent += ret;

            if (needs_status_update)
                progressbar_update(pbar, total_sent_size);

        }

        total_sent_size += read_chunk_sz;
    }

    FileAcceptedDTO_t file_accepted;
    status = receive_buffer(sockfd, &file_accepted, sizeof(file_accepted));
    if (status != OK || file_accepted.magic != FILE_ACCEPTED_MAGIC) {
        log_error("Did not receive file accept confirmation from server");
        goto error;
    }


    progressbar_finish(pbar);
    log_info("File was sent successfully!");
    return OK;

error:
cancel_send:

    progressbar_finish(pbar);
    return ERROR;
}

