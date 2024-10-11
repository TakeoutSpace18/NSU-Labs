#include "client_thread.h"

#include <bits/types/struct_timeval.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "error.h"
#include "log.h"
#include "common.h"
#include "timer.h"

static void *client_thread_main(void *arg);

void
clients_list_init(ActiveClients_t *clients)
{
    pthread_mutex_init(&clients->mutex, NULL);
    pthread_mutex_lock(&clients->mutex);
    clients->count = 0;
    pthread_mutex_unlock(&clients->mutex);
}

static void
add_to_clients_list(ActiveClients_t *clients, ClientData_t *client)
{
    pthread_mutex_lock(&clients->mutex);

    clients->list[clients->count] = client;
    clients->count++;

    pthread_mutex_unlock(&clients->mutex);
}

static void
remove_from_clients_list(ActiveClients_t *clients, ClientData_t *client)
{
    pthread_mutex_lock(&clients->mutex);
    size_t pos = 0;
    while (clients->list[pos] != client)
        pos++;
    memmove(clients->list + pos, clients->list + pos + 1,
            (clients->count - pos - 1) * sizeof(ClientData_t *));
    clients->count--;

    pthread_mutex_unlock(&clients->mutex);
}

Result_t
create_client_thread(ActiveClients_t *clients, int sockfd,
                     const char *description)
{
    if (clients->count == MAX_ALLOWED_CLIENTS)
        return CLIENTS_LIMIT_REACHED;
    ClientData_t *new_client = (ClientData_t *) malloc(sizeof(ClientData_t));

    new_client->sockfd = sockfd;
    strcpy(new_client->description, description);
    new_client->active_clients_p = clients;

    if (pthread_create(&new_client->thread, NULL,
                       client_thread_main, new_client) != 0) {
        log_error("pthread_create(): %s", strerror(errno));
        return ERROR;
    }

    add_to_clients_list(clients, new_client);
    return OK;
}

static Result_t
receive_begin_dto(const ClientData_t *self, BeginFileTransferDTO_t *dto)
{
    Result_t status = receive_buffer(self->sockfd, dto, sizeof(*dto));

    if (status != OK || dto->magic != BEGIN_FILE_TRANSFER_MAGIC) {
        log_error("%s: Failed to recieve BeginFileTransferDTO, %s",
                  self->description, strerror(errno));
        return ERROR;
    }

    log_debug("%s: BeginFileTransferDTO received\n"
              "\tfilename_size: %zi\n\tfile_size: %zi",
              self->description, dto->filename_size, dto->file_size);
    return OK;
}

static Result_t
receive_filename(const ClientData_t *self, size_t size,
                 char *filename)
{
    Result_t status = receive_buffer(self->sockfd, filename, size);
    if (status != OK) {
        log_error("%s: Failed to recieve file name, %s",
                  self->description, strerror(errno));
        return ERROR;
    }
    
    log_debug("%s: File name received: %s", self->description, filename);

    return OK;
}

static Result_t
receive_file(const ClientData_t *self, size_t size, FILE *f)
{
    struct timespec last_report, last_spd_update, rcv_begin;
    get_mono_time(&rcv_begin);
    memcpy(&last_report, &rcv_begin, sizeof(struct timespec));
    memcpy(&last_spd_update, &rcv_begin, sizeof(struct timespec));

    struct timeval timeout = { .tv_sec = 0, .tv_usec = 100000 };
    set_receive_timeout(self->sockfd, timeout);

    size_t recvd_since_spd_update = 0;
    double instant_spd = 0.0;
    double avg_spd = 0.0;

    char file_chunk[FILE_CHUNK_SIZE];
    for (size_t rcvd_total = 0; rcvd_total < size; ) {

        size_t left_sz = size - rcvd_total;
        size_t chunk_sz = left_sz < FILE_CHUNK_SIZE ? left_sz : FILE_CHUNK_SIZE;
        
        for (size_t rcvd_in_chunk = 0; rcvd_in_chunk < chunk_sz; ) {

            char *buf = file_chunk + rcvd_in_chunk;
            size_t len = chunk_sz - rcvd_in_chunk;
            ssize_t ret = recv(self->sockfd, buf, len, 0);

            if (ret == -1) {
                if (errno == EINTR)
                    return INTERRUPTED;

                if (errno != EAGAIN)
                    return ERROR;

            } else {
                rcvd_in_chunk += ret;
                rcvd_total += ret;
                recvd_since_spd_update += ret;
            }

            struct timespec now;
            get_mono_time(&now);

            int64_t msecs_since_last_spd_update = msecs_elapsed(&last_spd_update, &now);
            if (msecs_since_last_spd_update >= SPD_UPDATE_MSECS) {
                if (recvd_since_spd_update == 0) {
                    log_error("%s: Client sent nothing for too long",
                              self->description);
                    return INTERRUPTED;
                }

                instant_spd = recvd_since_spd_update * 1000.0 / msecs_since_last_spd_update;
                recvd_since_spd_update = 0;

                memcpy(&last_spd_update, &now, sizeof(struct timespec));
            }

            if (msecs_elapsed(&last_report, &now) >= REPORT_INTERVAL_MSECS) {
                avg_spd = rcvd_total * 1000.0 / msecs_elapsed(&rcv_begin, &now);
                int status = rcvd_total * 100 / size;

                log_info("%s: Receiving progress - %i%\n"
                         "\tInstant speed: %.1lf Mb/s\n"
                         "\tAverage speed: %.1lf Mb/s",
                         self->description, status,
                         instant_spd / (1024 * 1024),
                         avg_spd / (1024 * 1024));

                memcpy(&last_report, &now, sizeof(struct timespec));
            }
        }

        if (fwrite(file_chunk, chunk_sz, 1, f) != 1) {
            log_error("%s: Falied to write file chunk, %s", strerror(errno));
            return ERROR;
        }

    }

    log_info("Receiving finished");

    return OK;
}

