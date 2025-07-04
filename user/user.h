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
int read(int, void*, int);
int chdir(const char*);

// printf
void printf(const char *, ...);

// umalloc
void *malloc(uint);
void free(void *);
// ulib.c
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));
void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
#endif
