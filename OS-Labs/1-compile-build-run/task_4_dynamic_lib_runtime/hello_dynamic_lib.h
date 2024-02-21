#ifndef HELLO_DYNAMIC_LIB_H
#define HELLO_DYNAMIC_LIB_H

void print_hello(void);
typedef void (*print_hello_fn)(void);

#endif