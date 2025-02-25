#ifndef LOCK_H
#define LOCK_H

void *create_lock(void);

void read_lock(void *lock);
void write_lock(void *lock);

void unlock(void *lock);

#endif // !LOCK_H
