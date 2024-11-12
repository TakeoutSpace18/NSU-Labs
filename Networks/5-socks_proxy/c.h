#ifndef C_H
#define C_H

#define _GNU_SOURCE

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>

#include <assert.h>
#include <string.h>

#include <errno.h>
#include <signal.h>

#include <unistd.h>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

/* only GCC supports the unused attribute */
#ifdef __GNUC__
#define attribute_unused() __attribute__((unused))
#else
#define attribute_unused()
#endif

/* GCC support aligned, packed and noreturn */
#ifdef __GNUC__
#define attribute_aligned(a) __attribute__((aligned(a)))
#define attribute_noreturn() __attribute__((noreturn))
#define attribute_packed() __attribute__((packed))
#endif

#if defined __has_builtin
#if __has_builtin(__builtin_unreachable) /* unreachable code */
#define unreachable() __builtin_unreachable()
#endif
#endif

#ifndef unreachable
#define unreachable() abort()
#endif

#define container_of(N, T, F) ((T *)((char *)(N) - __builtin_offsetof(T, F)))

#endif /* C_H */
