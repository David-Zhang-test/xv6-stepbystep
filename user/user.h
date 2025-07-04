#ifndef __USER_H__
#define __USER_H__

#include "../kernel/types.h"

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int *);
int write(int, const void *, int);
int kill(int);
int getpid(void);
char *sbrk(int);
int sleep(int);
int uptime(void);
int exec(const char *, char **);

// printf
void printf(const char *, ...);

// umalloc
void *malloc(uint);
void free(void *);

#endif
