/**
 * @file ipc_unified.c
 * @brief Unified IPC implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - userspace\usr_bin_legacy\trace\service\ipc.c
 * - minix4\libos_legacy\ipc\ipc.c
 * - minix4\libos\lib_legacy\libbdev\ipc.c
 * - minix4\libos\lib_legacy\openssl\lib\libposix\ipc.c
 * - minix4\exokernel\kernel_legacy\uts\common\os\ipc.c
 * - minix4\exokernel\kernel_legacy\cmd\truss\ipc.c
 * - minix4\exokernel\kernel_legacy\cmd\mdb\common\modules\ipc\ipc.c
 * - minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.cdpd\ipc.c

 *
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Synthesis Engine
 * @copyright Copyright (c) 2025 MINIX Project
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

/* MINIX4 system headers */
#include <minix4/kernel_types.h>
#include <minix4/config.h>

/* Architecture-specific headers */
#ifdef ARCH_X86_64
#include <minix4/arch/x86_64/arch.h>
#elif defined(ARCH_I386)
#include <minix4/arch/i386/arch.h>
#elif defined(ARCH_ARM)
#include <minix4/arch/arm/arch.h>
#endif


/* ============================================================================
 * SYNTHESIS NOTE: CAREFUL ANALYSIS REQUIRED
 * ============================================================================
 * This file contains 8 very different implementations that
 * require manual review and careful synthesis. The files have been
 * concatenated for analysis, but manual integration is needed.
 * 
 * TODO: Manual synthesis tasks:
 * 1. Analyze functional differences between implementations
 * 2. Determine best approach for each function
 * 3. Modernize to C23 standards
 * 4. Add proper error handling and security
 * 5. Create comprehensive unit tests
 * ============================================================================ */


/* ============================================================================
 * SOURCE 1/8: userspace\usr_bin_legacy\trace\service\ipc.c
 * Size: 9,997 bytes, Lines: 446
 * Hash: 66e9a45d6a1b...
 * ============================================================================ */

/* This file is concerned with the IPC server, not with kernel-level IPC. */

#include "inc.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

static void
put_key(struct trace_proc * proc, const char * name, key_t key)
{

	if (!valuesonly && key == IPC_PRIVATE)
		put_field(proc, name, "IPC_PRIVATE");
	else
		put_value(proc, name, "%ld", key);
}

static const struct flags ipcget_flags[] = {
	FLAG(IPC_CREAT),
	FLAG(IPC_EXCL),
};

static int
ipc_shmget_out(struct trace_proc * proc, const message * m_out)
{

	put_key(proc, "key", m_out->m_lc_ipc_shmget.key);
	put_value(proc, "size", "%zu", m_out->m_lc_ipc_shmget.size);
	put_flags(proc, "shmflg", ipcget_flags, COUNT(ipcget_flags), "0%o",
	    m_out->m_lc_ipc_shmget.flag);

	return CT_DONE;
}

static void
ipc_shmget_in(struct trace_proc * proc, const message * __unused m_out,
	const message * m_in, int failed)
{

	if (!failed)
		put_value(proc, NULL, "%d", m_in->m_lc_ipc_shmget.retid);
	else
		put_result(proc);
}

static const struct flags shmat_flags[] = {
	FLAG(SHM_RDONLY),
	FLAG(SHM_RND),
};

static int
ipc_shmat_out(struct trace_proc * proc, const message * m_out)
{

	put_value(proc, "shmid", "%d", m_out->m_lc_ipc_shmat.id);
	put_ptr(proc, "shmaddr", (vir_bytes)m_out->m_lc_ipc_shmat.addr);
	put_flags(proc, "shmflg", shmat_flags, COUNT(shmat_flags), "0x%x",
	    m_out->m_lc_ipc_shmat.flag);

	return CT_DONE;
}

static void
ipc_shmat_in(struct trace_proc * proc, const message * __unused m_out,
	const message * m_in, int failed)
{

	if (!failed)
		put_ptr(proc, NULL, (vir_bytes)m_in->m_lc_ipc_shmat.retaddr);
	else
		put_result(proc);
}

static int
ipc_shmdt_out(struct trace_proc * proc, const message * m_out)
{

	put_ptr(proc, "shmaddr", (vir_bytes)m_out->m_lc_ipc_shmdt.addr);

	return CT_DONE;
}

static void
put_shmctl_cmd(struct trace_proc * proc, const char * name, int cmd)
{
	const char *text = NULL;

	if (!valuesonly) {
		switch (cmd) {
		TEXT(IPC_RMID);
		TEXT(IPC_SET);
		TEXT(IPC_STAT);
		TEXT(SHM_STAT);
		TEXT(SHM_INFO);
		TEXT(IPC_INFO);
		}
	}

	if (text != NULL)
		put_field(proc, name, text);
	else
		put_value(proc, name, "%d", cmd);
}

static const struct flags shm_mode_flags[] = {
	FLAG(SHM_DEST),
	FLAG(SHM_LOCKED),
};

static void
put_struct_shmid_ds(struct trace_proc * proc, const char * name, int flags,
	vir_bytes addr)
{
	struct shmid_ds buf;
	int set;

	if (!put_open_struct(proc, name, flags, addr, &buf, sizeof(buf)))
		return;

	/* Is this an IPC_SET call?  Then print a small subset of fields.. */
	set = (flags & PF_ALT);

	put_open(proc, "shm_perm", 0, "{", ", ");

	put_value(proc, "uid", "%u", buf.shm_perm.uid);
	put_value(proc, "gid", "%u", buf.shm_perm.gid);
	if (!set && verbose > 0) {
		put_value(proc, "cuid", "%u", buf.shm_perm.cuid);
		put_value(proc, "cgid", "%u", buf.shm_perm.cgid);
	}
	put_flags(proc, "mode", shm_mode_flags, COUNT(shm_mode_flags),
	    "0%03o", buf.shm_perm.mode);

	put_close(proc, "}");

	if (!set) {
		put_value(proc, "shm_segsz", "%zu", buf.shm_segsz);
		if (verbose > 0) {
			put_value(proc, "shm_lpid", "%d", buf.shm_lpid);
			put_value(proc, "shm_cpid", "%d", buf.shm_cpid);
			put_time(proc, "shm_atime", buf.shm_atime);
			put_time(proc, "shm_dtime", buf.shm_dtime);
			put_time(proc, "shm_ctime", buf.shm_ctime);
		}
		put_value(proc, "shm_nattch", "%u", buf.shm_nattch);
	}

	put_close_struct(proc, set || verbose > 0);
}

static int
ipc_shmctl_out(struct trace_proc * proc, const message * m_out)
{

	put_value(proc, "shmid", "%d", m_out->m_lc_ipc_shmctl.id);
	put_shmctl_cmd(proc, "cmd", m_out->m_lc_ipc_shmctl.cmd);

	/* TODO: add support for the IPC_INFO and SHM_INFO structures.. */
	switch (m_out->m_lc_ipc_shmctl.cmd) {
	case IPC_STAT:
	case SHM_STAT:
		return CT_NOTDONE;

	case IPC_SET:
		put_struct_shmid_ds(proc, "buf", PF_ALT,
		    (vir_bytes)m_out->m_lc_ipc_shmctl.buf);

		return CT_DONE;

	default:
		put_ptr(proc, "buf", (vir_bytes)m_out->m_lc_ipc_shmctl.buf);

		return CT_DONE;
	}
}

static void
ipc_shmctl_in(struct trace_proc * proc, const message * m_out,
	const message * m_in, int failed)
{

	switch (m_out->m_lc_ipc_shmctl.cmd) {
	case IPC_STAT:
	case SHM_STAT:
		put_struct_shmid_ds(proc, "buf", failed,
		    (vir_bytes)m_out->m_lc_ipc_shmctl.buf);
		put_equals(proc);

		break;
	}

	if (!failed) {
		switch (m_out->m_lc_ipc_shmctl.cmd) {
		case SHM_INFO:
		case SHM_STAT:
		case IPC_INFO:
			put_value(proc, NULL, "%d", m_in->m_lc_ipc_shmctl.ret);

			return;
		}
	}

	put_result(proc);
}

static int
ipc_semget_out(struct trace_proc * proc, const message * m_out)
{

	put_key(proc, "key", m_out->m_lc_ipc_semget.key);
	put_value(proc, "nsems", "%d", m_out->m_lc_ipc_semget.nr);
	put_flags(proc, "semflg", ipcget_flags, COUNT(ipcget_flags), "0%o",
	    m_out->m_lc_ipc_semget.flag);

	return CT_DONE;
}

static void
ipc_semget_in(struct trace_proc * proc, const message * __unused m_out,
	const message * m_in, int failed)
{

	if (!failed)
		put_value(proc, NULL, "%d", m_in->m_lc_ipc_semget.retid);
	else
		put_result(proc);
}

static void
put_semctl_cmd(struct trace_proc * proc, const char * name, int cmd)
{
	const char *text = NULL;

	if (!valuesonly) {
		switch (cmd) {
		TEXT(IPC_RMID);
		TEXT(IPC_SET);
		TEXT(IPC_STAT);
		TEXT(GETNCNT);
		TEXT(GETPID);
		TEXT(GETVAL);
		TEXT(GETALL);
		TEXT(GETZCNT);
		TEXT(SETVAL);
		TEXT(SETALL);
		TEXT(SEM_STAT);
		TEXT(SEM_INFO);
		TEXT(IPC_INFO);
		}
	}

	if (text != NULL)
		put_field(proc, name, text);
	else
		put_value(proc, name, "%d", cmd);
}

static void
put_struct_semid_ds(struct trace_proc * proc, const char * name, int flags,
	vir_bytes addr)
{
	struct semid_ds buf;
	int set;

	if (!put_open_struct(proc, name, flags, addr, &buf, sizeof(buf)))
		return;

	/* Is this an IPC_SET call?  Then print a small subset of fields.. */
	set = (flags & PF_ALT);

	put_open(proc, "sem_perm", 0, "{", ", ");

	put_value(proc, "uid", "%u", buf.sem_perm.uid);
	put_value(proc, "gid", "%u", buf.sem_perm.gid);
	if (!set && verbose > 0) {
		put_value(proc, "cuid", "%u", buf.sem_perm.cuid);
		put_value(proc, "cgid", "%u", buf.sem_perm.cgid);
	}
	put_value(proc, "mode", "0%03o", buf.sem_perm.mode);

	put_close(proc, "}");

	if (!set) {
		if (verbose > 0) {
			put_time(proc, "sem_otime", buf.sem_otime);
			put_time(proc, "sem_ctime", buf.sem_ctime);
		}
		put_value(proc, "sem_nsems", "%u", buf.sem_nsems);
	}

	put_close_struct(proc, set || verbose > 0);
}


static int
ipc_semctl_out(struct trace_proc * proc, const message * m_out)
{

	put_value(proc, "semid", "%d", m_out->m_lc_ipc_semctl.id);
	put_value(proc, "semnum", "%d", m_out->m_lc_ipc_semctl.num);
	put_semctl_cmd(proc, "cmd", m_out->m_lc_ipc_semctl.cmd);

	/* TODO: add support for the IPC_INFO and SEM_INFO structures.. */
	switch (m_out->m_lc_ipc_semctl.cmd) {
	case IPC_STAT:
	case SEM_STAT:
		return CT_NOTDONE;

	case IPC_SET:
		put_struct_semid_ds(proc, "buf", PF_ALT,
		    (vir_bytes)m_out->m_lc_ipc_semctl.opt);

		return CT_DONE;

	case IPC_INFO:
	case SEM_INFO:
		put_ptr(proc, "buf", (vir_bytes)m_out->m_lc_ipc_semctl.opt);

		return CT_DONE;

	case GETALL:
	case SETALL:
		put_ptr(proc, "array", (vir_bytes)m_out->m_lc_ipc_semctl.opt);

		return CT_DONE;

	case SETVAL:
		put_value(proc, "val", "%lu", m_out->m_lc_ipc_semctl.opt);

		return CT_DONE;

	default:
		return CT_DONE;
	}
}

static void
ipc_semctl_in(struct trace_proc * proc, const message * m_out,
	const message * m_in, int failed)
{

	switch (m_out->m_lc_ipc_semctl.cmd) {
	case IPC_STAT:
	case SEM_STAT:
		put_struct_semid_ds(proc, "buf", failed,
		    (vir_bytes)m_out->m_lc_ipc_semctl.opt);
		put_equals(proc);

		break;
	}

	if (!failed) {
		switch (m_out->m_lc_ipc_semctl.cmd) {
		case GETNCNT:
		case GETPID:
		case GETVAL:
		case GETZCNT:
		case SEM_INFO:
		case SEM_STAT:
		case IPC_INFO:
			put_value(proc, NULL, "%d", m_in->m_lc_ipc_semctl.ret);
			return;
		}
	}
	put_result(proc);
}

static const struct flags sem_flags[] = {
	FLAG(IPC_NOWAIT),
	FLAG(SEM_UNDO),
};

static void
put_struct_sembuf(struct trace_proc * proc, const char * name, int flags,
	vir_bytes addr)
{
	struct sembuf buf;
	int all;

	if (!put_open_struct(proc, name, flags, addr, &buf, sizeof(buf)))
		return;

	all = FALSE;
	put_value(proc, "sem_num", "%u", buf.sem_num);
	put_value(proc, "sem_op", "%d", buf.sem_op);
	if (verbose > 0 || (buf.sem_flg & ~SEM_UNDO) != 0) {
		put_flags(proc, "sem_flg", sem_flags, COUNT(sem_flags), "0x%x",
		   buf.sem_flg);
		all = TRUE;
	}

	put_close_struct(proc, all);
}

static void
put_sembuf_array(struct trace_proc * proc, const char * name, vir_bytes addr,
	size_t count)
{
	struct sembuf buf[SEMOPM]; /* about 600 bytes, so OK for the stack */
	size_t i;

	if (valuesonly > 1 || count > SEMOPM ||
	    mem_get_data(proc->pid, addr, &buf, count * sizeof(buf[0])) != 0) {
		put_ptr(proc, name, addr);

		return;
	}

	put_open(proc, name, PF_NONAME, "[", ", ");
	for (i = 0; i < count; i++)
		put_struct_sembuf(proc, NULL, PF_LOCADDR, (vir_bytes)&buf[i]);
	put_close(proc, "]");
}

static int
ipc_semop_out(struct trace_proc * proc, const message * m_out)
{

	put_value(proc, "semid", "%d", m_out->m_lc_ipc_semop.id);
	put_sembuf_array(proc, "sops", (vir_bytes)m_out->m_lc_ipc_semop.ops,
	    m_out->m_lc_ipc_semop.size);
	put_value(proc, "nsops", "%u", m_out->m_lc_ipc_semop.size);

	return CT_DONE;
}

#define IPC_CALL(c) [((IPC_ ## c) - IPC_BASE)]

