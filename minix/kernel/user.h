#pragma once

struct stat;
struct rtcdate;

#include "exo.h"
#include "exo_cpu.h"

// system calls
int fork(void);
[[noreturn]] int exit(void);
int wait(void);
int pipe(int *);
int write(int, const void *, size_t);
int read(int, void *, size_t);
int close(int);
int kill(int);
int exec(char *, char **);
int open(const char *, int);
int mknod(const char *, short, short);
int unlink(const char *);
int fstat(int fd, struct stat *);
int link(const char *, const char *);
int mkdir(const char *);
int chdir(const char *);
int dup(int);
int getpid(void);
char *sbrk(int);
int sleep(int);
int uptime(void);
int mappte(void *, void *, int);

int exo_pctr_transfer(int cap);

int set_timer_upcall(void (*handler)(void));
exo_cap exo_alloc_page(void);
int exo_unbind_page(exo_cap);
int exo_alloc_block(uint dev, exo_blockcap *cap);
int exo_bind_block(exo_blockcap *cap, void *data, int write);
int exo_flush_block(exo_blockcap *cap, void *data);
int exo_yield_to(exo_cap target);
int exo_read_disk(exo_blockcap cap, void *dst, uint64 off, uint64 n);
int exo_write_disk(exo_blockcap cap, const void *src, uint64 off, uint64 n);
exo_cap proc_alloc(void);
int set_numa_node(int);
int fcntl(int, int, int);
int sigsend(int, int);
int sigcheck(void);
int cap_inc(uint16_t id);
int cap_dec(uint16_t id);

// ulib.c
int stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, size_t);
char *strchr(const char *, char c);
int strcmp(const char *, const char *);
void printf(int, const char *, ...);
char *gets(char *, size_t max);
size_t strlen(const char *);
void *memset(void *, int, size_t);
void *malloc(size_t);
void free(void *);
int atoi(const char *);
