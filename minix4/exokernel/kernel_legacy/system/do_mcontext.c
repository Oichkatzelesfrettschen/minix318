/* The kernel calls that are implemented in this file:
 *   m_type:	SYS_SETMCONTEXT
 *   m_type:	SYS_GETMCONTEXT
 *
 * The parameters for SYS_SETMCONTEXT kernel call are:
 *   m_lsys_krn_sys_setmcontext.endpt	# proc endpoint doing call
 *   m_lsys_krn_sys_setmcontext.ctx_ptr	# pointer to mcontext structure
 *
 * The parameters for SYS_GETMCONTEXT kernel call are:
 *   m_lsys_krn_sys_getmcontext.endpt	# proc endpoint doing call
 *   m_lsys_krn_sys_getmcontext.ctx_ptr	# pointer to mcontext structure
 */

#include "kernel/system.h"
// #include <string.h> // Replaced
// #include <assert.h> // Replaced
#include <machine/mcontext.h> // Kept

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t or similar if error codes are mapped
#include <sys/kassert.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>

// #include <string.h> // Replaced
// #include <assert.h> // Replaced
#include <machine/mcontext.h> // Kept

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t or similar if error codes are mapped
#include <sys/kassert.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


#if USE_MCONTEXT 
/*===========================================================================*
 *			      do_getmcontext				     *
 *===========================================================================*/
int do_getmcontext(struct proc * caller, message * m_ptr)
{
/* Retrieve machine context of a process */

  register struct proc *rp;
  int proc_nr, r;
  mcontext_t mc;

  if (!isokendpt(m_ptr->m_lsys_krn_sys_getmcontext.endpt, &proc_nr))
	return(EINVAL); // EINVAL might be undefined
  if (iskerneln(proc_nr)) return(EPERM); // EPERM might be undefined
	return(EINVAL); // EINVAL might be undefined
  if (iskerneln(proc_nr)) return(EPERM); // EPERM might be undefined
  rp = proc_addr(proc_nr);

#if defined(__i386__)
  if (!proc_used_fpu(rp))
	return(OK);	/* No state to copy */
#endif

  /* Get the mcontext structure into our address space.  */
  if ((r = data_copy(m_ptr->m_lsys_krn_sys_getmcontext.endpt,
		m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) sizeof(mcontext_t))) != OK)
	return(r);

  mc.mc_flags = 0;
#if defined(__i386__)
  /* Copy FPU state */
  if (proc_used_fpu(rp)) {
	/* make sure that the FPU context is saved into proc structure first */
	save_fpu(rp);
	mc.mc_flags = (rp->p_misc_flags & MF_FPU_INITIALIZED) ? _MC_FPU_SAVED : 0;
	KASSERT(sizeof(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	kmemcpy(&(mc.__fpregs.__fp_reg_set), rp->p_seg.fpu_state, FPU_XFP_SIZE); // MODIFIED
  } 
#endif


  /* Copy the mcontext structure to the user's address space. */
  if ((r = data_copy(KERNEL, (vir_bytes) &mc,
	m_ptr->m_lsys_krn_sys_getmcontext.endpt,
	m_ptr->m_lsys_krn_sys_getmcontext.ctx_ptr,
	(phys_bytes) sizeof(mcontext_t))) != OK)
	return(r);

  return(OK);
}


/*===========================================================================*
 *			      do_setmcontext				     *
 *===========================================================================*/
int do_setmcontext(struct proc * caller, message * m_ptr)
{
/* Set machine context of a process */

  register struct proc *rp;
  int proc_nr, r;
  mcontext_t mc;

  if (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) return(EINVAL); // EINVAL might be undefined
  if (!isokendpt(m_ptr->m_lsys_krn_sys_setmcontext.endpt, &proc_nr)) return(EINVAL); // EINVAL might be undefined
  rp = proc_addr(proc_nr);

  /* Get the mcontext structure into our address space.  */
  if ((r = data_copy(m_ptr->m_lsys_krn_sys_setmcontext.endpt,
		m_ptr->m_lsys_krn_sys_setmcontext.ctx_ptr, KERNEL,
		(vir_bytes) &mc, (phys_bytes) sizeof(mcontext_t))) != OK)
	return(r);

#if defined(__i386__)
  /* Copy FPU state */
  if (mc.mc_flags & _MC_FPU_SAVED) {
	rp->p_misc_flags |= MF_FPU_INITIALIZED;
	KASSERT(sizeof(mc.__fpregs.__fp_reg_set) == FPU_XFP_SIZE);
	kmemcpy(rp->p_seg.fpu_state, &(mc.__fpregs.__fp_reg_set), FPU_XFP_SIZE); // MODIFIED
  } else
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  /* force reloading FPU in either case */
  release_fpu(rp);
#endif

  return(OK);
}

#endif