static const struct call_handler ipc_map[] = {
	IPC_CALL(SHMGET) = HANDLER("shmget", ipc_shmget_out, ipc_shmget_in),
	IPC_CALL(SHMAT) = HANDLER("shmat", ipc_shmat_out, ipc_shmat_in),
	IPC_CALL(SHMDT) = HANDLER("shmdt", ipc_shmdt_out, default_in),
	IPC_CALL(SHMCTL) = HANDLER("shmctl", ipc_shmctl_out, ipc_shmctl_in),
	IPC_CALL(SEMGET) = HANDLER("semget", ipc_semget_out, ipc_semget_in),
	IPC_CALL(SEMCTL) = HANDLER("semctl", ipc_semctl_out, ipc_semctl_in),
	IPC_CALL(SEMOP) = HANDLER("semop", ipc_semop_out, default_in),
};

const struct calls ipc_calls = {
	.endpt = ANY,
	.base = IPC_BASE,
	.map = ipc_map,
	.count = COUNT(ipc_map)
};



/* ============================================================================
 * SOURCE 2/8: minix4\libos_legacy\ipc\ipc.c
 * Size: 584 bytes, Lines: 30
 * Hash: c9af812601db...
 * ============================================================================ */

#include "posix.h"
#include "exo_ipc.h"
#include "exokernel.h"

int libos_msgq_send(exo_cap dest, const void *buf, size_t len) {
    return exo_send(dest, buf, len);
}

int libos_msgq_recv(exo_cap src, void *buf, size_t len) {
    return exo_recv(src, buf, len);
}

int libos_sem_post(exo_cap sem) {
    return exo_send(sem, NULL, 0);
}

int libos_sem_wait(exo_cap sem) {
    char tmp;
    int r = exo_recv(sem, &tmp, 1);
    return r < 0 ? r : 0;
}

exo_cap libos_shm_alloc(void) {
    return exo_alloc_page();
}

int libos_shm_free(exo_cap cap) {
    return exo_unbind_page(cap);
}



/* ============================================================================
 * SOURCE 3/8: minix4\libos\lib_legacy\libbdev\ipc.c
 * Size: 9,289 bytes, Lines: 347
 * Hash: 456528c52b6a...
 * ============================================================================ */

/* libbdev - IPC and recovery functions */

#include <minix/drivers.h>
#include <minix/bdev.h>
#include <assert.h>

#include "const.h"
#include "type.h"
#include "proto.h"

static void bdev_cancel(dev_t dev)
{
/* Recovering the driver for the given device has failed repeatedly. Mark it as
 * permanently unusable, and clean up any associated calls and resources.
 */
  bdev_call_t *call, *next;

  printf("bdev: giving up on major %d\n", major(dev));

  /* Cancel all pending asynchronous requests. */
  call = NULL;

  while ((call = bdev_call_iter_maj(dev, call, &next)) != NULL)
	bdev_callback_asyn(call, EDEADSRCDST);

  /* Mark the driver as unusable. */
  bdev_driver_clear(dev);
}

static int bdev_recover(dev_t dev, int update_endpt)
{
/* The IPC subsystem has signaled an error communicating to the driver
 * associated with the given device. Try to recover. If 'update_endpt' is set,
 * we need to find the new endpoint of the driver first. Return TRUE iff
 * recovery has been successful.
 */
  bdev_call_t *call, *next;
  endpoint_t endpt;
  int r, active, nr_tries;

  /* Only print output if there is something to recover. Some drivers may be
   * shut down and later restarted legitimately, and if they were not in use
   * while that happened, there is no need to flood the console with messages.
   */
  active = bdev_minor_is_open(dev) || bdev_call_iter_maj(dev, NULL, &next);

  if (active)
	printf("bdev: recovering from a driver restart on major %d\n",
		major(dev));

  for (nr_tries = 0; nr_tries < RECOVER_TRIES; nr_tries++) {
	/* First update the endpoint, if necessary. */
	if (update_endpt)
		(void) bdev_driver_update(dev);

	if ((endpt = bdev_driver_get(dev)) == NONE)
		break;

	/* If anything goes wrong, update the endpoint again next time. */
	update_endpt = TRUE;

	/* Reopen all minor devices on the new driver. */
	if ((r = bdev_minor_reopen(dev)) != OK) {
		/* If the driver died again, we may give it another try. */
		if (r == EDEADSRCDST)
			continue;

		/* If another error occurred, we cannot continue using the
		 * driver as is, but we also cannot force it to restart.
		 */
		break;
	}

	/* Resend all asynchronous requests. */
	call = NULL;

	while ((call = bdev_call_iter_maj(dev, call, &next)) != NULL) {
		/* It is not strictly necessary that we manage to reissue all
		 * asynchronous requests successfully. We can fail them on an
		 * individual basis here, without affecting the overall
		 * recovery. Note that we will never get new IPC failures here.
		 */
		if ((r = bdev_restart_asyn(call)) != OK)
			bdev_callback_asyn(call, r);
	}

	/* Recovery seems successful. We can now reissue the current
	 * synchronous request (if any), and continue normal operation.
	 */
	if (active)
		printf("bdev: recovery successful, new driver at %d\n", endpt);

	return TRUE;
  }

  /* Recovery failed repeatedly. Give up on this driver. */
  bdev_cancel(dev);

  return FALSE;
}

void bdev_update(dev_t dev, char *label)
{
/* Set the endpoint for a driver. Perform recovery if necessary.
 */
  endpoint_t endpt, old_endpt;

  old_endpt = bdev_driver_get(dev);

  endpt = bdev_driver_set(dev, label);

  /* If updating the driver causes an endpoint change, we need to perform
   * recovery, but not update the endpoint yet again.
   */
  if (old_endpt != NONE && old_endpt != endpt)
	bdev_recover(dev, FALSE /*update_endpt*/);
}

int bdev_senda(dev_t dev, const message *m_orig, bdev_id_t id)
{
/* Send an asynchronous request for the given device. This function will never
 * get any new IPC errors sending to the driver. If sending an asynchronous
 * request fails, we will find out through other ways later.
 */
  endpoint_t endpt;
  message m;
  int r;

  /* If we have no usable driver endpoint, fail instantly. */
  if ((endpt = bdev_driver_get(dev)) == NONE)
	return EDEADSRCDST;

  m = *m_orig;
  m.m_lbdev_lblockdriver_msg.id = id;

  r = asynsend(endpt, &m);

  if (r != OK)
	printf("bdev: asynsend to driver (%d) failed (%d)\n", endpt, r);

  return r;
}

int bdev_sendrec(dev_t dev, const message *m_orig)
{
/* Send a synchronous request for the given device, and wait for the reply.
 * Return ERESTART if the caller should try to reissue the request.
 */
  endpoint_t endpt;
  message m;
  int r;

  /* If we have no usable driver endpoint, fail instantly. */
  if ((endpt = bdev_driver_get(dev)) == NONE)
	return EDEADSRCDST;

  /* Send the request and block until we receive a reply. */
  m = *m_orig;
  m.m_lbdev_lblockdriver_msg.id = NO_ID;

  r = ipc_sendrec(endpt, &m);

  /* If communication failed, the driver has died. We assume it will be
   * restarted soon after, so we attempt recovery. Upon success, we let the
   * caller reissue the synchronous request.
   */
  if (r == EDEADSRCDST) {
	if (!bdev_recover(dev, TRUE /*update_endpt*/))
		return EDEADSRCDST;

	return ERESTART;
  }

  if (r != OK) {
	printf("bdev: IPC to driver (%d) failed (%d)\n", endpt, r);
	return r;
  }

  if (m.m_type != BDEV_REPLY) {
	printf("bdev: driver (%d) sent weird response (%d)\n",
		endpt, m.m_type);
	return EINVAL;
  }

  /* The protocol contract states that no asynchronous reply can satisfy a
   * synchronous SENDREC call, so we can never get an asynchronous reply here.
   */
  if (m.m_lblockdriver_lbdev_reply.id != NO_ID) {
	printf("bdev: driver (%d) sent invalid ID (%d)\n", endpt,
		m.m_lblockdriver_lbdev_reply.id);
	return EINVAL;
  }

  /* Unless the caller is misusing libbdev, we will only get ERESTART if we
   * have managed to resend a raw block I/O request to the driver after a
   * restart, but before VFS has had a chance to reopen the associated device
   * first. This is highly exceptional, and hard to deal with correctly. We
   * take the easiest route: sleep for a while so that VFS can reopen the
   * device, and then resend the request. If the call keeps failing, the caller
   * will eventually give up.
   */
  if (m.m_lblockdriver_lbdev_reply.status == ERESTART) {
	printf("bdev: got ERESTART from driver (%d), sleeping for reopen\n",
		endpt);

	micro_delay(1000);

	return ERESTART;
  }

  /* Return the result of our request. */
  return m.m_lblockdriver_lbdev_reply.status;
}

static int bdev_receive(dev_t dev, message *m)
{
/* Receive one valid message.
 */
  endpoint_t endpt;
  int r, nr_tries = 0;

  for (;;) {
	/* Retrieve and check the driver endpoint on every try, as it will
	 * change with each driver restart.
	 */
	if ((endpt = bdev_driver_get(dev)) == NONE)
		return EDEADSRCDST;

	r = sef_receive(endpt, m);

	if (r == EDEADSRCDST) {
		/* If we reached the maximum number of retries, give up. */
		if (++nr_tries == DRIVER_TRIES)
			break;

		/* Attempt recovery. If successful, all asynchronous requests
		 * will have been resent, and we can retry receiving a reply.
		 */
		if (!bdev_recover(dev, TRUE /*update_endpt*/))
			return EDEADSRCDST;

		continue;
	}

	if (r != OK) {
		printf("bdev: IPC to driver (%d) failed (%d)\n", endpt, r);

		return r;
	}

	if (m->m_type != BDEV_REPLY) {
		printf("bdev: driver (%d) sent weird response (%d)\n",
			endpt, m->m_type);
		return EINVAL;
	}

	/* The caller is responsible for checking the ID and status. */
	return OK;
  }

  /* All tries failed, even though all recovery attempts succeeded. In this
   * case, we let the caller recheck whether it wants to keep calling us,
   * returning ERESTART to indicate we can be called again but did not actually
   * receive a message.
   */
  return ERESTART;
}

void bdev_reply_asyn(message *m)
{
/* A reply has come in from a disk driver.
 */
  bdev_call_t *call;
  endpoint_t endpt;
  bdev_id_t id;
  int r;

  /* This is a requirement for the caller. */
  assert(m->m_type == BDEV_REPLY);

  /* Get the corresponding asynchronous call structure. */
  id = m->m_lblockdriver_lbdev_reply.id;

  if ((call = bdev_call_get(id)) == NULL) {
	printf("bdev: driver (%d) replied to unknown request (%d)\n",
		m->m_source, m->m_lblockdriver_lbdev_reply.id);
	return;
  }

  /* Make sure the reply was sent from the right endpoint. */
  endpt = bdev_driver_get(call->dev);

  if (m->m_source != endpt) {
	/* If the endpoint is NONE, this may be a stray reply. */
	if (endpt != NONE)
		printf("bdev: driver (%d) replied to request not sent to it\n",
			m->m_source);
	return;
  }

  /* See the ERESTART comment in bdev_sendrec(). */
  if (m->m_lblockdriver_lbdev_reply.status == ERESTART) {
	printf("bdev: got ERESTART from driver (%d), sleeping for reopen\n",
		endpt);

	micro_delay(1000);

	if ((r = bdev_restart_asyn(call)) != OK)
		bdev_callback_asyn(call, r);

	return;
  }

  bdev_callback_asyn(call, m->m_lblockdriver_lbdev_reply.status);
}

int bdev_wait_asyn(bdev_id_t id)
{
/* Wait for an asynchronous request to complete.
 */
  bdev_call_t *call;
  dev_t dev;
  message m;
  int r;

  if ((call = bdev_call_get(id)) == NULL)
	return ENOENT;

  dev = call->dev;

  do {
	if ((r = bdev_receive(dev, &m)) != OK && r != ERESTART)
		return r;

	/* Processing the reply will free up the call structure as a side
	 * effect. If we repeatedly get ERESTART, we will repeatedly resend the
	 * asynchronous request, which will then eventually hit the retry limit
	 * and we will break out of the loop.
	 */
	if (r == OK)
		bdev_reply_asyn(&m);

  } while (bdev_call_get(id) != NULL);

  return OK;
}



/* ============================================================================
 * SOURCE 4/8: minix4\libos\lib_legacy\openssl\lib\libposix\ipc.c
 * Size: 4,135 bytes, Lines: 181
 * Hash: e757e428bea7...
 * ============================================================================ */

#include "posix_ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct cap *cap_alloc(unsigned long rights)
{
    struct cap *c = malloc(sizeof(*c));
    if (!c)
        return NULL;
    c->parent = NULL;
    c->children = NULL;
    c->next_sibling = NULL;
    c->epoch = 0;
    c->rights = rights;
    c->flags = 0;
    return c;
}

/* ------------------------ Message queues ------------------------ */
cap_mq_t *cap_mq_open(const char *name, int oflag, mode_t mode,
                      unsigned int maxmsg)
{
    cap_mq_t *q = malloc(sizeof(*q));
    if (!q)
        return NULL;
    struct mq_attr attr = {0};
    if (maxmsg)
        attr.mq_maxmsg = maxmsg;
    if (attr.mq_maxmsg == 0)
        attr.mq_maxmsg = 8;
    attr.mq_msgsize = 128;
    q->mq = mq_open(name, oflag, mode, &attr);
    if (q->mq == (mqd_t)-1) {
        free(q);
        return NULL;
    }
    q->cap = cap_alloc(MQ_RIGHT_SEND | MQ_RIGHT_RECV | MQ_RIGHT_CLOSE);
    if (!q->cap) {
        mq_close(q->mq);
        free(q);
        return NULL;
    }
    return q;
}

int cap_mq_send(cap_mq_t *q, const char *msg, size_t len, unsigned int prio)
{
    if (!q || !q->cap || !(q->cap->rights & MQ_RIGHT_SEND)) {
        errno = EPERM;
        return -1;
    }
    return mq_send(q->mq, msg, len, prio);
}

ssize_t cap_mq_receive(cap_mq_t *q, char *msg, size_t len, unsigned int *prio)
{
    if (!q || !q->cap || !(q->cap->rights & MQ_RIGHT_RECV)) {
        errno = EPERM;
        return -1;
    }
    return mq_receive(q->mq, msg, len, prio);
}

int cap_mq_close(cap_mq_t *q)
{
    if (!q)
        return -1;
    if (q->cap && (q->cap->rights & MQ_RIGHT_CLOSE))
        mq_close(q->mq);
    free(q->cap);
    free(q);
    return 0;
}

