#include <stdio.h>
#include <stdlib.h>

#include "storage.h"
#include "lock.h"

static void generate_random_string(char *str, size_t max_size)
{
    size_t len = rand() % max_size;
    for (size_t i = 0; i < len; ++i) {
        str[i] = 'a' + rand() % ('z' - 'a' + 1);
    }
    str[len] = '\0';
}

storage_t generate_storage(const size_t size)
{
    storage_t storage = { 0 };
    storage.lock = create_lock();

    for (size_t i = 0; i < size; ++i) {
        node_t *new = calloc(1, sizeof(*new));
        if (!new) {
            perror("malloc()");
            abort();
        }

        new->lock = create_lock();
        new->next = storage.first;
        generate_random_string(new->value, MAX_STRING_SIZE);

        storage.first = new;
    }

    return storage;
}
