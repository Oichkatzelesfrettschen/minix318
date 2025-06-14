/* The kernel call implemented in this file:
 *   m_type:	SYS_SDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_sdevio.request	(request input or output)
 *    m_lsys_krn_sys_sdevio.port	(port to read/ write)
 *    m_lsys_krn_sys_sdevio.vec_addr	(virtual address of buffer, or grant ID)
 *    m_lsys_krn_sys_sdevio.vec_size	(number of elements)
 *    m_lsys_krn_sys_sdevio.vec_endpt	(process where buffer is)
 *    m_lsys_krn_sys_sdevio.offset	(offset into the grant)
 */

#include "kernel/system.h"
#include <minix/devio.h>    // Kept
#include <minix/endpoint.h> // Kept

#include "arch_proto.h"

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t, k_size_t
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


#if USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
int do_sdevio(struct proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  int proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes count_vb = m_ptr->m_lsys_krn_sys_sdevio.vec_size; // Keep as vir_bytes for message struct
  k_size_t count_ksz = (k_size_t)m_ptr->m_lsys_krn_sys_sdevio.vec_size; // Use k_size_t for logic
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  int i, r, req_type, req_dir, size, nr_io_range;
  struct priv *privp;
  struct io_range *iorp;
  struct proc *destproc;
  int retval;

  /* Allow safe copies and accesses to SELF */
  if ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	static int first= 1;
	if (first)
	{
		first= 0;
		kprintf_stub("do_sdevio: for %d, req %d\n", // MODIFIED
			m_ptr->m_source, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  if (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  else
	if(!isokendpt(proc_nr_e, &proc_nr))
		return(EINVAL); // EINVAL might be undefined
  if (iskerneln(proc_nr)) return(EPERM); // EPERM might be undefined

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  if((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     if((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, count_ksz, // Use k_size_t version for verify_grant
		req_dir == _DIO_INPUT ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		NULL)) != OK) { // NULL might be undefined
	if(r == ENOTREADY) return r; // ENOTREADY might be undefined
	kprintf_stub("do_sdevio: verify_grant failed\n"); // MODIFIED
	return EPERM; // EPERM might be undefined
    }
	if(!isokendpt(newep, &proc_nr))
		return(EINVAL); // EINVAL might be undefined
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } else {
     if(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	kprintf_stub("do_sdevio: unsafe sdevio by %d in %d denied\n", // MODIFIED
		caller->p_endpoint, proc_nr_e);
	return EPERM; // EPERM might be undefined
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  switch (req_type)
  {
	case _DIO_BYTE: size= 1; break;
	case _DIO_WORD: size= 2; break;
	case _DIO_LONG: size= 4; break;
	default: size= 4; break;	/* Be conservative */
  }

  privp= priv(caller);
  if (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	for (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		if (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			break;
	}
	if (i >= nr_io_range)
	{
		kprintf_stub( // MODIFIED
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM; // EPERM might be undefined
		goto return_error;
	}
  }

  if (port & (size-1))
  {
	kprintf_stub("do_devio: unaligned port 0x%x (size %d)\n", (int)port, size); // MODIFIED, cast port to int for %x
	retval = EPERM; // EPERM might be undefined
	goto return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  if (req_dir == _DIO_INPUT) { 
      switch (req_type) {
      case _DIO_BYTE: phys_insb(port, vir_buf, count_vb); break;
      case _DIO_WORD: phys_insw(port, vir_buf, count_vb); break;
      default:
		retval = EINVAL; // EINVAL might be undefined
		goto return_error;
      } 
  } else if (req_dir == _DIO_OUTPUT) { 
      switch (req_type) {
      case _DIO_BYTE: phys_outsb(port, vir_buf, count_vb); break;
      case _DIO_WORD: phys_outsw(port, vir_buf, count_vb); break;
      default:
		retval = EINVAL; // EINVAL might be undefined
		goto return_error;
      } 
  }
  else {
	  retval = EINVAL; // EINVAL might be undefined
	  goto return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  return retval;
}

#endif /* USE_SDEVIO */