/* ------------------------ Semaphores ------------------------ */
cap_sem_t *cap_sem_open(const char *name, int oflag, mode_t mode,
                        unsigned int value)
{
    cap_sem_t *s = malloc(sizeof(*s));
    if (!s)
        return NULL;
    s->sem = sem_open(name, oflag, mode, value);
    if (s->sem == SEM_FAILED) {
        free(s);
        return NULL;
    }
    s->cap = cap_alloc(SEM_RIGHT_WAIT | SEM_RIGHT_POST | SEM_RIGHT_CLOSE);
    if (!s->cap) {
        sem_close(s->sem);
        free(s);
        return NULL;
    }
    return s;
}

int cap_sem_wait(cap_sem_t *s)
{
    if (!s || !s->cap || !(s->cap->rights & SEM_RIGHT_WAIT)) {
        errno = EPERM;
        return -1;
    }
    return sem_wait(s->sem);
}

int cap_sem_post(cap_sem_t *s)
{
    if (!s || !s->cap || !(s->cap->rights & SEM_RIGHT_POST)) {
        errno = EPERM;
        return -1;
    }
    return sem_post(s->sem);
}

int cap_sem_close(cap_sem_t *s)
{
    if (!s)
        return -1;
    if (s->cap && (s->cap->rights & SEM_RIGHT_CLOSE))
        sem_close(s->sem);
    free(s->cap);
    free(s);
    return 0;
}

/* ------------------------ Shared memory ------------------------ */
cap_shm_t *cap_shm_open(const char *name, int oflag, mode_t mode, size_t size)
{
    cap_shm_t *shm = malloc(sizeof(*shm));
    if (!shm)
        return NULL;
    shm->fd = shm_open(name, oflag, mode);
    if (shm->fd < 0) {
        free(shm);
        return NULL;
    }
    if ((oflag & O_CREAT) && size > 0) {
        if (ftruncate(shm->fd, (off_t)size) < 0) {
            close(shm->fd);
            free(shm);
            return NULL;
        }
    }
    shm->cap = cap_alloc(SHM_RIGHT_MAP | SHM_RIGHT_UNMAP | SHM_RIGHT_CLOSE);
    if (!shm->cap) {
        close(shm->fd);
        free(shm);
        return NULL;
    }
    return shm;
}

void *cap_shm_map(cap_shm_t *shm, size_t len, int prot, int flags, off_t off)
{
    if (!shm || !shm->cap || !(shm->cap->rights & SHM_RIGHT_MAP)) {
        errno = EPERM;
        return MAP_FAILED;
    }
    return mmap(NULL, len, prot, flags, shm->fd, off);
}

int cap_shm_unmap(void *addr, size_t len)
{
    return munmap(addr, len);
}

int cap_shm_close(cap_shm_t *shm)
{
    if (!shm)
        return -1;
    if (shm->cap && (shm->cap->rights & SHM_RIGHT_CLOSE))
        close(shm->fd);
    free(shm->cap);
    free(shm);
    return 0;
}




/* ============================================================================
 * SOURCE 5/8: minix4\exokernel\kernel_legacy\uts\common\os\ipc.c
 * Size: 41,769 bytes, Lines: 1,410
 * Hash: b1d8c9ec1d12...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 1988, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2016 Joyent, Inc.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T		*/
/*	All Rights Reserved					*/


/*
 * Common Inter-Process Communication routines.
 *
 * Overview
 * --------
 *
 * The System V inter-process communication (IPC) facilities provide
 * three services, message queues, semaphore arrays, and shared memory
 * segments, which are mananged using filesystem-like namespaces.
 * Unlike a filesystem, these namespaces aren't mounted and accessible
 * via a path -- a special API is used to interact with the different
 * facilities (nothing precludes a VFS-based interface, but the
 * standards require the special APIs).  Furthermore, these special
 * APIs don't use file descriptors, nor do they have an equivalent.
 * This means that every operation which acts on an object needs to
 * perform the quivalent of a lookup, which in turn means that every
 * operation can fail if the specified object doesn't exist in the
 * facility's namespace.
 *
 * Objects
 * -------
 *
 * Each object in a namespace has a unique ID, which is assigned by the
 * system and is used to identify the object when performing operations
 * on it.  An object can also have a key, which is selected by the user
 * at allocation time and is used as a primitive rendezvous mechanism.
 * An object without a key is said to have a "private" key.
 *
 * To perform an operation on an object given its key, one must first
 * perform a lookup and obtain its ID.  The ID is then used to identify
 * the object when performing the operation.  If the object has a
 * private key, the ID must be known or obtained by other means.
 *
 * Each object in the namespace has a creator uid and gid, as well as
 * an owner uid and gid.  Both are initialized with the ruid and rgid
 * of the process which created the object.  The creator or current
 * owner has the ability to change the owner of the object.
 *
 * Each object in the namespace has a set of file-like permissions,
 * which, in conjunction with the creator and owner uid and gid,
 * control read and write access to the object (execute is ignored).
 *
 * Each object also has a creator project and zone, which are used to
 * account for its resource usage.
 *
 * Operations
 * ----------
 *
 * There are five operations which all three facilities have in
 * common: GET, SET, STAT, RMID, and IDS.
 *
 * GET, like open, is used to allocate a new object or obtain an
 * existing one (using its key).  It takes a key, a set of flags and
 * mode bits, and optionally facility-specific arguments.  If the key
 * is IPC_PRIVATE, a new object with the requested mode bits and
 * facility-specific attributes is created.  If the key isn't
 * IPC_PRIVATE, the GET will attempt to look up the specified key and
 * either return that or create a new key depending on the state of the
 * IPC_CREAT and IPC_EXCL flags, much like open.  If GET needs to
 * allocate an object, it can fail if there is insufficient space in
 * the namespace (the maximum number of ids for the facility has been
 * exceeded) or if the facility-specific initialization fails.  If GET
 * finds an object it can return, it can still fail if that object's
 * permissions or facility-specific attributes are less than those
 * requested.
 *
 * SET is used to adjust facility-specific parameters of an object, in
 * addition to the owner uid and gid, and mode bits.  It can fail if
 * the caller isn't the creator or owner.
 *
 * STAT is used to obtain information about an object including the
 * general attributes object described as well as facility-specific
 * information.  It can fail if the caller doesn't have read
 * permission.
 *
 * RMID removes an object from the namespace.  Subsequent operations
 * using the object's ID or key will fail (until another object is
 * created with the same key or ID).  Since an RMID may be performed
 * asynchronously with other operations, it is possible that other
 * threads and/or processes will have references to the object.  While
 * a facility may have actions which need to be performed at RMID time,
 * only when all references are dropped can the object be destroyed.
 * RMID will fail if the caller isn't the creator or owner.
 *
 * IDS obtains a list of all IDs in a facility's namespace.  There are
 * no facility-specific behaviors of IDS.
 *
 * Design
 * ------
 *
 * Because some IPC facilities provide services whose operations must
 * scale, a mechanism which allows fast, concurrent access to
 * individual objects is needed.  Of primary importance is object
 * lookup based on ID (SET, STAT, others).  Allocation (GET),
 * deallocation (RMID), ID enumeration (IDS), and key lookups (GET) are
 * lesser concerns, but should be implemented in such a way that ID
 * lookup isn't affected (at least not in the common case).
 *
 * Starting from the bottom up, each object is represented by a
 * structure, the first member of which must be a kipc_perm_t.  The
 * kipc_perm_t contains the information described above in "Objects", a
 * reference count (since the object may continue to exist after it has
 * been removed from the namespace), as well as some additional
 * metadata used to manage data structure membership.  These objects
 * are dynamically allocated.
 *
 * Above the objects is a power-of-two sized table of ID slots.  Each
 * slot contains a pointer to an object, a sequence number, and a
 * lock.  An object's ID is a function of its slot's index in the table
 * and its slot's sequence number.  Every time a slot is released (via
 * RMID) its sequence number is increased.  Strictly speaking, the
 * sequence number is unnecessary.  However, checking the sequence
 * number after a lookup provides a certain degree of robustness
 * against the use of stale IDs (useful since nothing else does).  When
 * the table fills up, it is resized (see Locking, below).
 *
 * Of an ID's 31 bits (an ID is, as defined by the standards, a signed
 * int) the top IPC_SEQ_BITS are used for the sequence number with the
 * remainder holding the index into the table.  The size of the table
 * is therefore bounded at 2 ^ (31 - IPC_SEQ_BITS) slots.
 *
 * Managing this table is the ipc_service structure.  It contains a
 * pointer to the dynamically allocated ID table, a namespace-global
 * lock, an id_space for managing the free space in the table, and
 * sundry other metadata necessary for the maintenance of the
 * namespace.  An AVL tree of all keyed objects in the table (sorted by
 * key) is used for key lookups.  An unordered doubly linked list of
 * all objects in the namespace (keyed or not) is maintained to
 * facilitate ID enumeration.
 *
 * To help visualize these relationships, here's a picture of a
 * namespace with a table of size 8 containing three objects
 * (IPC_SEQ_BITS = 28):
 *
 *
 * +-ipc_service_t--+
 * | table          *---\
 * | keys           *---+----------------------\
 * | all ids        *--\|                      |
 * |                |  ||                      |
 * +----------------+  ||                      |
 *                     ||                      |
 * /-------------------/|                      |
 * |    /---------------/                      |
 * |    |                                      |
 * |    v                                      |
 * |  +-0------+-1------+-2------+-3------+-4--+---+-5------+-6------+-7------+
 * |  | Seq=3  |        |        | Seq=1  |    :   |        |        | Seq=6  |
 * |  |        |        |        |        |    :   |        |        |        |
 * |  +-*------+--------+--------+-*------+----+---+--------+--------+-*------+
 * |    |                          |           |                       |
 * |    |                      /---/           |      /----------------/
 * |    |                      |               |      |
 * |    v                      v               |      v
 * |  +-kipc_perm_t-+        +-kipc_perm_t-+   |    +-kipc_perm_t-+
 * |  | id=0x30     |        | id=0x13     |   |    | id=0x67     |
 * |  | key=0xfeed  |        | key=0xbeef  |   |    | key=0xcafe  |
 * \->| [list]      |<------>| [list]      |<------>| [list]      |
 * /->| [avl left]  x   /--->| [avl left]  x   \--->| [avl left]  *---\
 * |  | [avl right] x   |    | [avl right] x        | [avl right] *---+-\
 * |  |             |   |    |             |        |             |   | |
 * |  +-------------+   |    +-------------+        +-------------+   | |
 * |                    \---------------------------------------------/ |
 * \--------------------------------------------------------------------/
 *
 * Locking
 * -------
 *
 * There are three locks (or sets of locks) which are used to ensure
 * correctness: the slot locks, the namespace lock, and p_lock (needed
 * when checking resource controls).  Their ordering is
 *
 *   namespace lock -> slot lock 0 -> ... -> slot lock t -> p_lock
 *
 * Generally speaking, the namespace lock is used to protect allocation
 * and removal from the namespace, ID enumeration, and resizing the ID
 * table.  Specifically:
 *
 * - write access to all fields of the ipc_service structure
 * - read access to all variable fields of ipc_service except
 *   ipcs_tabsz (table size) and ipcs_table (the table pointer)
 * - read/write access to ipc_avl, ipc_list in visible objects'
 *   kipc_perm structures (i.e. objects which have been removed from
 *   the namespace don't have this restriction)
 * - write access to ipct_seq and ipct_data in the table entries
 *
 * A slot lock by itself is meaningless (except when resizing).  Of
 * greater interest conceptually is the notion of an ID lock -- a
 * "virtual lock" which refers to whichever slot lock an object's ID
 * currently hashes to.
 *
 * An ID lock protects all objects with that ID.  Normally there will
 * only be one such object: the one pointed to by the locked slot.
 * However, if an object is removed from the namespace but retains
 * references (e.g. an attached shared memory segment which has been
 * RMIDed), it continues to use the lock associated with its original
 * ID.  While this can result in increased contention, operations which
 * require taking the ID lock of removed objects are infrequent.
 *
 * Specifically, an ID lock protects the contents of an object's
 * structure, including the contents of the embedded kipc_perm
 * structure (but excluding those fields protected by the namespace
 * lock).  It also protects the ipct_seq and ipct_data fields in its
 * slot (it is really a slot lock, after all).
 *
 * Recall that the table is resizable.  To avoid requiring every ID
 * lookup to take a global lock, a scheme much like that employed for
 * file descriptors (see the comment above UF_ENTER in user.h) is
 * used.  Note that the sequence number and data pointer are protected
 * by both the namespace lock and their slot lock.  When the table is
 * resized, the following operations take place:
 *
 *   1) A new table is allocated.
 *   2) The global lock is taken.
 *   3) All old slots are locked, in order.
 *   4) The first half of the new slots are locked.
 *   5) All table entries are copied to the new table, and cleared from
 *	the old table.
 *   6) The ipc_service structure is updated to point to the new table.
 *   7) The ipc_service structure is updated with the new table size.
 *   8) All slot locks (old and new) are dropped.
 *
 * Because the slot locks are embedded in the table, ID lookups and
 * other operations which require taking an slot lock need to verify
 * that the lock taken wasn't part of a stale table.  This is
 * accomplished by checking the table size before and after
 * dereferencing the table pointer and taking the lock: if the size
 * changes, the lock must be dropped and reacquired.  It is this
 * additional work which distinguishes an ID lock from a slot lock.
 *
 * Because we can't guarantee that threads aren't accessing the old
 * tables' locks, they are never deallocated.  To prevent spurious
 * reports of memory leaks, a pointer to the discarded table is stored
 * in the new one in step 5.  (Theoretically ipcs_destroy will delete
 * the discarded tables, but it is only ever called from a failed _init
 * invocation; i.e. when there aren't any.)
 *
 * Interfaces
 * ----------
 *
 * The following interfaces are provided by the ipc module for use by
 * the individual IPC facilities:
 *
 * ipcperm_access
 *
 *   Given an object and a cred structure, determines if the requested
 *   access type is allowed.
 *
 * ipcperm_set, ipcperm_stat,
 * ipcperm_set64, ipcperm_stat64
 *
 *   Performs the common portion of an STAT or SET operation.  All
 *   (except stat and stat64) can fail, so they should be called before
 *   any facility-specific non-reversible changes are made to an
 *   object.  Similarly, the set operations have side effects, so they
 *   should only be called once the possibility of a facility-specific
 *   failure is eliminated.
 *
 * ipcs_create
 *
 *   Creates an IPC namespace for use by an IPC facility.
 *
 * ipcs_destroy
 *
 *   Destroys an IPC namespace.
 *
 * ipcs_lock, ipcs_unlock
 *
 *   Takes the namespace lock.  Ideally such access wouldn't be
 *   necessary, but there may be facility-specific data protected by
 *   this lock (e.g. project-wide resource consumption).
 *
 * ipc_lock
 *
 *   Takes the lock associated with an ID.  Can't fail.
 *
 * ipc_relock
 *
 *   Like ipc_lock, but takes a pointer to a held lock.  Drops the lock
 *   unless it is the one that would have been returned by ipc_lock.
 *   Used after calls to cv_wait.
 *
 * ipc_lookup
 *
 *   Performs an ID lookup, returns with the ID lock held.  Fails if
 *   the ID doesn't exist in the namespace.
 *
 * ipc_hold
 *
 *   Takes a reference on an object.
 *
 * ipc_rele
 *
 *   Releases a reference on an object, and drops the object's lock.
 *   Calls the object's destructor if last reference is being
 *   released.
 *
 * ipc_rele_locked
 *
 *   Releases a reference on an object.  Doesn't drop lock, and may
 *   only be called when there is more than one reference to the
 *   object.
 *
 * ipc_get, ipc_commit_begin, ipc_commit_end, ipc_cleanup
 *
 *   Components of a GET operation.  ipc_get performs a key lookup,
 *   allocating an object if the key isn't found (returning with the
 *   namespace lock and p_lock held), and returning the existing object
 *   if it is (with the object lock held).  ipc_get doesn't modify the
 *   namespace.
 *
 *   ipc_commit_begin begins the process of inserting an object
 *   allocated by ipc_get into the namespace, and can fail.  If
 *   successful, it returns with the namespace lock and p_lock held.
 *   ipc_commit_end completes the process of inserting an object into
 *   the namespace and can't fail.  The facility can call ipc_cleanup
 *   at any time following a successful ipc_get and before
 *   ipc_commit_end or a failed ipc_commit_begin to fail the
 *   allocation.  Pseudocode for the suggested GET implementation:
 *
 *   top:
 *
 *     ipc_get
 *
 *     if failure
 *       return
 *
 *     if found {
 *
 *	 if object meets criteria
 *	   unlock object and return success
 *       else
 *	   unlock object and return failure
 *
 *     } else {
 *
 *	 perform resource control tests
 *	 drop namespace lock, p_lock
 *	 if failure
 *	   ipc_cleanup
 *
 *       perform facility-specific initialization
 *	 if failure {
 *	   facility-specific cleanup
 *	   ipc_cleanup
 *       }
 *
 *	 ( At this point the object should be destructible using the
 *	   destructor given to ipcs_create )
 *
 *       ipc_commit_begin
 *	 if retry
 *	   goto top
 *       else if failure
 *         return
 *
 *       perform facility-specific resource control tests/allocations
 *	 if failure
 *	   ipc_cleanup
 *
 *	 ipc_commit_end
 *	 perform any infallible post-creation actions, unlock, and return
 *
 *     }
 *
 * ipc_rmid
 *
 *   Performs the common portion of an RMID operation -- looks up an ID
 *   removes it, and calls the a facility-specific function to do
 *   RMID-time cleanup on the private portions of the object.
 *
 * ipc_ids
 *
 *   Performs the common portion of an IDS operation.
 *
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/cred.h>
#include <sys/policy.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/ipc.h>
#include <sys/ipc_impl.h>
#include <sys/errno.h>
#include <sys/systm.h>
#include <sys/list.h>
#include <sys/atomic.h>
#include <sys/zone.h>
#include <sys/task.h>
#include <sys/modctl.h>

#include <c2/audit.h>

static struct modlmisc modlmisc = {
	&mod_miscops,
	"common ipc code",
};

static struct modlinkage modlinkage = {
	MODREV_1, (void *)&modlmisc, NULL
};


int
_init(void)
{
	return (mod_install(&modlinkage));
}

int
_fini(void)
{
	return (mod_remove(&modlinkage));
}

int
_info(struct modinfo *modinfop)
{
	return (mod_info(&modlinkage, modinfop));
}


/*
 * Check message, semaphore, or shared memory access permissions.
 *
 * This routine verifies the requested access permission for the current
 * process.  The zone ids are compared, and the appropriate bits are
 * checked corresponding to owner, group (including the list of
 * supplementary groups), or everyone.  Zero is returned on success.
 * On failure, the security policy is asked to check to override the
 * permissions check; the policy will either return 0 for access granted
 * or EACCES.
 *
 * Access to objects in other zones requires that the caller be in the
 * global zone and have the appropriate IPC_DAC_* privilege, regardless
 * of whether the uid or gid match those of the object.  Note that
 * cross-zone accesses will normally never get here since they'll
 * fail in ipc_lookup or ipc_get.
 *
 * The arguments must be set up as follows:
 *	p - Pointer to permission structure to verify
 *	mode - Desired access permissions
 */
