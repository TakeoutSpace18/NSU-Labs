#include "myspinlock.h"

#include <stdatomic.h>
#include <unistd.h>

#define LOCKED 1
#define UNLOCKED 0

int myspinlock_init(myspinlock_t *myspinlock)
{
    *myspinlock = UNLOCKED;

    return MYSPINLOCK_OK;
}

void myspinlock_lock(myspinlock_t *myspinlock)
{
    for (;;) {
        int unlocked = UNLOCKED;
        if (atomic_compare_exchange_weak(myspinlock, &unlocked, LOCKED)) {
            break;
        }
    }
}

void myspinlock_unlock(myspinlock_t *myspinlock)
{
    *myspinlock = UNLOCKED;
}


