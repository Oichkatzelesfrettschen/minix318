/*
 * do_endksig.c
 *
 * SYS_ENDKSIG kernel call handler.
 *
 *   m_type:    SYS_ENDKSIG
 *   m_sigcalls.endpt: endpoint of process whose signal processing has ended
 *
 * This call is made by the signal manager (usually PM) to tell the kernel
 * that it has finished delivering and handling one or more signals for a
 * given target process.  Under the target’s p_sig_lock:
 *   - Verify the caller is the authorized s_sig_mgr.
 *   - Ensure RTS_SIG_PENDING is set.
 *   - If no new signals arrived (RTS_SIGNALED clear), clear RTS_SIG_PENDING.
 *   - Otherwise leave RTS_SIG_PENDING set; PM will be notified again.
 *
 * Returns:
 *   OK      – success
 *   EINVAL  – bad endpoint or protocol state
 *   EPERM   – caller not authorized signal manager
 */

#include "kernel/system.h"

#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/kernel_types.h>

#include "kernel/k_spinlock_irq.h"  /* spinlock_irq_t, irq-safe APIs */

#if USE_ENDKSIG

/**
 * @brief Handle the SYS_ENDKSIG kernel call.
 * @param caller The kernel process invoking end-of-signal handling (PM).
 * @param m_ptr  The message containing .m_sigcalls.endpt.
 * @return OK on success; error otherwise.
 */
int do_endksig(struct proc *caller, message *m_ptr)
{
    int proc_nr;
    struct proc *rp;
    unsigned flags;

    /* Validate endpoint and map to process slot */
    if (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
        return EINVAL;

    rp = proc_addr(proc_nr);

    /* Sanity checks on target process structure */
    KASSERT(rp != NULL,
            "do_endksig: null process for slot %d", proc_nr);
    KASSERT(rp->p_magic == PMAGIC,
            "do_endksig: corrupted proc %d/%d",
            proc_nr, rp->p_endpoint);
    KASSERT(priv(rp) != NULL,
            "do_endksig: no priv for proc %d/%d",
            proc_nr, rp->p_endpoint);

    /* Only the designated signal manager may call this */
    if (caller->p_endpoint != priv(rp)->s_sig_mgr)
        return EPERM;

    /* Acquire the process’s signal lock */
    flags = spin_lock_irqsave(&rp->p_sig_lock);

    /* Verify manager status under lock */
    KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr,
            "do_endksig: caller %d not s_sig_mgr %d",
            caller->p_endpoint, priv(rp)->s_sig_mgr);
    KASSERT(RTS_ISSET(rp, RTS_SIG_PENDING),
            "do_endksig: no SIG_PENDING on proc %d/%d",
            proc_nr, rp->p_endpoint);

    /* If unexpected state, bail out */
    if (!RTS_ISSET(rp, RTS_SIG_PENDING)) {
        spin_unlock_irqrestore(&rp->p_sig_lock, flags);
        return EINVAL;
    }

    /* Clear pending flag if no new signals arrived */
    if (!RTS_ISSET(rp, RTS_SIGNALED)) {
        RTS_UNSET(rp, RTS_SIG_PENDING);
    }

    /* Leave SIG_PENDING set if RTS_SIGNALED was true */

    spin_unlock_irqrestore(&rp->p_sig_lock, flags);

    return OK;
}

#endif /* USE_ENDKSIG */