int
ipcperm_access(kipc_perm_t *p, int mode, cred_t *cr)
{
	int shifts = 0;
	uid_t uid = crgetuid(cr);
	zoneid_t zoneid = getzoneid();

	if (p->ipc_zoneid == zoneid) {
		if (uid != p->ipc_uid && uid != p->ipc_cuid) {
			shifts += 3;
			if (!groupmember(p->ipc_gid, cr) &&
			    !groupmember(p->ipc_cgid, cr))
				shifts += 3;
		}

		mode &= ~(p->ipc_mode << shifts);

		if (mode == 0)
			return (0);
	} else if (zoneid != GLOBAL_ZONEID)
		return (EACCES);

	return (secpolicy_ipc_access(cr, p, mode));
}

/*
 * There are two versions of the ipcperm_set/stat functions:
 *   ipcperm_???        - for use with IPC_SET/STAT
 *   ipcperm_???_64     - for use with IPC_SET64/STAT64
 *
 * These functions encapsulate the common portions (copying, permission
 * checks, and auditing) of the set/stat operations.  All, except for
 * stat and stat_64 which are void, return 0 on success or a non-zero
 * errno value on error.
 */

int
ipcperm_set(ipc_service_t *service, struct cred *cr,
    kipc_perm_t *kperm, struct ipc_perm *perm, model_t model)
{
	STRUCT_HANDLE(ipc_perm, lperm);
	uid_t uid;
	gid_t gid;
	mode_t mode;
	zone_t *zone;

	ASSERT(IPC_LOCKED(service, kperm));

	STRUCT_SET_HANDLE(lperm, model, perm);
	uid = STRUCT_FGET(lperm, uid);
	gid = STRUCT_FGET(lperm, gid);
	mode = STRUCT_FGET(lperm, mode);

	if (secpolicy_ipc_owner(cr, kperm) != 0)
		return (EPERM);

	zone = crgetzone(cr);
	if (!VALID_UID(uid, zone) || !VALID_GID(gid, zone))
		return (EINVAL);

	kperm->ipc_uid = uid;
	kperm->ipc_gid = gid;
	kperm->ipc_mode = (mode & 0777) | (kperm->ipc_mode & ~0777);

	if (AU_AUDITING())
		audit_ipcget(service->ipcs_atype, kperm);

	return (0);
}

void
ipcperm_stat(struct ipc_perm *perm, kipc_perm_t *kperm, model_t model)
{
	STRUCT_HANDLE(ipc_perm, lperm);

	STRUCT_SET_HANDLE(lperm, model, perm);
	STRUCT_FSET(lperm, uid, kperm->ipc_uid);
	STRUCT_FSET(lperm, gid, kperm->ipc_gid);
	STRUCT_FSET(lperm, cuid, kperm->ipc_cuid);
	STRUCT_FSET(lperm, cgid, kperm->ipc_cgid);
	STRUCT_FSET(lperm, mode, kperm->ipc_mode);
	STRUCT_FSET(lperm, seq, 0);
	STRUCT_FSET(lperm, key, kperm->ipc_key);
}

int
ipcperm_set64(ipc_service_t *service, struct cred *cr,
    kipc_perm_t *kperm, ipc_perm64_t *perm64)
{
	zone_t *zone;

	ASSERT(IPC_LOCKED(service, kperm));

	if (secpolicy_ipc_owner(cr, kperm) != 0)
		return (EPERM);

	zone = crgetzone(cr);
	if (!VALID_UID(perm64->ipcx_uid, zone) ||
	    !VALID_GID(perm64->ipcx_gid, zone))
		return (EINVAL);

	kperm->ipc_uid = perm64->ipcx_uid;
	kperm->ipc_gid = perm64->ipcx_gid;
	kperm->ipc_mode = (perm64->ipcx_mode & 0777) |
	    (kperm->ipc_mode & ~0777);

	if (AU_AUDITING())
		audit_ipcget(service->ipcs_atype, kperm);

	return (0);
}

void
ipcperm_stat64(ipc_perm64_t *perm64, kipc_perm_t *kperm)
{
	perm64->ipcx_uid = kperm->ipc_uid;
	perm64->ipcx_gid = kperm->ipc_gid;
	perm64->ipcx_cuid = kperm->ipc_cuid;
	perm64->ipcx_cgid = kperm->ipc_cgid;
	perm64->ipcx_mode = kperm->ipc_mode;
	perm64->ipcx_key = kperm->ipc_key;
	perm64->ipcx_projid = kperm->ipc_proj->kpj_id;
	perm64->ipcx_zoneid = kperm->ipc_zoneid;
}


/*
 * ipc key comparator.
 */
static int
ipc_key_compar(const void *a, const void *b)
{
	kipc_perm_t *aperm = (kipc_perm_t *)a;
	kipc_perm_t *bperm = (kipc_perm_t *)b;
	int ak = aperm->ipc_key;
	int bk = bperm->ipc_key;
	zoneid_t az;
	zoneid_t bz;

	ASSERT(ak != IPC_PRIVATE);
	ASSERT(bk != IPC_PRIVATE);

	/*
	 * Compare key first, then zoneid.  This optimizes performance for
	 * systems with only one zone, since the zone checks will only be
	 * made when the keys match.
	 */
	if (ak < bk)
		return (-1);
	if (ak > bk)
		return (1);

	/* keys match */
	az = aperm->ipc_zoneid;
	bz = bperm->ipc_zoneid;
	if (az < bz)
		return (-1);
	if (az > bz)
		return (1);
	return (0);
}

/*
 * Create an ipc service.
 */
ipc_service_t *
ipcs_create(const char *name, rctl_hndl_t proj_rctl, rctl_hndl_t zone_rctl,
    size_t size, ipc_func_t *dtor, ipc_func_t *rmid, int audit_type,
    size_t rctl_offset)
{
	ipc_service_t *result;

	result = kmem_alloc(sizeof (ipc_service_t), KM_SLEEP);

	mutex_init(&result->ipcs_lock, NULL, MUTEX_ADAPTIVE, NULL);
	result->ipcs_count = 0;
	avl_create(&result->ipcs_keys, ipc_key_compar, size, 0);
	result->ipcs_tabsz = IPC_IDS_MIN;
	result->ipcs_table =
	    kmem_zalloc(IPC_IDS_MIN * sizeof (ipc_slot_t), KM_SLEEP);
	result->ipcs_ssize = size;
	result->ipcs_ids = id_space_create(name, 0, IPC_IDS_MIN);
	result->ipcs_dtor = dtor;
	result->ipcs_rmid = rmid;
	result->ipcs_proj_rctl = proj_rctl;
	result->ipcs_zone_rctl = zone_rctl;
	result->ipcs_atype = audit_type;
	ASSERT(rctl_offset < sizeof (ipc_rqty_t));
	result->ipcs_rctlofs = rctl_offset;
	list_create(&result->ipcs_usedids, sizeof (kipc_perm_t),
	    offsetof(kipc_perm_t, ipc_list));

	return (result);
}

/*
 * Destroy an ipc service.
 */
void
ipcs_destroy(ipc_service_t *service)
{
	ipc_slot_t *slot, *next;

	mutex_enter(&service->ipcs_lock);

	ASSERT(service->ipcs_count == 0);
	avl_destroy(&service->ipcs_keys);
	list_destroy(&service->ipcs_usedids);
	id_space_destroy(service->ipcs_ids);

	for (slot = service->ipcs_table; slot; slot = next) {
		next = slot[0].ipct_chain;
		kmem_free(slot, service->ipcs_tabsz * sizeof (ipc_slot_t));
		service->ipcs_tabsz >>= 1;
	}

	mutex_destroy(&service->ipcs_lock);
	kmem_free(service, sizeof (ipc_service_t));
}

/*
 * Takes the service lock.
 */
void
ipcs_lock(ipc_service_t *service)
{
	mutex_enter(&service->ipcs_lock);
}

/*
 * Releases the service lock.
 */
void
ipcs_unlock(ipc_service_t *service)
{
	mutex_exit(&service->ipcs_lock);
}


/*
 * Locks the specified ID.  Returns the ID's ID table index.
 */
static int
ipc_lock_internal(ipc_service_t *service, uint_t id)
{
	uint_t	tabsz;
	uint_t	index;
	kmutex_t *mutex;

	for (;;) {
		tabsz = service->ipcs_tabsz;
		membar_consumer();
		index = id & (tabsz - 1);
		mutex = &service->ipcs_table[index].ipct_lock;
		mutex_enter(mutex);
		if (tabsz == service->ipcs_tabsz)
			break;
		mutex_exit(mutex);
	}

	return (index);
}

/*
 * Locks the specified ID.  Returns a pointer to the ID's lock.
 */
kmutex_t *
ipc_lock(ipc_service_t *service, int id)
{
	uint_t index;

	/*
	 * These assertions don't reflect requirements of the code
	 * which follows, but they should never fail nonetheless.
	 */
	ASSERT(id >= 0);
	ASSERT(IPC_INDEX(id) < service->ipcs_tabsz);
	index = ipc_lock_internal(service, id);

	return (&service->ipcs_table[index].ipct_lock);
}

/*
 * Checks to see if the held lock provided is the current lock for the
 * specified id.  If so, we return it instead of dropping it and
 * returning the result of ipc_lock.  This is intended to speed up cv
 * wakeups where we are left holding a lock which could be stale, but
 * probably isn't.
 */
kmutex_t *
ipc_relock(ipc_service_t *service, int id, kmutex_t *lock)
{
	ASSERT(id >= 0);
	ASSERT(IPC_INDEX(id) < service->ipcs_tabsz);
	ASSERT(MUTEX_HELD(lock));

	if (&service->ipcs_table[IPC_INDEX(id)].ipct_lock == lock)
		return (lock);

	mutex_exit(lock);
	return (ipc_lock(service, id));
}

/*
 * Performs an ID lookup.  If the ID doesn't exist or has been removed,
 * or isn't visible to the caller (because of zones), NULL is returned.
 * Otherwise, a pointer to the ID's perm structure and held ID lock are
 * returned.
 */
kmutex_t *
ipc_lookup(ipc_service_t *service, int id, kipc_perm_t **perm)
{
	kipc_perm_t *result;
	uint_t index;

	/*
	 * There is no need to check to see if id is in-range (i.e.
	 * positive and fits into the table).  If it is out-of-range,
	 * the id simply won't match the object's.
	 */

	index = ipc_lock_internal(service, id);
	result = service->ipcs_table[index].ipct_data;
	if (result == NULL || result->ipc_id != (uint_t)id ||
	    !HASZONEACCESS(curproc, result->ipc_zoneid)) {
		mutex_exit(&service->ipcs_table[index].ipct_lock);
		return (NULL);
	}

	ASSERT(IPC_SEQ(id) == service->ipcs_table[index].ipct_seq);

	*perm = result;
	if (AU_AUDITING())
		audit_ipc(service->ipcs_atype, id, result);

	return (&service->ipcs_table[index].ipct_lock);
}

