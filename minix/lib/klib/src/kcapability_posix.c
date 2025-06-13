// minix/lib/klib/src/kcapability_posix.c
/**
 * @file kcapability_posix.c
 * @brief Conversion helpers for POSIX capability spaces.
 *
 * These routines map POSIX file metadata to capability structures
 * described in @ref kcapability_posix.h. They are used by the
 * mathematical open system call described in the design document
 * `docs/Mathematical_POSIX_Integration_Draft.md`.
 */

#include "kcapability_posix.h"
#include <sys/stat.h>

/**
 * @brief Initialize a POSIX capability structure from file metadata.
 *
 * @param cap Target capability structure to initialize.
 * @param st  `struct stat` information describing the file.
 */
void posix_capability_init_from_stat(posix_capability_space_t *cap,
                                     const struct stat *st) {
  if (!cap || !st)
    return;

  cap->inode_identifier = st->st_ino;
  cap->owner_uid = st->st_uid;
  cap->group_gid = st->st_gid;
  cap->temporal_validity = 0;
  cap->cryptographic_nonce = 0;
  cap->access_rights = 0;

  if (st->st_mode & S_IRUSR)
    cap->access_rights |= CAP_READ;
  if (st->st_mode & S_IWUSR)
    cap->access_rights |= CAP_WRITE;
  if (st->st_mode & S_IXUSR)
    cap->access_rights |= CAP_EXECUTE;
}

/**
 * @brief Check if a capability permits the requested access.
 *
 * The check follows standard POSIX semantics for owner and group.
 *
 * @param cap      Capability describing the file.
 * @param uid      Effective UID of the requester.
 * @param gid      Effective GID of the requester.
 * @param rights   Requested capability rights mask.
 *
 * @return true if access is permitted, false otherwise.
 */
bool posix_capability_check_access(const posix_capability_space_t *cap,
                                   uid_t uid, gid_t gid, int rights) {
  if (!cap)
    return false;

  if (cap->temporal_validity && cap->temporal_validity < 0)
    return false; // Reserved for future timestamp checks

  if (uid == cap->owner_uid) {
    // Owner has full rights encoded in access_rights
    return (cap->access_rights & rights) == rights;
  }

  if (gid == cap->group_gid) {
    return (cap->access_rights & rights) == rights;
  }

  // For now treat others as having no rights.
  return false;
}
