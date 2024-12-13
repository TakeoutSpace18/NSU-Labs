#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>

#include "storage.h"
#include "lock.h"

#define ASC_COUNTER_CPU 0
#define DESC_COUNTER_CPU 2
#define EQ_COUNTER_CPU 4
#define SWAPPER1_CPU 6
#define SWAPPER2_CPU 8
#define SWAPPER3_CPU 10

#define RUNTIME_SECONDS 6

struct stats {
    size_t asc_counter_iters;
    size_t desc_counter_iters;
    size_t eq_counter_iters;

    size_t asc_counter;
    size_t desc_counter;
    size_t eq_counter;

    size_t swaps1;
    size_t swaps2;
    size_t swaps3;
};

struct stats g_stats = { 0 };

typedef bool (*comparator_t)(node_t *lhs, node_t *rhs);

void print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf("  -l, --list-size SIZE   Set the size of the list (required)\n");
    printf("  -h, --help             Show this help message and exit\n");
}

unsigned short lfsr = 0xACE1u;
unsigned bit;

static unsigned fastrand(void)
{
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

static bool need_to_swap(void)
{
    return fastrand() % 2;
}

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

static bool nodes_asc(node_t *lhs, node_t *rhs)
{
    return strlen(lhs->value) < strlen(rhs->value);
}

static bool nodes_desc(node_t *lhs, node_t *rhs)
{
    return strlen(lhs->value) > strlen(rhs->value);
}

static bool nodes_eq(node_t *lhs, node_t *rhs)
{
    return strlen(lhs->value) == strlen(rhs->value);
}

static void counter(storage_t *storage, comparator_t cmp, size_t *nr_counts,
                    size_t *nr_iters)
{
    for (;;) {
        read_lock(storage->lock);
        node_t *cur = storage->first;
        if (cur == NULL) {
            unlock(storage->lock);
            continue;
        }

        rlock_node(cur);
        unlock(storage->lock);

        node_t *next = cur->next;
        while (next != NULL) {
            if (cmp(cur, cur->next)) {
                *nr_counts += 1;
            }

            rlock_node(next);
            unlock_node(cur);
            cur = next;
            next = cur->next;
        }
        unlock_node(cur);

        *nr_iters += 1;
    }
}

static void *asc_counter(void *arg)
{
    set_cpu(ASC_COUNTER_CPU);
    storage_t *storage = (storage_t *) arg;
    counter(storage, nodes_asc, &g_stats.asc_counter, &g_stats.asc_counter_iters);
    return NULL;
}

static void *desc_counter(void *arg)
{
    set_cpu(DESC_COUNTER_CPU);
    storage_t *storage = (storage_t *) arg;
    counter(storage, nodes_desc, &g_stats.desc_counter, &g_stats.desc_counter_iters);
    return NULL;
}

static void *eq_counter(void *arg)
{
    set_cpu(EQ_COUNTER_CPU);
    storage_t *storage = (storage_t *) arg;
    counter(storage, nodes_eq, &g_stats.eq_counter, &g_stats.eq_counter_iters);
    return NULL;
}

static void swapper(storage_t *storage, size_t *nr_swaps)
{
    for (;;) {
        node_t *cur, *next, *next_next;
        cur = next = next_next = NULL;

        write_lock(storage->lock);
        next = storage->first;

        if (next == NULL) {
            unlock(storage->lock);
            continue;
        }
        wlock_node(next);

        next_next = next->next;
        if (next_next == NULL) {
            unlock_node(next);
            unlock(storage->lock);
            continue;
        }
        wlock_node(next_next);

        if (need_to_swap()) {
            storage->first = next_next;
            next->next = next_next->next;
            next_next->next = next;

            *nr_swaps += 1;
        }

        unlock_node(next_next);
        unlock_node(next);
        unlock(storage->lock);

        cur = next;
        wlock_node(cur);

        for (;;) {
            next = cur->next;
            if (next == NULL) {
                unlock_node(cur);
                break;
            }
            wlock_node(next);

            next_next = next->next;
            if (next_next == NULL) {
                unlock_node(next);
                unlock_node(cur);
                break;
            }
            wlock_node(next_next);

            if (need_to_swap()) {
                cur->next = next_next;
                next->next = next_next->next;
                next_next->next = next;

                *nr_swaps += 1;
            }

            unlock_node(next_next);
            unlock_node(next);
            unlock_node(cur);

            cur = next;
            wlock_node(cur);
        }
    }
}

static void *swapper1(void *arg)
{
    set_cpu(SWAPPER1_CPU);
    storage_t *storage = (storage_t *) arg;
    swapper(storage, &g_stats.swaps1);
    return NULL;
}

static void *swapper2(void *arg)
{
    set_cpu(SWAPPER2_CPU);
    storage_t *storage = (storage_t *) arg;
    swapper(storage, &g_stats.swaps2);
    return NULL;
}
static void *swapper3(void *arg)
{
    set_cpu(SWAPPER3_CPU);
    storage_t *storage = (storage_t *) arg;
    swapper(storage, &g_stats.swaps3);
    return NULL;
}

static void create_thread(pthread_t *thread, void *(*routine)(void*), void *arg)
{
    if (pthread_create(thread, NULL, routine, arg) != 0) {
        perror("pthread_create()");
        abort();
    }
}

void print_stats(const struct stats *s) {
    printf("Stats:\n");
    printf("  Ascending Counter Iterations: %zu\n", s->asc_counter_iters);
    printf("  Descending Counter Iterations: %zu\n", s->desc_counter_iters);
    printf("  Equal Counter Iterations: %zu\n", s->eq_counter_iters);
    printf("  Ascending Counter: %zu\n", s->asc_counter);
    printf("  Descending Counter: %zu\n", s->desc_counter);
    printf("  Equal Counter: %zu\n", s->eq_counter);
    printf("  Swaps1: %zu\n", s->swaps1);
    printf("  Swaps2: %zu\n", s->swaps2);
    printf("  Swaps3: %zu\n", s->swaps3);
}

int main(int argc, char *argv[]) {
    int list_size = -1;
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"list-size", required_argument, 0, 'l'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    printf("PID: %i\n", getpid());

    while ((opt = getopt_long(argc, argv, "l:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'l':
                list_size = atoi(optarg);
                if (list_size <= 0) {
                    fprintf(stderr, "Error: list size must be a positive integer.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case '?':
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (list_size == -1) {
        fprintf(stderr, "Error: --list-size is required.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("List size: %d\n", list_size);

    storage_t storage = generate_storage(list_size);

    pthread_t asc_counter_thread, desc_counter_thread, eq_counter_thread;
    create_thread(&eq_counter_thread, eq_counter, &storage);
    create_thread(&asc_counter_thread, asc_counter, &storage);
    create_thread(&desc_counter_thread, desc_counter, &storage);


    pthread_t swapper1_thread, swapper2_thread, swapper3_thread;
    create_thread(&swapper1_thread, swapper1, &storage);
    create_thread(&swapper2_thread, swapper2, &storage);
    create_thread(&swapper3_thread, swapper3, &storage);

    sleep(RUNTIME_SECONDS);

    print_stats(&g_stats);

    return EXIT_SUCCESS;
}
