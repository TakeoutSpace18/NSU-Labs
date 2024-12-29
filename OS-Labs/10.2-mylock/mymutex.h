#ifndef MYMUTEX_H
#define MYMUTEX_H

#include <stdatomic.h>
#define MYMUTEX_OK 0
#define MYMUTEX_ERROR -1

typedef atomic_int mymutex_t;

int mymutex_init(mymutex_t *mymutex);

void mymutex_lock(mymutex_t *mymutex);
void mymutex_unlock(mymutex_t *mymutex);

#endif /* MYMUTEX_H */
