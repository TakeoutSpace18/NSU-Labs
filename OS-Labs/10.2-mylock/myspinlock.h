#ifndef MYSPINLOCK_H
#define MYSPINLOCK_H

#include <stdatomic.h>

#define MYSPINLOCK_OK 0
#define MYSPINLOCK_ERROR -1

typedef atomic_int myspinlock_t;

int myspinlock_init(myspinlock_t *myspinlock);

void myspinlock_lock(myspinlock_t *myspinlock);
void myspinlock_unlock(myspinlock_t *myspinlock);

#endif /* MYSPINLOCK_H */