/*
 * Increase the reference count on an ID.
 */
/*ARGSUSED*/
void
ipc_hold(ipc_service_t *s, kipc_perm_t *perm)
{
	ASSERT(IPC_INDEX(perm->ipc_id) < s->ipcs_tabsz);
	ASSERT(IPC_LOCKED(s, perm));
	perm->ipc_ref++;
}

/*
 * Decrease the reference count on an ID and drops the ID's lock.
 * Destroys the ID if the new reference count is zero.
 */
void
ipc_rele(ipc_service_t *s, kipc_perm_t *perm)
{
	int nref;

	ASSERT(IPC_INDEX(perm->ipc_id) < s->ipcs_tabsz);
	ASSERT(IPC_LOCKED(s, perm));
	ASSERT(perm->ipc_ref > 0);

	nref = --perm->ipc_ref;
	mutex_exit(&s->ipcs_table[IPC_INDEX(perm->ipc_id)].ipct_lock);

	if (nref == 0) {
		ASSERT(IPC_FREE(perm));		/* ipc_rmid clears IPC_ALLOC */
		s->ipcs_dtor(perm);
		project_rele(perm->ipc_proj);
		zone_rele_ref(&perm->ipc_zone_ref, ZONE_REF_IPC);
		kmem_free(perm, s->ipcs_ssize);
	}
}

/*
 * Decrease the reference count on an ID, but don't drop the ID lock.
 * Used in cases where one thread needs to remove many references (on
 * behalf of other parties).
 */
void
ipc_rele_locked(ipc_service_t *s, kipc_perm_t *perm)
{
	ASSERT(perm->ipc_ref > 1);
	ASSERT(IPC_INDEX(perm->ipc_id) < s->ipcs_tabsz);
	ASSERT(IPC_LOCKED(s, perm));

	perm->ipc_ref--;
}


/*
 * Internal function to grow the service ID table.
 */
static int
ipc_grow(ipc_service_t *service)
{
	ipc_slot_t *new, *old;
	int i, oldsize, newsize;

	ASSERT(MUTEX_HELD(&service->ipcs_lock));
	ASSERT(MUTEX_NOT_HELD(&curproc->p_lock));

	if (service->ipcs_tabsz == IPC_IDS_MAX)
		return (ENOSPC);

	oldsize = service->ipcs_tabsz;
	newsize = oldsize << 1;
	new = kmem_zalloc(newsize * sizeof (ipc_slot_t), KM_NOSLEEP);
	if (new == NULL)
		return (ENOSPC);

	old = service->ipcs_table;
	for (i = 0; i < oldsize; i++) {
		mutex_enter(&old[i].ipct_lock);
		mutex_enter(&new[i].ipct_lock);

		new[i].ipct_seq = old[i].ipct_seq;
		new[i].ipct_data = old[i].ipct_data;
		old[i].ipct_data = NULL;
	}

	new[0].ipct_chain = old;
	service->ipcs_table = new;
	membar_producer();
	service->ipcs_tabsz = newsize;

	for (i = 0; i < oldsize; i++) {
		mutex_exit(&old[i].ipct_lock);
		mutex_exit(&new[i].ipct_lock);
	}

	id_space_extend(service->ipcs_ids, oldsize, service->ipcs_tabsz);

	return (0);
}


static int
ipc_keylookup(ipc_service_t *service, key_t key, int flag, kipc_perm_t **permp)
{
	kipc_perm_t *perm = NULL;
	avl_index_t where;
	kipc_perm_t template;

	ASSERT(MUTEX_HELD(&service->ipcs_lock));

	template.ipc_key = key;
	template.ipc_zoneid = getzoneid();
	if (perm = avl_find(&service->ipcs_keys, &template, &where)) {
		ASSERT(!IPC_FREE(perm));
		if ((flag & (IPC_CREAT | IPC_EXCL)) == (IPC_CREAT | IPC_EXCL))
			return (EEXIST);
		if ((flag & 0777) & ~perm->ipc_mode) {
			if (AU_AUDITING())
				audit_ipcget(0, (void *)perm);
			return (EACCES);
		}
		*permp = perm;
		return (0);
	} else if (flag & IPC_CREAT) {
		*permp = NULL;
		return (0);
	}
	return (ENOENT);
}

static int
ipc_alloc_test(ipc_service_t *service, proc_t *pp)
{
	ASSERT(MUTEX_HELD(&service->ipcs_lock));

	/*
	 * Resizing the table first would result in a cleaner code
	 * path, but would also allow a user to (permanently) double
	 * the id table size in cases where the allocation would be
	 * denied.  Hence we test the rctl first.
	 */
retry:
	mutex_enter(&pp->p_lock);
	if ((rctl_test(service->ipcs_proj_rctl, pp->p_task->tk_proj->kpj_rctls,
	    pp, 1, RCA_SAFE) & RCT_DENY) ||
	    (rctl_test(service->ipcs_zone_rctl, pp->p_zone->zone_rctls,
	    pp, 1, RCA_SAFE) & RCT_DENY)) {
		mutex_exit(&pp->p_lock);
		return (ENOSPC);
	}

	if (service->ipcs_count == service->ipcs_tabsz) {
		int error;

		mutex_exit(&pp->p_lock);
		if (error = ipc_grow(service))
			return (error);
		goto retry;
	}

	return (0);
}

/*
 * Given a key, search for or create the associated identifier.
 *
 * If IPC_CREAT is specified and the key isn't found, or if the key is
 * equal to IPC_PRIVATE, we return 0 and place a pointer to a newly
 * allocated object structure in permp.  A pointer to the held service
 * lock is placed in lockp.  ipc_mode's IPC_ALLOC bit is clear.
 *
 * If the key is found and no error conditions arise, we return 0 and
 * place a pointer to the existing object structure in permp.  A
 * pointer to the held ID lock is placed in lockp.  ipc_mode's
 * IPC_ALLOC bit is set.
 *
 * Otherwise, a non-zero errno value is returned.
 */
int
ipc_get(ipc_service_t *service, key_t key, int flag, kipc_perm_t **permp,
    kmutex_t **lockp)
{
	kipc_perm_t	*perm = NULL;
	proc_t		*pp = curproc;
	int		error, index;
	cred_t		*cr = CRED();

	if (key != IPC_PRIVATE) {

		mutex_enter(&service->ipcs_lock);
		error = ipc_keylookup(service, key, flag, &perm);
		if (perm != NULL)
			index = ipc_lock_internal(service, perm->ipc_id);
		mutex_exit(&service->ipcs_lock);

		if (error) {
			ASSERT(perm == NULL);
			return (error);
		}

		if (perm) {
			ASSERT(!IPC_FREE(perm));
			*permp = perm;
			*lockp = &service->ipcs_table[index].ipct_lock;
			return (0);
		}

		/* Key not found; fall through */
	}

	perm = kmem_zalloc(service->ipcs_ssize, KM_SLEEP);

	mutex_enter(&service->ipcs_lock);
	if (error = ipc_alloc_test(service, pp)) {
		mutex_exit(&service->ipcs_lock);
		kmem_free(perm, service->ipcs_ssize);
		return (error);
	}

	perm->ipc_cuid = perm->ipc_uid = crgetuid(cr);
	perm->ipc_cgid = perm->ipc_gid = crgetgid(cr);
	perm->ipc_zoneid = getzoneid();
	perm->ipc_mode = flag & 0777;
	perm->ipc_key = key;
	perm->ipc_ref = 1;
	perm->ipc_id = IPC_ID_INVAL;
	*permp = perm;
	*lockp = &service->ipcs_lock;

	return (0);
}

/*
 * Attempts to add the a newly created ID to the global namespace.  If
 * creating it would cause an error, we return the error.  If there is
 * the possibility that we could obtain the existing ID and return it
 * to the user, we return EAGAIN.  Otherwise, we return 0 with p_lock
 * and the service lock held.
 *
 * Since this should be only called after all initialization has been
 * completed, on failure we automatically invoke the destructor for the
 * object and deallocate the memory associated with it.
 */
int
ipc_commit_begin(ipc_service_t *service, key_t key, int flag,
    kipc_perm_t *newperm)
{
	kipc_perm_t *perm;
	int error;
	proc_t *pp = curproc;

	ASSERT(newperm->ipc_ref == 1);
	ASSERT(IPC_FREE(newperm));

	/*
	 * Set ipc_proj and ipc_zone_ref so that future calls to ipc_cleanup()
	 * clean up the necessary state.  This must be done before the
	 * potential call to ipcs_dtor() below.
	 */
	newperm->ipc_proj = pp->p_task->tk_proj;
	zone_init_ref(&newperm->ipc_zone_ref);
	zone_hold_ref(pp->p_zone, &newperm->ipc_zone_ref, ZONE_REF_IPC);

	mutex_enter(&service->ipcs_lock);
	/*
	 * Ensure that no-one has raced with us and created the key.
	 */
	if ((key != IPC_PRIVATE) &&
	    (((error = ipc_keylookup(service, key, flag, &perm)) != 0) ||
	    (perm != NULL))) {
		error = error ? error : EAGAIN;
		goto errout;
	}

	/*
	 * Ensure that no-one has raced with us and used the last of
	 * the permissible ids, or the last of the free spaces in the
	 * id table.
	 */
	if (error = ipc_alloc_test(service, pp))
		goto errout;

	ASSERT(MUTEX_HELD(&service->ipcs_lock));
	ASSERT(MUTEX_HELD(&pp->p_lock));

	return (0);
errout:
	mutex_exit(&service->ipcs_lock);
	service->ipcs_dtor(newperm);
	zone_rele_ref(&newperm->ipc_zone_ref, ZONE_REF_IPC);
	kmem_free(newperm, service->ipcs_ssize);
	return (error);
}

/*
 * Commit the ID allocation transaction.  Called with p_lock and the
 * service lock held, both of which are dropped.  Returns the held ID
 * lock so the caller can extract the ID and perform ipcget auditing.
 */
kmutex_t *
ipc_commit_end(ipc_service_t *service, kipc_perm_t *perm)
{
	ipc_slot_t *slot;
	avl_index_t where;
	int index;
	void *loc;

	ASSERT(MUTEX_HELD(&service->ipcs_lock));
	ASSERT(MUTEX_HELD(&curproc->p_lock));

	(void) project_hold(perm->ipc_proj);
	mutex_exit(&curproc->p_lock);

	/*
	 * Pick out our slot.
	 */
	service->ipcs_count++;
	index = id_alloc(service->ipcs_ids);
	ASSERT(index < service->ipcs_tabsz);
	slot = &service->ipcs_table[index];
	mutex_enter(&slot->ipct_lock);
	ASSERT(slot->ipct_data == NULL);

	/*
	 * Update the perm structure.
	 */
	perm->ipc_mode |= IPC_ALLOC;
	perm->ipc_id = (slot->ipct_seq << IPC_SEQ_SHIFT) | index;

	/*
	 * Push into global visibility.
	 */
	slot->ipct_data = perm;
	if (perm->ipc_key != IPC_PRIVATE) {
		loc = avl_find(&service->ipcs_keys, perm, &where);
		ASSERT(loc == NULL);
		avl_insert(&service->ipcs_keys, perm, where);
	}
	list_insert_head(&service->ipcs_usedids, perm);

	/*
	 * Update resource consumption.
	 */
	IPC_PROJ_USAGE(perm, service) += 1;
	IPC_ZONE_USAGE(perm, service) += 1;

	mutex_exit(&service->ipcs_lock);
	return (&slot->ipct_lock);
}

/*
 * Clean up function, in case the allocation fails.  If called between
 * ipc_lookup and ipc_commit_begin, perm->ipc_proj will be 0 and we
 * merely free the perm structure.  If called after ipc_commit_begin,
 * we also drop locks and call the ID's destructor.
 */
void
ipc_cleanup(ipc_service_t *service, kipc_perm_t *perm)
{
	ASSERT(IPC_FREE(perm));
	if (perm->ipc_proj) {
		mutex_exit(&curproc->p_lock);
		mutex_exit(&service->ipcs_lock);
		service->ipcs_dtor(perm);
	}
	if (perm->ipc_zone_ref.zref_zone != NULL)
		zone_rele_ref(&perm->ipc_zone_ref, ZONE_REF_IPC);
	kmem_free(perm, service->ipcs_ssize);
}


/*
 * Common code to remove an IPC object.  This should be called after
 * all permissions checks have been performed, and with the service
 * and ID locked.  Note that this does not remove the object from
 * the ipcs_usedids list (this needs to be done by the caller before
 * dropping the service lock).
 */
static void
ipc_remove(ipc_service_t *service, kipc_perm_t *perm)
{
	int id = perm->ipc_id;
	int index;

	ASSERT(MUTEX_HELD(&service->ipcs_lock));
	ASSERT(IPC_LOCKED(service, perm));

	index = IPC_INDEX(id);

	service->ipcs_table[index].ipct_data = NULL;

	if (perm->ipc_key != IPC_PRIVATE)
		avl_remove(&service->ipcs_keys, perm);
	list_remove(&service->ipcs_usedids, perm);
	perm->ipc_mode &= ~IPC_ALLOC;

	id_free(service->ipcs_ids, index);

	if (service->ipcs_table[index].ipct_seq++ == IPC_SEQ_MASK)
		service->ipcs_table[index].ipct_seq = 0;
	service->ipcs_count--;
	ASSERT(IPC_PROJ_USAGE(perm, service) > 0);
	ASSERT(IPC_ZONE_USAGE(perm, service) > 0);
	IPC_PROJ_USAGE(perm, service) -= 1;
	IPC_ZONE_USAGE(perm, service) -= 1;
	ASSERT(service->ipcs_count || ((IPC_PROJ_USAGE(perm, service) == 0) &&
	    (IPC_ZONE_USAGE(perm, service) == 0)));
}

/*
 * Perform actual IPC_RMID, either via ipc_rmid or due to a delayed *_RMID.
 */
void
ipc_rmsvc(ipc_service_t *service, kipc_perm_t *perm)
{
	ASSERT(service->ipcs_count > 0);
	ASSERT(MUTEX_HELD(&service->ipcs_lock));

	ipc_remove(service, perm);
	mutex_exit(&service->ipcs_lock);

	/* perform any per-service removal actions */
	service->ipcs_rmid(perm);

	ipc_rele(service, perm);
}

/*
 * Common code to perform an IPC_RMID.  Returns an errno value on
 * failure, 0 on success.
 */
