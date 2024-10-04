#include "client_thread.h"

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"
#include "protocol.h"

static void *client_thread_main(void *arg);

void
clients_list_init(ActiveClients_t *clients)
{
    pthread_mutex_init(&clients->mutex, NULL);
    clients->count = 0;
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
            (clients->count - pos - 1) * sizeof(ClientData_t));

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

static void*
client_thread_main(void *arg)
{
    void *ret = EXIT_SUCCESS;
    Result_t status;
    ClientData_t *self = (ClientData_t *) arg;

    BeginFileTransferDTO_t begin_dto;
    status = receive_buffer(self->sockfd, &begin_dto, sizeof(begin_dto));
    if (status != OK || begin_dto.magic != BEGIN_FILE_TRANSFER_MAGIC) {
        log_error("%s: Failed to recieve BeginFileTransferDTO, %s",
                  self->description, strerror(errno));
        ret = (void *) EXIT_FAILURE;
        goto finish;
    }

    log_debug("%s: BeginFileTransferDTO received\n"
              "\tfilename_size: %zi\n\tfile_size: %zi",
              self->description, begin_dto.filename_size, begin_dto.file_size);

    char *filename = (char *) malloc(begin_dto.filename_size);

    status = receive_buffer(self->sockfd, filename, begin_dto.filename_size);
    if (status != OK) {
        log_error("%s: Failed to recieve file name, %s",
                  self->description, strerror(errno));
        ret = (void *) EXIT_FAILURE;
        goto finish;
    }

    for (size_t recv_size = 0; recv_size < begin_dto.file_size; ) {
        
    }

finish:
    log_info("%s: Closing connection...", self->description);
    remove_from_clients_list(self->active_clients_p, self);
    free(self);
    
    return ret;
}

static void
recieve_file(void)
{

}

Result_t
shutdown_active_clients(void)
{
    return OK;
}
