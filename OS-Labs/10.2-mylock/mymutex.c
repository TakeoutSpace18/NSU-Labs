#include "mymutex.h"

#include <stdatomic.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>

#define LOCKED 1
#define UNLOCKED 0

static int
futex(int* uaddr, int futex_op, int val, const struct timespec* timeout,
      int* uaddr2, int val3)
{
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

int mymutex_init(mymutex_t *mymutex)
{
    *mymutex = UNLOCKED;

    return MYMUTEX_OK;
}

void mymutex_lock(mymutex_t *mymutex)
{
    for (;;) {
        int unlocked = UNLOCKED;
        if (atomic_compare_exchange_weak(mymutex, &unlocked, LOCKED)) {
            break;
        }

        int err = futex((int *) mymutex, FUTEX_WAIT, LOCKED, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN) {
            perror("futex");
            abort();
        }
    }
}

void mymutex_unlock(mymutex_t *mymutex)
{
    *mymutex = UNLOCKED;
    int err = futex((int *) mymutex, FUTEX_WAKE, 1, NULL, NULL, 0);
    if (err == -1) {
        perror("futex");
        abort();
    }
}