int
ipc_rmid(ipc_service_t *service, int id, cred_t *cr)
{
	kipc_perm_t *perm;
	kmutex_t *lock;

	mutex_enter(&service->ipcs_lock);

	lock = ipc_lookup(service, id, &perm);
	if (lock == NULL) {
		mutex_exit(&service->ipcs_lock);
		return (EINVAL);
	}

	ASSERT(service->ipcs_count > 0);

	if (secpolicy_ipc_owner(cr, perm) != 0) {
		mutex_exit(lock);
		mutex_exit(&service->ipcs_lock);
		return (EPERM);
	}

	/*
	 * Nothing can fail from this point on.
	 */
	ipc_rmsvc(service, perm);

	return (0);
}

/*
 * Implementation for shmids, semids, and msgids.  buf is the address
 * of the user buffer, nids is the size, and pnids is a pointer to
 * where we write the actual number of ids that [would] have been
 * copied out.
 */
int
ipc_ids(ipc_service_t *service, int *buf, uint_t nids, uint_t *pnids)
{
	kipc_perm_t *perm;
	size_t	idsize = 0;
	int	error = 0;
	int	idcount;
	int	*ids = NULL;
	int	numids = 0;
	zoneid_t zoneid = getzoneid();
	int	global = INGLOBALZONE(curproc);

	if (buf == NULL)
		nids = 0;

	/*
	 * Get an accurate count of the total number of ids, and allocate a
	 * staging buffer.  Since ipcs_count is always sane, we don't have
	 * to take ipcs_lock for our first guess.  If there are no ids, or
	 * we're in the global zone and the number of ids is greater than
	 * the size of the specified buffer, we shunt to the end.  Otherwise,
	 * we go through the id list looking for (and counting) what is
	 * visible in the specified zone.
	 */
	idcount = service->ipcs_count;
	for (;;) {
		if ((global && idcount > nids) || idcount == 0) {
			numids = idcount;
			nids = 0;
			goto out;
		}

		idsize = idcount * sizeof (int);
		ids = kmem_alloc(idsize, KM_SLEEP);

		mutex_enter(&service->ipcs_lock);
		if (idcount >= service->ipcs_count)
			break;
		idcount = service->ipcs_count;
		mutex_exit(&service->ipcs_lock);

		if (idsize != 0) {
			kmem_free(ids, idsize);
			ids = NULL;
			idsize = 0;
		}
	}

	for (perm = list_head(&service->ipcs_usedids); perm != NULL;
	    perm = list_next(&service->ipcs_usedids, perm)) {
		ASSERT(!IPC_FREE(perm));
		if (global || perm->ipc_zoneid == zoneid)
			ids[numids++] = perm->ipc_id;
	}
	mutex_exit(&service->ipcs_lock);

	/*
	 * If there isn't enough space to hold all of the ids, just
	 * return the number of ids without copying out any of them.
	 */
	if (nids < numids)
		nids = 0;

out:
	if (suword32(pnids, (uint32_t)numids) ||
	    (nids != 0 && copyout(ids, buf, numids * sizeof (int))))
		error = EFAULT;
	if (ids != NULL)
		kmem_free(ids, idsize);
	return (error);
}

/*
 * Destroy IPC objects from the given service that are associated with
 * the given zone.
 *
 * We can't hold on to the service lock when freeing objects, so we
 * first search the service and move all the objects to a private
 * list, then walk through and free them after dropping the lock.
 */
void
ipc_remove_zone(ipc_service_t *service, zoneid_t zoneid)
{
	kipc_perm_t *perm, *next;
	list_t rmlist;
	kmutex_t *lock;

	list_create(&rmlist, sizeof (kipc_perm_t),
	    offsetof(kipc_perm_t, ipc_list));

	mutex_enter(&service->ipcs_lock);
	for (perm = list_head(&service->ipcs_usedids); perm != NULL;
	    perm = next) {
		next = list_next(&service->ipcs_usedids, perm);
		if (perm->ipc_zoneid != zoneid)
			continue;

		/*
		 * Remove the object from the service, then put it on
		 * the removal list so we can defer the call to
		 * ipc_rele (which will actually free the structure).
		 * We need to do this since the destructor may grab
		 * the service lock.
		 */
		ASSERT(!IPC_FREE(perm));
		lock = ipc_lock(service, perm->ipc_id);
		ipc_remove(service, perm);
		mutex_exit(lock);
		list_insert_tail(&rmlist, perm);
	}
	mutex_exit(&service->ipcs_lock);

	/*
	 * Now that we've dropped the service lock, loop through the
	 * private list freeing removed objects.
	 */
	for (perm = list_head(&rmlist); perm != NULL; perm = next) {
		next = list_next(&rmlist, perm);
		list_remove(&rmlist, perm);

		(void) ipc_lock(service, perm->ipc_id);

		/* perform any per-service removal actions */
		service->ipcs_rmid(perm);

		/* release reference */
		ipc_rele(service, perm);
	}

	list_destroy(&rmlist);
}



/* ============================================================================
 * SOURCE 6/8: minix4\exokernel\kernel_legacy\cmd\truss\ipc.c
 * Size: 9,148 bytes, Lines: 418
 * Hash: 2897c34a09b7...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2010 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <stropts.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/termio.h>
#include <libproc.h>
#include "ramdata.h"
#include "proto.h"

/*
 * Routines related to interprocess communication
 * among the truss processes which are controlling
 * multiple traced processes.
 */

/*
 * Function prototypes for static routines in this module.
 */
void	Ecritical(int);
void	Xcritical(int);

/*
 * Ensure everyone keeps out of each other's way
 * while writing lines of trace output.
 */
void
Flush()
{
	/*
	 * Except for regions bounded by Eserialize()/Xserialize(),
	 * this is the only place anywhere in the program where a
	 * write() to the trace output file takes place, so here
	 * is where we detect errors writing to the output.
	 */

	errno = 0;

	Ecritical(0);
	(void) fflush(stdout);
	Xcritical(0);

	if (ferror(stdout) && errno)	/* error on write(), probably EPIPE */
		interrupt = SIGTERM;		/* post an interrupt */
}

/*
 * Eserialize() and Xserialize() are used to bracket
 * a region which may produce large amounts of output,
 * such as showargs()/dumpargs().
 */

void
Eserialize()
{
	/* serialize output */
	Ecritical(0);
}

void
Xserialize()
{
	(void) fflush(stdout);
	Xcritical(0);
}

/*
 * Enter critical region --- Wait on mutex, lock out other processes.
 * Lock zero is used to serialize output in situations where multiple processes
 * may be writing to stdout/stderr and order must be preserved.  Most of these
 * are in expound.c
 * Lock one is used to protect the table of processes currently being traced
 * every time a pid is added or removed from the table Ecritical(1)/Xcritical(1)
 * get called.
 */
void
Ecritical(int num)
{
	int rv;

	if (num == 0)
		rv = mutex_lock(&gps->ps_mutex0);
	else if (num == 1)
		rv = mutex_lock(&gps->ps_mutex1);
	else
		abend("Invalid mutex specified", NULL);

	if (rv != 0) {
		char mnum[2];
		mnum[0] = '0' + num;
		mnum[1] = '\0';
		errno = rv;
		perror(command);
		errmsg("cannot grab mutex #", mnum);
	}
}

/*
 * Exit critical region ---
 * Release other processes waiting on mutex.
 */
void
Xcritical(int num)
{
	int rv;

	if (num == 0)
		rv = mutex_unlock(&gps->ps_mutex0);
	else if (num == 1)
		rv = mutex_unlock(&gps->ps_mutex1);
	else
		abend("Invalid mutex specified", NULL);


	if (rv != 0) {
		char mnum[2];
		mnum[0] = '0' + num;
		mnum[1] = '\0';
		errno = rv;
		perror(command);
		errmsg("cannot release mutex #", mnum);
	}
}

/*
 * Add process to set of those being traced.
 */
void
procadd(pid_t spid, const char *lwplist)
{
	int i;
	int j = -1;

	if (gps == NULL)
		return;

	Ecritical(1);
	for (i = 0; i < sizeof (gps->tpid) / sizeof (gps->tpid[0]); i++) {
		if (gps->tpid[i] == 0) {
			if (j == -1)	/* remember first vacant slot */
				j = i;
			if (gps->spid[i] == 0)	/* this slot is better */
				break;
		}
	}
	if (i < sizeof (gps->tpid) / sizeof (gps->tpid[0]))
		j = i;
	if (j >= 0) {
		gps->tpid[j] = getpid();
		gps->spid[j] = spid;
		gps->lwps[j] = lwplist;
	}
	Xcritical(1);
}

/*
 * Delete process from set of those being traced.
 */
void
procdel()
{
	int i;
	pid_t tpid;

	if (gps == NULL)
		return;

	tpid = getpid();

	Ecritical(1);
	for (i = 0; i < sizeof (gps->tpid) / sizeof (gps->tpid[0]); i++) {
		if (gps->tpid[i] == tpid) {
			gps->tpid[i] = 0;
			break;
		}
	}
	Xcritical(1);
}

/*
 * Determine if the lwp for this process should be traced.
 */
int
lwptrace(pid_t spid, lwpid_t lwpid)
{
	int i;
	pid_t tpid;
	const char *lwps;

	if (gps == NULL)
		return (0);

	tpid = getpid();

	Ecritical(1);
	for (i = 0; i < sizeof (gps->tpid) / sizeof (gps->tpid[0]); i++) {
		if (gps->tpid[i] == tpid &&
		    gps->spid[i] == spid)
			break;
	}
	lwps = gps->lwps[i];
	Xcritical(1);

	return (proc_lwp_in_set(lwps, lwpid));
}

/*
 * Check for open of a /proc/nnnnn file.
 * Return 0 if this is not an open of a /proc file.
 * Return 1 if the process opened itself.
 * Return 2 if the process failed to open another process
 * in truss's set of controlled processes.
 * Return 3 if the process successfully opened another process
 * in truss's set of controlled processes.
 * We notify and wait for the other controlling truss process
 * to terminate before returning in cases 2 and 3.
 */
/* ARGSUSED */
int
checkproc(private_t *pri)
{
	char *path = pri->sys_path;
	const pstatus_t *Psp = Pstatus(Proc);
	struct ps_lwphandle *Lwp = pri->Lwp;
	const lwpstatus_t *Lsp = pri->lwpstat;
	int what = Lsp->pr_what;	/* one of the SYS_open* syscalls */
	int err = Lsp->pr_errno;
	int pid;
	int i;
	const char *dirname;
	char *next;
	char *sp1;
	char *sp2;
	prgreg_t pc;

	/*
	 * A bit heuristic ...
	 * Test for the cases:
	 *	1234
	 *	1234/as
	 *	1234/ctl
	 *	1234/lwp/24/lwpctl
	 *	.../1234
	 *	.../1234/as
	 *	.../1234/ctl
	 *	.../1234/lwp/24/lwpctl
	 * Insert a '\0', if necessary, so the path becomes ".../1234".
	 *
	 * Along the way, watch out for /proc/self and /proc/1234/lwp/agent
	 */
	if ((sp1 = strrchr(path, '/')) == NULL)		/* last component */
		/* EMPTY */;
	else if (isdigit(*(sp1+1))) {
		sp1 += strlen(sp1);
		while (--sp1 > path && isdigit(*sp1))
			;
		if (*sp1 != '/')
			return (0);
	} else if (strcmp(sp1+1, "as") == 0 ||
	    strcmp(sp1+1, "ctl") == 0) {
		*sp1 = '\0';
	} else if (strcmp(sp1+1, "lwpctl") == 0) {
		/*
		 * .../1234/lwp/24/lwpctl
		 * ............   ^-- sp1
		 */
		if (sp1-6 >= path && strncmp(sp1-6, "/agent", 6) == 0)
			sp1 -= 6;
		else {
			while (--sp1 > path && isdigit(*sp1))
				;
		}
		if (*sp1 != '/' ||
		    (sp1 -= 4) <= path ||
		    strncmp(sp1, "/lwp", 4) != 0)
			return (0);
		*sp1 = '\0';
	} else if (strcmp(sp1+1, "self") != 0) {
		return (0);
	}

	if ((sp2 = strrchr(path, '/')) == NULL)
		dirname = path;
	else
		dirname = sp2 + 1;

	if (strcmp(dirname, "self") == 0) {
		pid = Psp->pr_pid;
	} else if ((pid = strtol(dirname, &next, 10)) < 0 ||
	    *next != '\0') {	/* dirname not a number */
		if (sp1 != NULL)
			*sp1 = '/';
		return (0);
	}
	if (sp2 == NULL)
		dirname = ".";
	else {
		*sp2 = '\0';
		dirname = path;
	}

	if (!Pisprocdir(Proc, dirname) || /* file not in a /proc directory */
	    pid == getpid() ||		/* process opened truss's /proc file */
	    pid == 0) {			/* process opened process 0 */
		if (sp1 != NULL)
			*sp1 = '/';
		if (sp2 != NULL)
			*sp2 = '/';
		return (0);
	}
	if (sp1 != NULL)
		*sp1 = '/';
	if (sp2 != NULL)
		*sp2 = '/';

	/*
	 * Process did open a /proc file ---
	 */
	if (pid == Psp->pr_pid) {	/* process opened its own /proc file */
		/*
		 * In SunOS 5.6 and beyond, self-opens always succeed.
		 */
		return (1);
	}

	/*
	 * Search for a matching pid in our set of controlled processes.
	 */
	for (i = 0; i < sizeof (gps->tpid)/sizeof (gps->tpid[0]); i++) {
		if (gps->spid[i] == pid) {
			pid = gps->tpid[i];
			break;
		}
	}
	if (i >= sizeof (gps->tpid) / sizeof (gps->tpid[0])) {
		/*
		 * The process opened a /proc file, but not one we care about.
		 */
		return (0);
	}

	/*
	 * Notify and wait for the controlling process to terminate.
	 */
	while (pid && gps->tpid[i] == pid) {
		if (kill(pid, SIGUSR1) == -1)
			break;
		(void) usleep(1000000);
	}
	Ecritical(1);
	if (gps->tpid[i] == 0)
		gps->spid[i] = 0;
	Xcritical(1);

	if (err) {	/* prepare to reissue the failed open() system call */
#if defined(__sparc)
		(void) Lgetareg(Lwp, R_PC, &pc);
		if (pri->sys_indirect) {
			(void) Lputareg(Lwp, R_G1, (prgreg_t)SYS_syscall);
			(void) Lputareg(Lwp, R_O0, (prgreg_t)what);
			for (i = 0; i < 5; i++)
				(void) Lputareg(Lwp, R_O1+i, pri->sys_args[i]);
		} else {
			(void) Lputareg(Lwp, R_G1, (prgreg_t)what);
			for (i = 0; i < 6; i++)
				(void) Lputareg(Lwp, R_O0+i, pri->sys_args[i]);
		}
		(void) Lputareg(Lwp, R_nPC, pc);
#elif defined(__amd64)
		(void) Lgetareg(Lwp, R_PC, &pc);
		(void) Lputareg(Lwp, REG_RAX, (prgreg_t)what);
#elif defined(__i386)
		(void) Lgetareg(Lwp, R_PC, &pc);
		(void) Lputareg(Lwp, EAX, (prgreg_t)what);
#else
#error "unrecognized architecture"
#endif
		(void) Pissyscall_prev(Proc, pc, (uintptr_t *)&pc);
		(void) Lputareg(Lwp, R_PC, pc);
		return (2);
	}

	return (3);
}