static Result_t
send_accepted_dto(const ClientData_t *self)
{
    FileAcceptedDTO_t dto;
    dto.magic = FILE_ACCEPTED_MAGIC;

    if (send_buffer(self->sockfd, &dto, sizeof(dto)) != OK) {
        log_error("%s: Failed to send FileAcceptedDTO, %s",
                  self->description, strerror(errno));
        return ERROR;
    }

    log_debug("%s: sent FileAcceptedDTO", self->description);
    return OK;
}

static Result_t
create_file_in_storage(const ClientData_t *self, const char *filename, FileData_t *file)
{
    file->pathname = malloc(strlen(storage_dir) + strlen(filename) + 2);
    if (!file->pathname) {
        log_error("%s: out of memory", self->description);
        return ERROR;
    }

    strcpy(file->pathname, storage_dir);
    strcat(file->pathname, "/");
    strcat(file->pathname, filename);

    file->f = fopen(file->pathname, "w+b");
    if (file->f == NULL) {
        log_error("%s: Failed to create file %s, %s",
                  self->description, file->pathname, strerror(errno));
        free(file->pathname);
        memset(file, 0, sizeof(*file));
        return ERROR;
    }

    return OK;
}

static void
close_file(FileData_t *file)
{
    free(file->pathname);
    if (file->f)
        fclose(file->f);

    memset(file, 0, sizeof(*file));
}

static void
remove_file_from_storage(FileData_t *file)
{
    fclose(file->f);
    unlink(file->pathname);
    free(file->pathname);

    memset(file, 0, sizeof(*file));
}

static void*
client_thread_main(void *arg)
{
    void *ret = EXIT_SUCCESS;
    ClientData_t *self = (ClientData_t *) arg;

    BeginFileTransferDTO_t begin_dto;
    if (receive_begin_dto(self, &begin_dto) != OK)
        goto error;


    char *filename = (char *) malloc(begin_dto.filename_size);
    if (!filename) {
        log_error("%s: out of memory", self->description);
        goto error;
    }

    if (receive_filename(self, begin_dto.filename_size, filename) != OK)
        goto error;

    FileData_t file = { 0 };
    if (create_file_in_storage(self, filename, &file) != OK)
        goto error;

    Result_t status = receive_file(self, begin_dto.file_size, file.f);
    if (status == INTERRUPTED) {
        log_info("%s: File %s receiving cancelled", self->description, filename);
        remove_file_from_storage(&file);
        goto finish;
    }
    if (status != OK)
        goto error;

    if (send_accepted_dto(self) != OK)
        goto error;

finish:
    /* This cleanup should be done in pthread_cleanup_push() in case of cancel,
     * but i am too lazy to implement it :( */
    log_info("%s: Closing connection...", self->description);
    close_file(&file);
    close(self->sockfd);
    remove_from_clients_list(self->active_clients_p, self);
    free(self);
    
    return ret;

error:
    ret = (void *) EXIT_FAILURE;
    goto finish;
}

Result_t
shutdown_active_clients(ActiveClients_t *clients)
{
    for (size_t i = 0; i < clients->count; ++i) {
        if (pthread_cancel(clients->list[i]->thread) == -1) {
            log_error("pthread_cancel() failed: %s", strerror(errno));
            return ERROR;
        }
    }
    return OK;
}
