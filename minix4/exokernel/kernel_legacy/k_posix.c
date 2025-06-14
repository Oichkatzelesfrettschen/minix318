/**
 * @file k_posix.c
 * @brief Stub implementations of core POSIX helpers used inside the kernel.
 *
 * These functions provide a stable interface for upcoming capability-based
 * POSIX routines as described in docs/Mathematical_POSIX_Integration_Draft.md.
 * They currently return -ENOSYS until full implementations are completed.
 */

#include "k_posix.h"
#include "system.h"
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

int k_posix_open(const char *path, int flags, mode_t mode) {
  (void)path;
  (void)flags;
  (void)mode;
  return -ENOSYS;
}

ssize_t k_posix_read(int fd, void *buf, size_t count) {
  (void)fd;
  (void)buf;
  (void)count;
  return -ENOSYS;
}

ssize_t k_posix_write(int fd, const void *buf, size_t count) {
  (void)fd;
  (void)buf;
  (void)count;
  return -ENOSYS;
}

int k_posix_close(int fd) {
  (void)fd;
  return -ENOSYS;
}

off_t k_posix_lseek(int fd, off_t offset, int whence) {
  (void)fd;
  (void)offset;
  (void)whence;
  return -ENOSYS;
}

int k_posix_stat(const char *path, struct stat *st) {
  (void)path;
  (void)st;
  return -ENOSYS;
}

int k_posix_fstat(int fd, struct stat *st) {
  (void)fd;
  (void)st;
  return -ENOSYS;
}

int k_posix_chmod(const char *path, mode_t mode) {
  (void)path;
  (void)mode;
  return -ENOSYS;
}

int k_posix_chown(const char *path, uid_t uid, gid_t gid) {
  (void)path;
  (void)uid;
  (void)gid;
  return -ENOSYS;
}

int k_posix_unlink(const char *path) {
  (void)path;
  return -ENOSYS;
}

int k_posix_mkdir(const char *path, mode_t mode) {
  (void)path;
  (void)mode;
  return -ENOSYS;
}

int k_posix_rmdir(const char *path) {
  (void)path;
  return -ENOSYS;
}

int k_posix_link(const char *oldpath, const char *newpath) {
  (void)oldpath;
  (void)newpath;
  return -ENOSYS;
}

int k_posix_rename(const char *oldpath, const char *newpath) {
  (void)oldpath;
  (void)newpath;
  return -ENOSYS;
}

int k_posix_dup2(int oldfd, int newfd) {
  (void)oldfd;
  (void)newfd;
  return -ENOSYS;
}