/* ============================================================================
 * SOURCE 7/8: minix4\exokernel\kernel_legacy\cmd\mdb\common\modules\ipc\ipc.c
 * Size: 18,375 bytes, Lines: 744
 * Hash: a5dbb13e7b1a...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright 2016 Joyent, Inc.
 */

#include <mdb/mdb_modapi.h>
#include <mdb/mdb_ks.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/project.h>
#include <sys/ipc_impl.h>
#include <sys/shm_impl.h>
#include <sys/sem_impl.h>
#include <sys/msg_impl.h>

#include <vm/anon.h>

#define	CMN_HDR_START	"%<u>"
#define	CMN_HDR_END	"%</u>\n"
#define	CMN_INDENT	(4)
#define	CMN_INACTIVE	"%s facility inactive.\n"

/*
 * Bitmap data for page protection flags suitable for use with %b.
 */
const mdb_bitmask_t prot_flag_bits[] = {
	{ "PROT_READ", PROT_READ, PROT_READ },
	{ "PROT_WRITE", PROT_WRITE, PROT_WRITE },
	{ "PROT_EXEC", PROT_EXEC, PROT_EXEC },
	{ "PROT_USER", PROT_USER, PROT_USER },
	{ NULL, 0, 0 }
};

static void
printtime_nice(const char *str, time_t time)
{
	if (time)
		mdb_printf("%s%Y\n", str, time);
	else
		mdb_printf("%sn/a\n", str);
}

/*
 * Print header common to all IPC types.
 */
static void
ipcperm_header()
{
	mdb_printf(CMN_HDR_START "%?s %5s %5s %8s %5s %5s %6s %5s %5s %5s %5s"
	    CMN_HDR_END, "ADDR", "REF", "ID", "KEY", "MODE", "PRJID", "ZONEID",
	    "OWNER", "GROUP", "CREAT", "CGRP");
}

/*
 * Print data common to all IPC types.
 */
static void
ipcperm_print(uintptr_t addr, kipc_perm_t *perm)
{
	kproject_t proj;
	int res;

	res = mdb_vread(&proj, sizeof (kproject_t), (uintptr_t)perm->ipc_proj);

	if (res == -1)
		mdb_warn("failed to read kproject_t at %#p", perm->ipc_proj);

	mdb_printf("%0?p %5d %5d", addr, perm->ipc_ref, perm->ipc_id);
	if (perm->ipc_key)
		mdb_printf(" %8x", perm->ipc_key);
	else
		mdb_printf(" %8s", "private");
	mdb_printf(" %5#o", perm->ipc_mode & 07777);
	if (res == -1)
		mdb_printf(" %5s %5s", "<flt>", "<flt>");
	else
		mdb_printf(" %5d %6d", proj.kpj_id, proj.kpj_zoneid);
	mdb_printf(" %5d %5d %5d %5d\n", perm->ipc_uid, perm->ipc_gid,
	    perm->ipc_cuid, perm->ipc_cgid);

}

/*ARGSUSED*/
static int
ipcperm(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	kipc_perm_t perm;

	if (!(flags & DCMD_ADDRSPEC))
		return (DCMD_USAGE);

	if (DCMD_HDRSPEC(flags))
		ipcperm_header();

	if (mdb_vread(&perm, sizeof (kipc_perm_t), addr) == -1) {
		mdb_warn("failed to read kipc_perm_t at %#lx", addr);
		return (DCMD_ERR);
	}

	ipcperm_print(addr, &perm);
	return (DCMD_OK);
}


#define	MSG_SND_SIZE 0x1
static int
msgq_check_for_waiters(list_t *walk_this, int min, int max,
    int copy_wait, uintptr_t addr, int flag)
{
	int found = 0;
	int ii;
	msgq_wakeup_t *walker, next;
	uintptr_t head;

	for (ii = min; ii < max; ii++) {
		head = ((ulong_t)addr) + sizeof (list_t)*ii +
		    sizeof (list_node_t);
		if (head != (uintptr_t)walk_this[ii].list_head.list_next) {
			walker =
			    (msgq_wakeup_t *)walk_this[ii].list_head.list_next;
			while (head != (uintptr_t)walker) {
				if (mdb_vread(&next, sizeof (msgq_wakeup_t),
				    (uintptr_t)walker) == -1) {
					mdb_warn(
					    "Failed to read message queue\n");
					return (found);
				}

				if (flag & MSG_SND_SIZE) {
					mdb_printf("%15lx\t%6d\t%15lx\t%15d\n",
					    next.msgw_thrd, next.msgw_type,
					    walker + (uintptr_t)
					    OFFSETOF(msgq_wakeup_t,
					    msgw_wake_cv), next.msgw_snd_size);
				} else {
					mdb_printf("%15lx\t%6d\t%15lx\t%15s\n",
					    next.msgw_thrd, next.msgw_type,
					    walker + (uintptr_t)
					    OFFSETOF(msgq_wakeup_t,
					    msgw_wake_cv),
					    (copy_wait ? "yes":"no"));
				}
				found++;
				walker =
				    (msgq_wakeup_t *)next.msgw_list.list_next;
			}
		}
	}
	return (found);
}

static void
msq_print(kmsqid_t *msqid, uintptr_t addr)
{
	int	total = 0;

	mdb_printf("&list: %-?p\n", addr + OFFSETOF(kmsqid_t, msg_list));
	mdb_printf("cbytes: 0t%lu    qnum: 0t%lu    qbytes: 0t%lu"
	    "    qmax: 0t%lu\n", msqid->msg_cbytes, msqid->msg_qnum,
	    msqid->msg_qbytes, msqid->msg_qmax);
	mdb_printf("lspid: 0t%d    lrpid: 0t%d\n",
	    (int)msqid->msg_lspid, (int)msqid->msg_lrpid);
	printtime_nice("stime: ", msqid->msg_stime);
	printtime_nice("rtime: ", msqid->msg_rtime);
	printtime_nice("ctime: ", msqid->msg_ctime);
	mdb_printf("snd_cnt: 0t%lld    snd_cv: %hd (%p)\n",
	    msqid->msg_snd_cnt, msqid->msg_snd_cv._opaque,
	    addr + (uintptr_t)OFFSETOF(kmsqid_t, msg_snd_cv));
	mdb_printf("Blocked recievers\n");
	mdb_printf("%15s\t%6s\t%15s\t%15s\n", "Thread Addr",
	    "Type", "cv addr", "copyout-wait?");
	total += msgq_check_for_waiters(&msqid->msg_cpy_block,
	    0, 1, 1, addr + OFFSETOF(kmsqid_t, msg_cpy_block), 0);
	total += msgq_check_for_waiters(msqid->msg_wait_snd_ngt,
	    0, MSG_MAX_QNUM + 1, 0,
	    addr + OFFSETOF(kmsqid_t, msg_wait_snd_ngt), 0);
	mdb_printf("Blocked senders\n");
	total += msgq_check_for_waiters(&msqid->msg_wait_rcv,
	    0, 1, 1, addr + OFFSETOF(kmsqid_t, msg_wait_rcv),
	    MSG_SND_SIZE);
	mdb_printf("%15s\t%6s\t%15s\t%15s\n", "Thread Addr",
	    "Type", "cv addr", "Msg Size");
	total += msgq_check_for_waiters(msqid->msg_wait_snd,
	    0, MSG_MAX_QNUM + 1, 0, addr + OFFSETOF(kmsqid_t,
	    msg_wait_snd), 0);
	mdb_printf("Total number of waiters: %d\n", total);
}


/*ARGSUSED1*/
static void
shm_print(kshmid_t *shmid, uintptr_t addr)
{
	shmatt_t nattch;

	nattch = shmid->shm_perm.ipc_ref - (IPC_FREE(&shmid->shm_perm) ? 0 : 1);

	mdb_printf(CMN_HDR_START "%10s %?s %5s %7s %7s %7s %7s" CMN_HDR_END,
	    "SEGSZ", "AMP", "LKCNT", "LPID", "CPID", "NATTCH", "CNATTCH");
	mdb_printf("%10#lx %?p %5u %7d %7d %7lu %7lu\n",
	    shmid->shm_segsz, shmid->shm_amp, shmid->shm_lkcnt,
	    (int)shmid->shm_lpid, (int)shmid->shm_cpid, nattch,
	    shmid->shm_ismattch);

	printtime_nice("atime: ", shmid->shm_atime);
	printtime_nice("dtime: ", shmid->shm_dtime);
	printtime_nice("ctime: ", shmid->shm_ctime);
	mdb_printf("sptinfo: %-?p    sptseg: %-?p\n",
	    shmid->shm_sptinfo, shmid->shm_sptseg);
	mdb_printf("opts: rmpend: %d prot: <%b>\n",
	    ((shmid->shm_opts & SHM_RM_PENDING) != 0),
	    (shmid->shm_opts & SHM_PROT_MASK), prot_flag_bits);
}


/*ARGSUSED1*/
static void
sem_print(ksemid_t *semid, uintptr_t addr)
{
	mdb_printf("base: %-?p    nsems: 0t%u\n",
	    semid->sem_base, semid->sem_nsems);
	printtime_nice("otime: ", semid->sem_otime);
	printtime_nice("ctime: ", semid->sem_ctime);
	mdb_printf("binary: %s\n", semid->sem_binary ? "yes" : "no");
}

typedef struct ipc_ops_vec {
	char	*iv_wcmd;	/* walker name		*/
	char	*iv_ocmd;	/* output dcmd		*/
	char	*iv_service;	/* service pointer	*/
	void	(*iv_print)(void *, uintptr_t); /* output callback */
	size_t	iv_idsize;
} ipc_ops_vec_t;

ipc_ops_vec_t msq_ops_vec = {
	"msq",
	"kmsqid",
	"msq_svc",
	(void(*)(void *, uintptr_t))msq_print,
	sizeof (kmsqid_t)
};

ipc_ops_vec_t shm_ops_vec = {
	"shm",
	"kshmid",
	"shm_svc",
	(void(*)(void *, uintptr_t))shm_print,
	sizeof (kshmid_t)
};

ipc_ops_vec_t sem_ops_vec = {
	"sem",
	"ksemid",
	"sem_svc",
	(void(*)(void *, uintptr_t))sem_print,
	sizeof (ksemid_t)
};


/*
 * Generic IPC data structure display code
 */
static int
ds_print(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv,
    ipc_ops_vec_t *iv)
{
	void *iddata;

	if (!(flags & DCMD_ADDRSPEC)) {
		uint_t oflags = 0;

		if (mdb_getopts(argc, argv, 'l', MDB_OPT_SETBITS, 1, &oflags,
		    NULL) != argc)
			return (DCMD_USAGE);

		if (mdb_walk_dcmd(iv->iv_wcmd, oflags ? iv->iv_ocmd : "ipcperm",
		    argc, argv) == -1) {
			mdb_warn("can't walk '%s'", iv->iv_wcmd);
			return (DCMD_ERR);
		}
		return (DCMD_OK);
	}

	iddata = mdb_alloc(iv->iv_idsize, UM_SLEEP | UM_GC);
	if (mdb_vread(iddata, iv->iv_idsize, addr) == -1) {
		mdb_warn("failed to read %s at %#lx", iv->iv_ocmd, addr);
		return (DCMD_ERR);
	}

	if (!DCMD_HDRSPEC(flags) && iv->iv_print)
		mdb_printf("\n");

	if (DCMD_HDRSPEC(flags) || iv->iv_print)
		ipcperm_header();

	ipcperm_print(addr, (struct kipc_perm *)iddata);
	if (iv->iv_print) {
		mdb_inc_indent(CMN_INDENT);
		iv->iv_print(iddata, addr);
		mdb_dec_indent(CMN_INDENT);
	}

	return (DCMD_OK);
}


/*
 * Stubs to call ds_print with the appropriate ops vector
 */
static int
cmd_kshmid(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ds_print(addr, flags, argc, argv, &shm_ops_vec));
}


static int
cmd_kmsqid(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ds_print(addr, flags, argc, argv, &msq_ops_vec));
}

static int
cmd_ksemid(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ds_print(addr, flags, argc, argv, &sem_ops_vec));
}

/*
 * Generic IPC walker
 */

static int
ds_walk_init(mdb_walk_state_t *wsp)
{
	ipc_ops_vec_t	*iv = wsp->walk_arg;

	if (wsp->walk_arg != NULL && wsp->walk_addr != 0)
		mdb_printf("ignoring provided address\n");

	if (wsp->walk_arg)
		if (mdb_readvar(&wsp->walk_addr, iv->iv_service) == -1) {
			mdb_printf("failed to read '%s'; module not present\n",
			    iv->iv_service);
			return (WALK_DONE);
		}
	else
		wsp->walk_addr = wsp->walk_addr +
		    OFFSETOF(ipc_service_t, ipcs_usedids);

	if (mdb_layered_walk("list", wsp) == -1)
		return (WALK_ERR);

	return (WALK_NEXT);
}


static int
ds_walk_step(mdb_walk_state_t *wsp)
{
	return (wsp->walk_callback(wsp->walk_addr, wsp->walk_layer,
	    wsp->walk_cbdata));
}

/*
 * Generic IPC ID/key to pointer code
 */

static int
ipcid_impl(uintptr_t svcptr, uintptr_t id, uintptr_t *addr)
{
	ipc_service_t service;
	kipc_perm_t perm;
	ipc_slot_t slot;
	uintptr_t slotptr;
	uint_t index;

	if (id > INT_MAX) {
		mdb_warn("id out of range\n");
		return (DCMD_ERR);
	}

	if (mdb_vread(&service, sizeof (ipc_service_t), svcptr) == -1) {
		mdb_warn("failed to read ipc_service_t at %#lx", svcptr);
		return (DCMD_ERR);
	}

	index = (uint_t)id & (service.ipcs_tabsz - 1);
	slotptr = (uintptr_t)(service.ipcs_table + index);

	if (mdb_vread(&slot, sizeof (ipc_slot_t), slotptr) == -1) {
		mdb_warn("failed to read ipc_slot_t at %#lx", slotptr);
		return (DCMD_ERR);
	}

	if (slot.ipct_data == NULL)
		return (DCMD_ERR);

	if (mdb_vread(&perm, sizeof (kipc_perm_t),
	    (uintptr_t)slot.ipct_data) == -1) {
		mdb_warn("failed to read kipc_perm_t at %#p",
		    slot.ipct_data);
		return (DCMD_ERR);
	}

	if (perm.ipc_id != (uint_t)id)
		return (DCMD_ERR);

	*addr = (uintptr_t)slot.ipct_data;

	return (DCMD_OK);
}


