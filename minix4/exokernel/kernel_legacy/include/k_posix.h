#pragma once
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @file k_posix.h
 * @brief Kernel-level prototypes for essential POSIX helpers.
 */

int k_posix_open(const char *path, int flags, mode_t mode);
ssize_t k_posix_read(int fd, void *buf, size_t count);
ssize_t k_posix_write(int fd, const void *buf, size_t count);
int k_posix_close(int fd);
off_t k_posix_lseek(int fd, off_t offset, int whence);
int k_posix_stat(const char *path, struct stat *st);
int k_posix_fstat(int fd, struct stat *st);
int k_posix_chmod(const char *path, mode_t mode);
int k_posix_chown(const char *path, uid_t uid, gid_t gid);
int k_posix_unlink(const char *path);
int k_posix_mkdir(const char *path, mode_t mode);
int k_posix_rmdir(const char *path);
int k_posix_link(const char *oldpath, const char *newpath);
int k_posix_rename(const char *oldpath, const char *newpath);
int k_posix_dup2(int oldfd, int newfd);
