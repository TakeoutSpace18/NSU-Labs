#include "client_thread.h"

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"

static void *client_thread_main(void *arg);

void clients_list_init(ActiveClients_t *clients)
{
    pthread_mutex_init(&clients->mutex, NULL);
}

Result_t
create_client_thread(ActiveClients_t *clients, int sockfd)
{
    if (clients->count == MAX_ALLOWED_CLIENTS)
        return CLIENTS_LIMIT_REACHED;

    ClientData_t *new_client = (ClientData_t *) malloc(sizeof(ClientData_t));
    new_client->sockfd = sockfd;

    if (pthread_create(&new_client->thread, NULL,
                       client_thread_main, new_client) != 0) {
        log_error("pthread_create(): %s", strerror(errno));
        return ERROR;
    }

    clients->count++;
    return OK;
}

static void*
client_thread_main(void *arg) {
    ClientData_t *self = (ClientData_t *) arg;

    for (;;) {
        const char *msg = "Hello!\n";
        int ret = send(self->sockfd, msg, strlen(msg), 0);
        if (ret == -1) {
            log_error("send() failed: %s", strerror(errno));
            break;
        }
        sleep(1);
    }

    return EXIT_SUCCESS;
}

Result_t
shutdown_active_clients(void)
{
    return OK;
}