typedef struct findkey_data {
	key_t fk_key;
	uintptr_t fk_addr;
	boolean_t fk_found;
} findkey_data_t;

static int
findkey(uintptr_t addr, kipc_perm_t *perm, findkey_data_t *arg)
{
	if (perm->ipc_key == arg->fk_key) {
		arg->fk_found = B_TRUE;
		arg->fk_addr = addr;
		return (WALK_DONE);
	}
	return (WALK_NEXT);
}

static int
ipckey_impl(uintptr_t svcptr, uintptr_t key, uintptr_t *addr)
{
	ipc_service_t	service;
	findkey_data_t	fkdata;

	if ((key == IPC_PRIVATE) || (key > INT_MAX)) {
		mdb_warn("key out of range\n");
		return (DCMD_ERR);
	}

	if (mdb_vread(&service, sizeof (ipc_service_t), svcptr) == -1) {
		mdb_warn("failed to read ipc_service_t at %#lx", svcptr);
		return (DCMD_ERR);
	}

	fkdata.fk_key = (key_t)key;
	fkdata.fk_found = B_FALSE;
	if ((mdb_pwalk("avl", (mdb_walk_cb_t)findkey, &fkdata,
	    svcptr + OFFSETOF(ipc_service_t, ipcs_keys)) == -1) ||
	    !fkdata.fk_found)
		return (DCMD_ERR);

	*addr = fkdata.fk_addr;

	return (DCMD_OK);
}

static int
ipckeyid(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv,
    int(*fp)(uintptr_t, uintptr_t, uintptr_t *))
{
	uintmax_t val;
	uintptr_t raddr;
	int result;

	if (!(flags & DCMD_ADDRSPEC) || (argc != 1))
		return (DCMD_USAGE);

	if (argv[0].a_type == MDB_TYPE_IMMEDIATE)
		val = argv[0].a_un.a_val;
	else if (argv[0].a_type == MDB_TYPE_STRING)
		val = mdb_strtoull(argv[0].a_un.a_str);
	else
		return (DCMD_USAGE);

	result = fp(addr, val, &raddr);

	if (result == DCMD_OK)
		mdb_printf("%lx", raddr);

	return (result);
}

static int
ipckey(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ipckeyid(addr, flags, argc, argv, ipckey_impl));
}

static int
ipcid(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ipckeyid(addr, flags, argc, argv, ipcid_impl));
}

static int
ds_ptr(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv,
    ipc_ops_vec_t *iv)
{
	uint_t		kflag = FALSE;
	uintptr_t	svcptr, raddr;
	int		result;

	if (!(flags & DCMD_ADDRSPEC))
		return (DCMD_USAGE);

	if (mdb_getopts(argc, argv,
	    'k', MDB_OPT_SETBITS, TRUE, &kflag, NULL) != argc)
		return (DCMD_USAGE);

	if (mdb_readvar(&svcptr, iv->iv_service) == -1) {
		mdb_warn("failed to read '%s'; module not present\n",
		    iv->iv_service);
		return (DCMD_ERR);
	}

	result = kflag ? ipckey_impl(svcptr, addr, &raddr) :
	    ipcid_impl(svcptr, addr, &raddr);

	if (result == DCMD_OK)
		mdb_printf("%lx", raddr);

	return (result);
}

/*
 * Stubs to call ds_ptr with the appropriate ops vector
 */
static int
id2shm(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ds_ptr(addr, flags, argc, argv, &shm_ops_vec));
}

static int
id2msq(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ds_ptr(addr, flags, argc, argv, &msq_ops_vec));
}

static int
id2sem(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	return (ds_ptr(addr, flags, argc, argv, &sem_ops_vec));
}


/*
 * The message queue contents walker
 */

static int
msg_walk_init(mdb_walk_state_t *wsp)
{
	wsp->walk_addr += OFFSETOF(kmsqid_t, msg_list);
	if (mdb_layered_walk("list", wsp) == -1)
		return (WALK_ERR);

	return (WALK_NEXT);
}

static int
msg_walk_step(mdb_walk_state_t *wsp)
{
	return (wsp->walk_callback(wsp->walk_addr, wsp->walk_layer,
	    wsp->walk_cbdata));
}

/*
 * The "::ipcs" command itself.  Just walks each IPC type in turn.
 */

/*ARGSUSED*/
static int
ipcs(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	uint_t	oflags = 0;

	if ((flags & DCMD_ADDRSPEC) || mdb_getopts(argc, argv, 'l',
	    MDB_OPT_SETBITS, 1, &oflags, NULL) != argc)
		return (DCMD_USAGE);

	mdb_printf("Message queues:\n");
	if (mdb_walk_dcmd("msq", oflags ? "kmsqid" : "ipcperm", argc, argv) ==
	    -1) {
		mdb_warn("can't walk 'msq'");
		return (DCMD_ERR);
	}

	mdb_printf("\nShared memory:\n");
	if (mdb_walk_dcmd("shm", oflags ? "kshmid" : "ipcperm", argc, argv) ==
	    -1) {
		mdb_warn("can't walk 'shm'");
		return (DCMD_ERR);
	}

	mdb_printf("\nSemaphores:\n");
	if (mdb_walk_dcmd("sem", oflags ? "ksemid" : "ipcperm", argc, argv) ==
	    -1) {
		mdb_warn("can't walk 'sem'");
		return (DCMD_ERR);
	}

	return (DCMD_OK);
}

static int
msgprint(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	struct msg message;
	uint_t	lflag = FALSE;
	long	type = 0;
	char	*tflag = NULL;

	if (!(flags & DCMD_ADDRSPEC) || (mdb_getopts(argc, argv,
	    'l', MDB_OPT_SETBITS, TRUE, &lflag,
	    't', MDB_OPT_STR, &tflag, NULL) != argc))
		return (DCMD_USAGE);

	/*
	 * Handle negative values.
	 */
	if (tflag != NULL) {
		if (*tflag == '-') {
			tflag++;
			type = -1;
		} else {
			type = 1;
		}
		type *= mdb_strtoull(tflag);
	}

	if (DCMD_HDRSPEC(flags))
		mdb_printf("%<u>%?s %?s %8s %8s %8s%</u>\n",
		    "ADDR", "TEXT", "SIZE", "TYPE", "REF");

	if (mdb_vread(&message, sizeof (struct msg), addr) == -1) {
		mdb_warn("failed to read msg at %#lx", addr);
		return (DCMD_ERR);
	}

	/*
	 * If we are meeting our type contraints, display the message.
	 * If -l was specified, we will also display the message
	 * contents.
	 */
	if ((type == 0) ||
	    (type > 0 && message.msg_type == type) ||
	    (type < 0 && message.msg_type <= -type)) {

		if (lflag && !DCMD_HDRSPEC(flags))
			mdb_printf("\n");

		mdb_printf("%0?lx %?p %8ld %8ld %8ld\n", addr, message.msg_addr,
		    message.msg_size, message.msg_type, message.msg_copycnt);

		if (lflag) {
			mdb_printf("\n");
			mdb_inc_indent(CMN_INDENT);
			if (mdb_dumpptr(
			    (uintptr_t)message.msg_addr, message.msg_size,
			    MDB_DUMP_RELATIVE | MDB_DUMP_TRIM |
			    MDB_DUMP_ASCII | MDB_DUMP_HEADER |
			    MDB_DUMP_GROUP(4), NULL, NULL)) {
				mdb_dec_indent(CMN_INDENT);
				return (DCMD_ERR);
			}
			mdb_dec_indent(CMN_INDENT);
		}
	}

	return (DCMD_OK);
}

/*
 * MDB module linkage
 */
static const mdb_dcmd_t dcmds[] = {
	/* Generic routines */
	{ "ipcperm", ":", "display an IPC perm structure", ipcperm },
	{ "ipcid", ":id", "perform an IPC id lookup", ipcid },
	{ "ipckey", ":key", "perform an IPC key lookup", ipckey },

	/* Specific routines */
	{ "kshmid", "?[-l]", "display a struct kshmid", cmd_kshmid },
	{ "kmsqid", "?[-l]", "display a struct kmsqid", cmd_kmsqid },
	{ "ksemid", "?[-l]", "display a struct ksemid", cmd_ksemid },
	{ "msg", ":[-l] [-t type]", "display contents of a message", msgprint },

	/* Convenience routines */
	{ "id2shm", ":[-k]", "convert shared memory ID to pointer", id2shm },
	{ "id2msq", ":[-k]", "convert message queue ID to pointer", id2msq },
	{ "id2sem", ":[-k]", "convert semaphore ID to pointer", id2sem },

	{ "ipcs", "[-l]", "display System V IPC information", ipcs },
	{ NULL }
};

static const mdb_walker_t walkers[] = {
	{ "ipcsvc", "walk a System V IPC service",
		ds_walk_init, ds_walk_step },
	{ "shm", "walk the active shmid_ds structures",
		ds_walk_init, ds_walk_step, NULL, &shm_ops_vec },
	{ "msq", "walk the active msqid_ds structures",
		ds_walk_init, ds_walk_step, NULL, &msq_ops_vec },
	{ "sem", "walk the active semid_ds structures",
		ds_walk_init, ds_walk_step, NULL, &sem_ops_vec },
	{ "msgqueue", "walk messages on a message queue",
		msg_walk_init, msg_walk_step },
	{ NULL }
};

static const mdb_modinfo_t modinfo = { MDB_API_VERSION, dcmds, walkers };

const mdb_modinfo_t *
_mdb_init(void)
{
	return (&modinfo);
}



/* ============================================================================
 * SOURCE 8/8: minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.cdpd\ipc.c
 * Size: 4,886 bytes, Lines: 259
 * Hash: b7d6643e863d...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2015, 2018, 2019, Meisaka Yukara
 * Copyright 2018, 2019 Prominic.NET Inc. All Rights reserved.
 * Copyright 2019 OmniOS Community Edition (OmniOSce) Association.
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "yuka.h"
#include "cdp.h"
#include "cmd.h"

extern int verbose;

static void
yuka_handle_ipc(int fd)
{
	struct pollfd pollfds[] = {
		{ .fd = fd, .events = POLLIN }
	};
	char buf[1024], *cmd;
	int argc, fflag, i;

	if (verbose > 1)
		printf("handle_ipc for FD: %d\n", fd);

	i = poll(pollfds, 1, 3000);

	if (i == 0) {
		if (verbose)
			printf("IPC timeout.\n");
		return;
	}

	if (i == -1) {
		perror("IPC poll");
		return;
	}

	argc = recv(fd, buf, 1024, 0);

	if (argc < 2) {
		printf("IPC: short read, %d\n", argc);
		return;
	}

	if (verbose > 3) {
		printf("ipc-recv:\n");
		xdump((uint8_t *)buf, argc);
	}

	fflag = buf[0];
	cmd = buf + 1;
	argc -= 2;

	switch (cmd[0]) {

	case YUKA_CMD_REAP:
		yuka_cdp_reap(0);
		break;

	case YUKA_CMD_SHOW:
		if (argc < 2) {
			yuka_show_cdp_hosts(fd, fflag);
			break;
		}

		switch (cmd[1]) {
		case YUKA_CMD_SHOW_CDP:
			yuka_show_cdp_hosts(fd, fflag);
			break;
		case YUKA_CMD_SHOW_DETAIL:
			yuka_show_detail(fd);
			break;
		default:
			(void) write(fd, "Command Error\n", 14);
			break;
		}
		break;

	case YUKA_CMD_STATS:
		yuka_stats(fd);
		break;

	default:
		(void) write(fd, "Unknown command\n", 16);
		break;
	}
}

static void *
ipc_thread(void *xfd)
{
	int fd = (int)(uintptr_t)xfd;

	(void) pthread_setname_np(pthread_self(), "ipc");

	if (verbose)
		printf("IPC thread starting\n");

	for (;;) {
		struct pollfd pollfds[] = {
			{ .fd = fd, .events = POLLIN }
		};

		if (poll(pollfds, 1, -1) <= 0)
			break;

		if (pollfds[0].revents & POLLIN) {
			struct sockaddr_un addr;
			socklen_t addrlen = sizeof (addr);
			int c = accept4(fd, (struct sockaddr *)&addr, &addrlen,
			    SOCK_NDELAY | SOCK_NONBLOCK);

			if (c == -1) {
				printf("Control socket accept failed, %s\n",
				    strerror(errno));
				continue;
			}

			if (verbose)
				printf("New IPC connection.\n");

			yuka_handle_ipc(c);
			(void) close(c);
		}
	}
	(void) close(fd);
	(void) unlink(YUKA_SOCKET);

	return (NULL);
}

int
init_ipc_socket(void)
{
	struct sockaddr_un server;
	int fd;

	memset(&server, 0, sizeof (server));
	server.sun_family = AF_UNIX;
	(void) strlcpy(server.sun_path, YUKA_SOCKET, sizeof (server.sun_path));

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("ipc: socket()");
		return (-1);
	}

	if (bind(fd, (struct sockaddr *)&server, sizeof (server)) == -1) {
		if (errno == EADDRINUSE) {
			/* See if the socket is still live */

			if (connect(fd, (struct sockaddr *) &server,
			    sizeof (server)) >= 0) {
				printf("Control socket is in use by another "
				    "process.\n");
				goto err;
			}

			/* If not, clean it up */

			if (unlink(server.sun_path) == -1) {
				printf("Could not unlink stale control "
				    "socket, %s\n", strerror(errno));
				goto err;
			}

			/* Try and bind again */

			if (bind(fd, (struct sockaddr *)&server,
			    sizeof (server)) == -1) {
				printf("Cannot bind to control socket, %s\n",
				    strerror(errno));
				goto err;
			}
		} else {
			printf("Cannot bind to control socket, %s\n",
			    strerror(errno));
			goto err;
		}
	}

	if (fchmod(fd, 0444)) {
		perror("ipc: fchmod");
		goto err;
	}

	if (listen(fd, 10)) {
		perror("ipc: listen");
		goto err;
	}

	return (fd);

err:
	(void) close(fd);
	return (-1);
}

int
init_ipc(void)
{
	pthread_t tid;
	int fd;

	if ((fd = init_ipc_socket()) == -1)
		return (0);

	if (pthread_create(&tid, NULL, ipc_thread, (void *)(uintptr_t)fd)
	    == 0 && pthread_detach(tid) == 0) {
		return (1);
	}

	(void) close(fd);
	return (0);
}

void
deinit_ipc(int fd)
{
	(void) close(fd);
	if (unlink(YUKA_SOCKET) < 0)
		perror("deinit-ipc: unlink()");
}

