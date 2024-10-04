#include <cerrno>
#define _GNU_SOURCE

#include <stdbool.h>
#include <string.h>
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
#include <sys/socket.h>
#include <netdb.h>

#include "error.h"
#include "log.h"
#include "protocol.h"

char *host = NULL;
char *port = NULL;
char *filepath = NULL;

static Result_t connect_to_server(char *host, char *port, int *sockfd);
static size_t get_file_size(FILE *file);
static Result_t send_file(int sockfd, char *filepath);
static void update_status(struct timespec *last_update, const char *filename,
                          size_t filesize, size_t sent_size);

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

    disable_send_timeout(sockfd);

    BeginFileTransferDTO_t begin_dto;
    begin_dto.magic = BEGIN_FILE_TRANSFER_MAGIC;
    begin_dto.filename_size = strlen(filename) + 1;
    begin_dto.file_size = get_file_size(file);

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

    char file_chunk[FILE_CHUNK_SIZE];

    for (size_t total_sent_size = 0; total_sent_size < begin_dto.file_size; ) {
        size_t read_chunk_sz = fread(file_chunk, sizeof(char), FILE_CHUNK_SIZE, file);

        for (size_t sent = 0; sent < read_chunk_sz; ) {
            ssize_t ret = send(sockfd, file_chunk + sent, read_chunk_sz - sent, 0);
            if (ret == -1 && errno == EINTR)
                goto cancel_send;

            if (ret == -1)
                goto error;

            sent += ret;

            update_status()
        }

        total_sent_size += read_chunk_sz;
    }
    

    return OK;

error:
}

static void
update_status(struct timespec *last_update, const char *filename,
              size_t filesize, size_t sent_size)
{

}

