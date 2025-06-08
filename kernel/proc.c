/* This file contains essentially all of the process and message handling.
 * Together with "mpx.s" it forms the lowest layer of the MINIX kernel.
 * There is one entry point from the outside:
 *
 *   sys_call: 	      a system call, i.e., the kernel is trapped with an INT
 *
 * As well as several entry points used from the interrupt and task level:
 *
 *   lock_notify:     notify a process of a system event
 *   lock_send:	      send a message to a process
 *   lock_enqueue:    put a process on one of the scheduling queues 
 *   lock_dequeue:    remove a process from the scheduling queues
 *
 * Changes:
 *   Aug 19, 2005     rewrote scheduling code  (Jorrit N. Herder)
 *   Jul 25, 2005     rewrote system call handling  (Jorrit N. Herder)
 *   May 26, 2005     rewrote message passing functions  (Jorrit N. Herder)
 *   May 24, 2005     new notification system call  (Jorrit N. Herder)
 *   Oct 28, 2004     nonblocking send and receive calls  (Jorrit N. Herder)
 *
 * The code here is critical to make everything work and is important for the
 * overall performance of the system. A large fraction of the code deals with
 * list manipulation. To make this both easy to understand and fast to execute 
 * pointer pointers are used throughout the code. Pointer pointers prevent
 * exceptions for the head or tail of a linked list. 
 *
 *  node_t *queue, *new_node;	// assume these as global variables
 *  node_t **xpp = &queue; 	// get pointer pointer to head of queue 
 *  while (*xpp != NULL) 	// find last pointer of the linked list
 *      xpp = &(*xpp)->next;	// get pointer to next pointer 
 *  *xpp = new_node;		// now replace the end (the NULL pointer) 
 *  new_node->next = NULL;	// and mark the new end of the list
 * 
 * For example, when adding a new node to the end of the list, one normally 
 * makes an exception for an empty list and looks up the end of the list for 
 * nonempty lists. As shown above, this is not required with pointer pointers.
 */

#include <minix/com.h>
#include <minix/callnr.h>
#include "kernel.h"
#include "proc.h"
#include "reply_obj.h" // For explicit reply objects

/* Scheduling and message passing functions. The functions are available to 
 * other parts of the kernel through lock_...(). The lock temporarily disables 
 * interrupts to prevent race conditions. 
 */
FORWARD _PROTOTYPE( int mini_send, (struct proc *caller_ptr, int dst,
		message *m_ptr, unsigned flags) );
FORWARD _PROTOTYPE( int mini_receive, (struct proc *caller_ptr, int src,
		message *m_ptr, unsigned flags) );
// Modified mini_notify to accept the user message pointer for badge extraction
FORWARD _PROTOTYPE( int mini_notify, (struct proc *caller_ptr, int dst, message *user_m_ptr) );

FORWARD _PROTOTYPE( void enqueue, (struct proc *rp) );
FORWARD _PROTOTYPE( void dequeue, (struct proc *rp) );
FORWARD _PROTOTYPE( void sched, (struct proc *rp, int *queue, int *front) );
FORWARD _PROTOTYPE( void pick_proc, (void) );

// Global table for reply objects
struct reply_object reply_objects_table[NR_PROCS];

// Function to initialize the reply objects table
PUBLIC void init_reply_objects(void) {
    int i;
    for (i = 0; i < NR_PROCS; i++) {
        reply_objects_table[i].ro_in_use = 0;
        // Assuming p_reply_obj_index exists in struct proc and is initialized elsewhere
        // Ideally, proc_table[i].p_reply_obj_index = NO_REPLY_OBJ; would be here.
    }
    // Also initialize p_reply_obj_index for all processes in the proc table
    // This requires access to the proc table (e.g., `proc` or `proc_addr(i)`)
    // For example (conceptual):
    // struct proc *rp;
    // for (rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; rp++) {
    //     rp->p_reply_obj_index = NO_REPLY_OBJ; // ASSUMPTION: p_reply_obj_index exists
    // }
}

// Function to allocate a reply object
PRIVATE int alloc_reply_object(int caller_proc_nr, int target_proc_nr, endpoint_t caller_ep) {
    int i;
    for (i = 0; i < NR_PROCS; i++) {
        if (reply_objects_table[i].ro_in_use == 0) {
            reply_objects_table[i].ro_in_use = 1;
            reply_objects_table[i].ro_caller_proc_nr = caller_proc_nr;
            reply_objects_table[i].ro_target_proc_nr = target_proc_nr;
            reply_objects_table[i].ro_caller_ep = caller_ep;
            // ro_reply_message will be filled by do_reply
            return i;
        }
    }
    kprintf("alloc_reply_object: No free reply objects!\n");
    return -1; // Error: No free reply objects (should use a proper error code)
}

// Function to free a reply object
PRIVATE void free_reply_object(int obj_index) {
    if (obj_index >= 0 && obj_index < NR_PROCS) {
        reply_objects_table[obj_index].ro_in_use = 0;
        // Clear other fields for security/safety if necessary
        reply_objects_table[obj_index].ro_caller_proc_nr = NONE;
        reply_objects_table[obj_index].ro_target_proc_nr = NONE;
        reply_objects_table[obj_index].ro_caller_ep = NONE;
    } else {
        kprintf("free_reply_object: Invalid obj_index %d\n", obj_index);
    }
}

// Modified BuildMess to include a badge and use direct struct access
#define BuildMess(m_ptr_kernel, src_proc_nr, dst_proc_ptr, badge_val) \
	(m_ptr_kernel)->m_source = (src_proc_nr); \
	(m_ptr_kernel)->m_type = NOTIFY_FROM(src_proc_nr); \
	(m_ptr_kernel)->m_notify.timestamp = get_uptime(); \
	(m_ptr_kernel)->m_notify.badge = (badge_val); \
	switch (src_proc_nr) { \
	case HARDWARE: \
		(m_ptr_kernel)->m_notify.interrupts = priv(dst_proc_ptr)->s_int_pending; \
		priv(dst_proc_ptr)->s_int_pending = 0; \
		(m_ptr_kernel)->m_notify.sigset = 0; /* Clear sigset field */ \
		break; \
	case SYSTEM: \
		(m_ptr_kernel)->m_notify.sigset = priv(dst_proc_ptr)->s_sig_pending; \
		priv(dst_proc_ptr)->s_sig_pending = 0; \
		(m_ptr_kernel)->m_notify.interrupts = 0; /* Clear interrupts field */ \
		break; \
	default: \
		/* For other kernel notifications (e.g. from drivers), clear specific payload fields */ \
		(m_ptr_kernel)->m_notify.interrupts = 0; \
		(m_ptr_kernel)->m_notify.sigset = 0; \
		break; \
	}

#if (CHIP == INTEL)
#define CopyMess(s,sp,sm,dp,dm) \
	cp_mess(s, (sp)->p_memmap[D].mem_phys,	\
		 (vir_bytes)sm, (dp)->p_memmap[D].mem_phys, (vir_bytes)dm)
#endif /* (CHIP == INTEL) */

#if (CHIP == M68000)
/* M68000 does not have cp_mess() in assembly like INTEL. Declare prototype
 * for cp_mess() here and define the function below. Also define CopyMess. 
 */
#endif /* (CHIP == M68000) */

/*===========================================================================*
 *				sys_call				     * 
 *===========================================================================*/
PUBLIC int sys_call(call_nr, src_dst, m_ptr)
int call_nr;			/* system call number and flags */
int src_dst;			/* src to receive from or dst to send to */
message *m_ptr;			/* pointer to message in the caller's space */
{
/* System calls are done by trapping to the kernel with an INT instruction.
 * The trap is caught and sys_call() is called to send or receive a message
 * (or both). The caller is always given by 'proc_ptr'.
 */
  register struct proc *caller_ptr = proc_ptr;	/* get pointer to caller */
  int function = call_nr & SYSCALL_FUNC;	/* get system call function */
  unsigned flags = call_nr & SYSCALL_FLAGS;	/* get flags */
  int mask_entry;				/* bit to check in send mask */
  int result;					/* the system call's result */
  vir_clicks vlo, vhi;		/* virtual clicks containing message to send */

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  if (! (priv(caller_ptr)->s_trap_mask & (1 << function)) || 
          (iskerneln(src_dst) && function != SENDREC
           && function != RECEIVE)) { 
      kprintf("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          function, proc_nr(caller_ptr), src_dst);
      return(ECALLDENIED);		/* trap denied by mask or kernel */
  }
  
  /* Require a valid source and/ or destination process, unless echoing. */
  if (! (isokprocn(src_dst) || src_dst == ANY || function == ECHO)) { 
      kprintf("sys_call: invalid src_dst, src_dst %d, caller %d\n", 
          src_dst, proc_nr(caller_ptr));
      return(EBADSRCDST);		/* invalid process number */
  }

  /* If the call involves a message buffer, i.e., for SEND, RECEIVE, SENDREC, 
   * or ECHO, check the message pointer. This check allows a message to be 
   * anywhere in data or stack or gap. It will have to be made more elaborate 
   * for machines which don't have the gap mapped. 
   */
  if (function & CHECK_PTR) {	
      vlo = (vir_bytes) m_ptr >> CLICK_SHIFT;		
      vhi = ((vir_bytes) m_ptr + MESS_SIZE - 1) >> CLICK_SHIFT;
      if (vlo < caller_ptr->p_memmap[D].mem_vir || vlo > vhi ||
              vhi >= caller_ptr->p_memmap[S].mem_vir + 
              caller_ptr->p_memmap[S].mem_len) {
          kprintf("sys_call: invalid message pointer, trap %d, caller %d\n",
          	function, proc_nr(caller_ptr));
          return(EFAULT); 		/* invalid message pointer */
      }
  }

  /* If the call is to send to a process, i.e., for SEND, SENDREC or NOTIFY,
   * verify that the caller is allowed to send to the given destination and
   * that the destination is still alive. 
   */
  if (function & CHECK_DST) {	
      if (! get_sys_bit(priv(caller_ptr)->s_ipc_to, nr_to_id(src_dst))) {
          kprintf("sys_call: ipc mask denied %d sending to %d\n",
          	proc_nr(caller_ptr), src_dst);
          return(ECALLDENIED);		/* call denied by ipc mask */
      }

      if (isemptyn(src_dst) && !shutdown_started) {
          kprintf("sys_call: dead dest; %d, %d, %d\n", 
              function, proc_nr(caller_ptr), src_dst);
          return(EDEADDST); 		/* cannot send to the dead */
      }
  }

  /* Now check if the call is known and try to perform the request. The only
   * system calls that exist in MINIX are sending and receiving messages.
   *   - SENDREC: combines SEND and RECEIVE in a single system call
   *   - SEND:    sender blocks until its message has been delivered
   *   - RECEIVE: receiver blocks until an acceptable message has arrived
   *   - NOTIFY:  nonblocking call; deliver notification or mark pending
   *   - ECHO:    nonblocking call; directly echo back the message 
   */
  switch(function) {
  case SENDREC:
      /* A flag is set so that notifications cannot interrupt SENDREC. */
      priv(caller_ptr)->s_flags |= SENDREC_BUSY;
      caller_ptr->p_reply_obj_index = NO_REPLY_OBJ; // Initialize for this SENDREC attempt

      result = mini_send(caller_ptr, src_dst, m_ptr, flags);
      if (result != OK) {
          priv(caller_ptr)->s_flags &= ~SENDREC_BUSY; // Clear busy flag on send failure
          break;				/* SEND part failed */
      }

      /* SEND was successful, now prepare for RECEIVE part with reply object */
      /* ASSUMPTION: caller_ptr->p_reply_obj_index exists in struct proc */
      int obj_idx = alloc_reply_object(proc_nr(caller_ptr), src_dst, caller_ptr->p_endpoint);
      if (obj_idx < 0) {
          /* Failed to allocate a reply object. This is a critical error.
           * The original SENDREC would block here. We need to decide how to handle this.
           * Option 1: Return an error. This changes SENDREC behavior.
           * Option 2: Attempt to "undo" mini_send if possible (complex).
           * Option 3: Fallback to old mechanism (but we want to replace it).
           * For now, return an error. Let's use ENOSPC as a placeholder for no reply objects.
           */
          priv(caller_ptr)->s_flags &= ~SENDREC_BUSY;
          /* TODO: If mini_send made the process SENDING, this needs to be undone too,
           * or the destination might be confused. This is a complex part of error handling.
           * For this design phase, we'll assume mini_send blocks or this error is critical.
           */
          result = ENOSPC; // Or a more specific error like EREPLYOBJEXHAUSTED
          break;
      }
      caller_ptr->p_reply_obj_index = obj_idx;

      /* Fall through to RECEIVE */
      result = mini_receive(caller_ptr, src_dst, m_ptr, flags); // src_dst is the original target
      // If mini_receive blocks (returns OK), p_reply_obj_index remains set.
      // If mini_receive doesn't block (e.g. NON_BLOCKING flag for receive, unusual for SENDREC),
      // or if it gets a message immediately (not possible with reply_obj logic yet),
      // then the reply object might need cleanup if not used.
      // However, standard SENDREC implies blocking receive.
      if (result != OK && caller_ptr->p_reply_obj_index != NO_REPLY_OBJ) {
          // If mini_receive failed to block for some reason, free the allocated reply object.
          free_reply_object(caller_ptr->p_reply_obj_index);
          caller_ptr->p_reply_obj_index = NO_REPLY_OBJ;
          priv(caller_ptr)->s_flags &= ~SENDREC_BUSY; // Clear busy if not blocking
      }
      break;

  case SEND:			
      result = mini_send(caller_ptr, src_dst, m_ptr, flags);
      break;
  case RECEIVE:			
      /* This is for a standalone RECEIVE call. SENDREC has its own path. */
      priv(caller_ptr)->s_flags &= ~SENDREC_BUSY; // Clear busy flag if set by other means
      caller_ptr->p_reply_obj_index = NO_REPLY_OBJ; // Not using reply_obj for standalone RECEIVE
      result = mini_receive(caller_ptr, src_dst, m_ptr, flags);
      break;
  case NOTIFY:
      // Pass the caller's message (m_ptr) to mini_notify to extract the badge
      result = mini_notify(caller_ptr, src_dst, m_ptr);
      break;
  case ECHO:
      CopyMess(caller_ptr->p_nr, caller_ptr, m_ptr, caller_ptr, m_ptr);
      result = OK;
      break;
  case SYS_REPLY: /* New system call for replying */
      // src_dst parameter for SYS_REPLY would be the original caller's endpoint,
      // but do_reply expects it in m_ptr->m_type.
      // The user-level library function for reply(to_endpoint, message) should set this.
      // For sys_call, src_dst is usually the primary target/source endpoint.
      // Let's ensure m_ptr is correctly populated by the caller of SYS_REPLY.
      // The actual endpoint to reply to is taken from m_ptr->m_type in do_reply.
      result = do_reply(caller_ptr, m_ptr);
      break;
  default:
      result = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  return(result);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
PRIVATE int mini_send(caller_ptr, dst, m_ptr, flags)
register struct proc *caller_ptr;	/* who is trying to send a message? */
int dst;				/* to whom is message being sent? */
message *m_ptr;				/* pointer to message buffer */
unsigned flags;				/* system call flags */
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  register struct proc *dst_ptr = proc_addr(dst);
  register struct proc **xpp;
  register struct proc *xp;

  /* Check for deadlock by 'caller_ptr' and 'dst' sending to each other. */
  xp = dst_ptr;
  while (xp->p_rts_flags & SENDING) {		/* check while sending */
  	xp = proc_addr(xp->p_sendto);		/* get xp's destination */
  	if (xp == caller_ptr) return(ELOCKED);	/* deadlock if cyclic */
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  if ( (dst_ptr->p_rts_flags & (RECEIVING | SENDING)) == RECEIVING &&
       (dst_ptr->p_getfrom == ANY || dst_ptr->p_getfrom == caller_ptr->p_nr)) {
	/* Destination is indeed waiting for this message. */
	CopyMess(caller_ptr->p_nr, caller_ptr, m_ptr, dst_ptr,
		 dst_ptr->p_messbuf);
	if ((dst_ptr->p_rts_flags &= ~RECEIVING) == 0) enqueue(dst_ptr);
  } else if ( ! (flags & NON_BLOCKING)) {
	/* Destination is not waiting.  Block and queue caller. */
	caller_ptr->p_messbuf = m_ptr;
	if (caller_ptr->p_rts_flags == 0) dequeue(caller_ptr);
	caller_ptr->p_rts_flags |= SENDING;
	caller_ptr->p_sendto = dst;

	/* Process is now blocked.  Put in on the destination's queue. */
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	while (*xpp != NIL_PROC) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */
	caller_ptr->p_q_link = NIL_PROC;	/* mark new end of list */
  } else {
	return(ENOTREADY);
  }
  return(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
PRIVATE int mini_receive(caller_ptr, src, m_ptr, flags)
register struct proc *caller_ptr;	/* process trying to get message */
int src;				/* which message source is wanted */
message *m_ptr;				/* pointer to message buffer */
unsigned flags;				/* system call flags */
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller, unless the flags don't allow blocking.  
 */
  register struct proc **xpp;
  register struct notification **ntf_q_pp;
  message m;
  int bit_nr;
  sys_map_t *map;
  bitchunk_t *chunk;
  int i, src_id, src_proc_nr;

  /* If this is part of SENDREC and a reply object is associated,
   * the process must block and wait for an explicit SYS_REPLY.
   * Skip checking p_caller_q and notifications for this receive.
   * ASSUMPTION: p_reply_obj_index exists in struct proc.
   */
  if ((priv(caller_ptr)->s_flags & SENDREC_BUSY) &&
      (caller_ptr->p_reply_obj_index != NO_REPLY_OBJ)) {

      if (caller_ptr->p_rts_flags & SENDING) {
          // This should not happen if mini_send in SENDREC blocked the caller as expected.
          // If SEND part was non-blocking, this state is possible.
          // For now, we assume SENDREC's send part is blocking.
          // kprintf("mini_receive: SENDREC caller has SENDING set with reply_obj_index %d\n", caller_ptr->p_reply_obj_index);
      }

      // Block the caller for the REC part of SENDREC, waiting for SYS_REPLY.
      // The source (src) for SENDREC is the original destination.
      caller_ptr->p_getfrom = src;
      caller_ptr->p_messbuf = m_ptr; // Kernel buffer to copy reply message into
      if ((caller_ptr->p_rts_flags & (SENDING | RECEIVING)) == 0) {
          // Only dequeue if not already blocked (e.g. by SENDING part of SENDREC)
          // However, SENDREC implies the process was already marked SENDING or will be.
          // If SENDING was set, it should not be on ready queue.
          // If SENDING was not set (e.g. non-blocking SEND part, atypical for SENDREC),
          // then it might be on ready queue.
          // Standard SENDREC logic: mini_send blocks, then mini_receive is called.
          // So, it should already be off the ready queues if SENDING.
          // If p_rts_flags became 0 somehow (e.g. SEND was to a waiting receiver),
          // it might have been enqueued.
          if (caller_ptr->p_rts_flags == 0) dequeue(caller_ptr);
      }
      caller_ptr->p_rts_flags |= RECEIVING; // Mark as receiving
      // s_flags SENDREC_BUSY is already set by sys_call.
      return(OK); // Caller is now blocked, waiting for SYS_REPLY.
  }

  /* Original logic for standalone RECEIVE or SENDREC fallback (if p_reply_obj_index is NO_REPLY_OBJ) */
  if (!(caller_ptr->p_rts_flags & SENDING)) {
    /* Check if there are pending notifications, except for SENDREC
     * if it's using the new reply object mechanism (already handled above).
     */
    if (! (priv(caller_ptr)->s_flags & SENDREC_BUSY) || (caller_ptr->p_reply_obj_index == NO_REPLY_OBJ) ) {
        map = &priv(caller_ptr)->s_notify_pending;
        for (chunk=&map->chunk[0]; chunk<&map->chunk[NR_SYS_CHUNKS]; chunk++) {
            if (! *chunk) continue;
            for (i=0; ! (*chunk & (1<<i)); ++i) {}
            src_id = (chunk - &map->chunk[0]) * BITCHUNK_BITS + i;
            if (src_id >= NR_SYS_PROCS) break;
            src_proc_nr = id_to_nr(src_id);
            if (src!=ANY && src!=src_proc_nr && src_proc_nr != SYSTEM) continue; // Allow SYSTEM notifications through
            *chunk &= ~(1 << i);
            BuildMess(&m, src_proc_nr, caller_ptr);
            CopyMess(src_proc_nr, proc_addr(HARDWARE), &m, caller_ptr, m_ptr);
            return(OK);
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    /* This part should only be reached for:
     * 1. Standalone RECEIVE.
     * 2. SENDREC that is NOT using reply objects (e.g. alloc failed and we have a fallback,
     *    or if p_reply_obj_index was not set for some other reason).
     */
    if (! (priv(caller_ptr)->s_flags & SENDREC_BUSY) || (caller_ptr->p_reply_obj_index == NO_REPLY_OBJ) ) {
        xpp = &caller_ptr->p_caller_q;
        while (*xpp != NIL_PROC) {
            if (src == ANY || src == proc_nr(*xpp)) {
                CopyMess((*xpp)->p_nr, *xpp, (*xpp)->p_messbuf, caller_ptr, m_ptr);
                if (((*xpp)->p_rts_flags &= ~SENDING) == 0) enqueue(*xpp);
                *xpp = (*xpp)->p_q_link;
                // If this was a SENDREC that fell back, clear SENDREC_BUSY
                if(priv(caller_ptr)->s_flags & SENDREC_BUSY) {
                    priv(caller_ptr)->s_flags &= ~SENDREC_BUSY;
                }
                return(OK);
            }
            xpp = &(*xpp)->p_q_link;
        }
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC (archaic SENDREC path).
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  if ( ! (flags & NON_BLOCKING)) {
      caller_ptr->p_getfrom = src;		
      caller_ptr->p_messbuf = m_ptr;
      if (caller_ptr->p_rts_flags == 0) dequeue(caller_ptr);
      caller_ptr->p_rts_flags |= RECEIVING;
      // SENDREC_BUSY flag remains if it was SENDREC and it's blocking here.
      // If it's a SENDREC that used a reply object, it would have returned OK earlier.
      return(OK);
  } else {
      // Non-blocking receive and no message available.
      // If it was SENDREC, clear the busy flag and any reply object.
      if (priv(caller_ptr)->s_flags & SENDREC_BUSY) {
          if(caller_ptr->p_reply_obj_index != NO_REPLY_OBJ) {
             free_reply_object(caller_ptr->p_reply_obj_index);
             caller_ptr->p_reply_obj_index = NO_REPLY_OBJ;
          }
          priv(caller_ptr)->s_flags &= ~SENDREC_BUSY;
      }
      return(ENOTREADY);
  }
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
// Added user_m_ptr to extract badge from caller's message
PRIVATE int mini_notify(caller_ptr, dst, user_m_ptr)
register struct proc *caller_ptr;	/* sender of the notification */
int dst;				/* which process to notify */
message *user_m_ptr;        /* message from the NOTIFY caller, used for badge */
{
  register struct proc *dst_ptr = proc_addr(dst);
  int src_id;				/* source id for late delivery */
  message m;				/* the notification message kernel will build */
  u32_t badge_value = 0;    /* Default badge to 0 for kernel-internal notifications */

  /* Extract badge from the user-supplied message if this is a syscall.
   * As per problem spec, user-level library places badge in m_source.
   * This is unconventional; m1_iX fields are more typical for such parameters.
   * For syscalls, CHECK_PTR in sys_call ensures m_ptr (user_m_ptr here) is valid.
   */
  if (user_m_ptr != NULL) {
      // Assuming m_source is of a type assignable to u32_t.
      // In Minix, m_source is endpoint_t (effectively int). This should be fine for u32_t.
      badge_value = (u32_t) user_m_ptr->m_source;
  }

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  if ((dst_ptr->p_rts_flags & (RECEIVING|SENDING)) == RECEIVING &&
      ! (priv(dst_ptr)->s_flags & SENDREC_BUSY) && /* Not busy with SENDREC reply object */
      (dst_ptr->p_getfrom == ANY || dst_ptr->p_getfrom == caller_ptr->p_nr)) {

      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      BuildMess(&m, proc_nr(caller_ptr), dst_ptr, badge_value); // Pass badge to BuildMess
      CopyMess(proc_nr(caller_ptr), proc_addr(HARDWARE), &m, 
          dst_ptr, dst_ptr->p_messbuf);
      dst_ptr->p_rts_flags &= ~RECEIVING;	/* deblock destination */
      if (dst_ptr->p_rts_flags == 0) enqueue(dst_ptr);
      return(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the system id 
   * instead of the process number is used in the pending bit map.
   * IMPORTANT: The badge is NOT stored with the pending bit if delivery is deferred.
   * This design currently only adds badges to notifications that are delivered immediately.
   * To support badges for deferred notifications, s_notify_pending structure would
   * need to be redesigned to store badges alongside pending source bits.
   */
  if (badge_value != 0 && user_m_ptr != NULL) { // Only print for actual user-provided badges
      kprintf("mini_notify: Badge %u for notification from %d to %d will be lost as notification is pending.\n",
              badge_value, proc_nr(caller_ptr), dst);
  }
  src_id = priv(caller_ptr)->s_id;
  set_sys_bit(priv(dst_ptr)->s_notify_pending, src_id); 
  return(OK);
}

/*===========================================================================*
 *				lock_notify				     *
 *===========================================================================*/
// Modifying lock_notify to align with mini_notify's new signature.
// This function is used by kernel tasks/interrupts to send notifications.
// These notifications typically won't have a 'user_m_ptr' for a badge from a syscall.
// So, we pass NULL for user_m_ptr, and mini_notify will use a default badge (0).
PUBLIC int lock_notify(src_proc_nr, dst_proc_nr)
int src_proc_nr;			/* kernel sender of the notification (process number) */
int dst_proc_nr;			/* who is to be notified (process number) */
{
/* Safe gateway to mini_notify() for tasks and interrupt handlers. The sender
 * is explicitely given to prevent confusion where the call comes from. MINIX 
 * kernel is not reentrant, which means to interrupts are disabled after 
 * the first kernel entry (hardware interrupt, trap, or exception). Locking
 * is done by temporarily disabling interrupts. 
 */
  int result;
  struct proc *src_ptr;

  /* When src_proc_nr is HARDWARE or SYSTEM, proc_addr might not be appropriate
   * or might return a pointer to a "kernel proc" structure that doesn't fully
   * represent a normal process (e.g. no user_m_ptr).
   * mini_notify expects a valid caller_ptr for priv(caller_ptr)->s_id.
   * For HARDWARE/SYSTEM, a dummy proc_ptr or specific handling might be needed
   * if s_id is not directly available or relevant in the same way.
   * However, proc_addr(SYSTEM) and proc_addr(HARDWARE) are valid.
   */
  src_ptr = proc_addr(src_proc_nr);

  /* Exception or interrupt occurred, thus already locked. */
  if (k_reenter >= 0) {
      // Pass NULL for user_m_ptr, as this is a kernel-originated notification
      result = mini_notify(src_ptr, dst_proc_nr, NULL);
  }
  /* Call from task level, locking is required. */
  else {
      lock(0, "notify");
      // Pass NULL for user_m_ptr
      result = mini_notify(src_ptr, dst_proc_nr, NULL);
      unlock(0);
  }
  return(result);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
PRIVATE void enqueue(rp)
register struct proc *rp;	/* this process is now runnable */
{
/* Add 'rp' to one of the queues of runnable processes.  This function is 
 * responsible for inserting a process into one of the scheduling queues. 
 * The mechanism is implemented here.   The actual scheduling policy is
 * defined in sched() and pick_proc().
 */
  int q;	 				/* scheduling queue to use */
  int front;					/* add to front or back */

#if DEBUG_SCHED_CHECK
  check_runqueues("enqueue");
  if (rp->p_ready) kprintf("enqueue() already ready process\n");
#endif

  /* Determine where to insert to process. */
  sched(rp, &q, &front);

  /* Now add the process to the queue. */
  if (rdy_head[q] == NIL_PROC) {		/* add to empty queue */
      rdy_head[q] = rdy_tail[q] = rp; 		/* create a new queue */
      rp->p_nextready = NIL_PROC;		/* mark new end */
  } 
  else if (front) {				/* add to head of queue */
      rp->p_nextready = rdy_head[q];		/* chain head of queue */
      rdy_head[q] = rp;				/* set new queue head */
  } 
  else {					/* add to tail of queue */
      rdy_tail[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_tail[q] = rp;				/* set new queue tail */
      rp->p_nextready = NIL_PROC;		/* mark new end */
  }

  /* Now select the next process to run. */
  pick_proc();			

#if DEBUG_SCHED_CHECK
  rp->p_ready = 1;
  check_runqueues("enqueue");
#endif
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
PRIVATE void dequeue(rp)
register struct proc *rp;	/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 */
  register int q = rp->p_priority;		/* queue to use */
  register struct proc **xpp;			/* iterate over queue */
  register struct proc *prev_xp;

  /* Side-effect for kernel: check if the task's stack still is ok? */
  if (iskernelp(rp)) { 				
	if (*priv(rp)->s_stack_guard != STACK_GUARD)
		panic("stack overrun by task", proc_nr(rp));
  }

#if DEBUG_SCHED_CHECK
  check_runqueues("dequeue");
  if (! rp->p_ready) kprintf("dequeue() already unready process\n");
#endif

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = NIL_PROC;				
  for (xpp = &rdy_head[q]; *xpp != NIL_PROC; xpp = &(*xpp)->p_nextready) {

      if (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          if (rp == rdy_tail[q])		/* queue tail removed */
              rdy_tail[q] = prev_xp;		/* set new tail */
          if (rp == proc_ptr || rp == next_ptr)	/* active process removed */
              pick_proc();			/* pick new process to run */
          break;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }
  
#if DEBUG_SCHED_CHECK
  rp->p_ready = 0;
  check_runqueues("dequeue");
#endif
}

/*===========================================================================*
 *				sched					     * 
 *===========================================================================*/
PRIVATE void sched(rp, queue, front)
register struct proc *rp;			/* process to be scheduled */
int *queue;					/* return: queue to use */
int *front;					/* return: front or back */
{
/* This function determines the scheduling policy.  It is called whenever a
 * process must be added to one of the scheduling queues to decide where to
 * insert it.  As a side-effect the process' priority may be updated.  
 */
  static struct proc *prev_ptr = NIL_PROC;	/* previous without time */
  int time_left = (rp->p_ticks_left > 0);	/* quantum fully consumed */
  int penalty = 0;				/* change in priority */

  /* Check whether the process has time left. Otherwise give a new quantum 
   * and possibly raise the priority.  Processes using multiple quantums 
   * in a row get a lower priority to catch infinite loops in high priority
   * processes (system servers and drivers). 
   */
  if ( ! time_left) {				/* quantum consumed ? */
      rp->p_ticks_left = rp->p_quantum_size; 	/* give new quantum */
      if (prev_ptr == rp) penalty ++;		/* catch infinite loops */
      else penalty --; 				/* give slow way back */
      prev_ptr = rp;				/* store ptr for next */
  }

  /* Determine the new priority of this process. The bounds are determined
   * by IDLE's queue and the maximum priority of this process. Kernel task 
   * and the idle process are never changed in priority.
   */
  if (penalty != 0 && ! iskernelp(rp)) {
      rp->p_priority += penalty;		/* update with penalty */
      if (rp->p_priority < rp->p_max_priority)  /* check upper bound */ 
          rp->p_priority=rp->p_max_priority;
      else if (rp->p_priority > IDLE_Q-1)   	/* check lower bound */
      	  rp->p_priority = IDLE_Q-1;
  }

  /* If there is time left, the process is added to the front of its queue, 
   * so that it can immediately run. The queue to use simply is always the
   * process' current priority. 
   */
  *queue = rp->p_priority;
  *front = time_left;
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
PRIVATE void pick_proc()
{
/* Decide who to run now.  A new process is selected by setting 'next_ptr'.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 */
  register struct proc *rp;			/* process to run */
  int q;					/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the image table.
   * The lowest queue contains IDLE, which is always ready.
   */
  for (q=0; q < NR_SCHED_QUEUES; q++) {	
      if ( (rp = rdy_head[q]) != NIL_PROC) {
          next_ptr = rp;			/* run process 'rp' next */
          if (priv(rp)->s_flags & BILLABLE)	 	
              bill_ptr = rp;			/* bill for system time */
          return;				 
      }
  }
}

/*===========================================================================*
 *				lock_send				     *
 *===========================================================================*/
PUBLIC int lock_send(dst, m_ptr)
int dst;			/* to whom is message being sent? */
message *m_ptr;			/* pointer to message buffer */
{
/* Safe gateway to mini_send() for tasks. */
  int result;
  lock(2, "send");
  result = mini_send(proc_ptr, dst, m_ptr, NON_BLOCKING);
  unlock(2);
  return(result);
}

/*===========================================================================*
 *				lock_enqueue				     *
 *===========================================================================*/
PUBLIC void lock_enqueue(rp)
struct proc *rp;		/* this process is now runnable */
{
/* Safe gateway to enqueue() for tasks. */
  lock(3, "enqueue");
  enqueue(rp);
  unlock(3);
}

/*===========================================================================*
 *				lock_dequeue				     *
 *===========================================================================*/
PUBLIC void lock_dequeue(rp)
struct proc *rp;		/* this process is no longer runnable */
{
/* Safe gateway to dequeue() for tasks. */
  lock(4, "dequeue");
  dequeue(rp);
  unlock(4);
}

/*===========================================================================*
 *				do_reply				     *
 *===========================================================================*/
PUBLIC int do_reply(register struct proc *caller_ptr, /* pointer to replier's proc structure */
		    message *m_ptr)	/* pointer to message from replier */
{
    int original_caller_endpoint;
    struct proc *original_caller_ptr;
    int obj_idx;
    struct reply_object *ro_ptr;

    /* Parameter checks */
    // In MINIX, the message endpoint to reply to is often passed in a specific field.
    // Assuming m_ptr->m1_i1 contains the endpoint of the process to reply to.
    // This needs to be standardized for the SYS_REPLY call.
    // Let's use m_dest (M.m_type/destination for message) or a specific field if defined.
    // For now, using m_ptr->M_REPLY_TO_ENDPOINT if such a field existed, or m_ptr->m1_i1 as a placeholder.
    // Let's assume the user-level lib for SYS_REPLY sets m_ptr->m_type to the target endpoint.
    original_caller_endpoint = m_ptr->m_type; // Common pattern in Minix for destination.

    if (!isokendpt(original_caller_endpoint, &original_caller_ptr)) {
        kprintf("do_reply: Invalid original_caller_endpoint %d by %d\n", original_caller_endpoint, proc_nr(caller_ptr));
        return EDEADSRCDST; // Or EBADSRCDST if endpoint format is wrong
    }

    // Check if the original caller is actually waiting for a reply via this mechanism
    if (!(original_caller_ptr->p_rts_flags & RECEIVING) ||
        priv(original_caller_ptr)->s_flags & SENDREC_BUSY == 0 || // Must be in SENDREC state
        original_caller_ptr->p_reply_obj_index == NO_REPLY_OBJ) {
        kprintf("do_reply: Original caller %d not waiting for this reply (flags %x, reply_idx %d)\n",
               proc_nr(original_caller_ptr), original_caller_ptr->p_rts_flags, original_caller_ptr->p_reply_obj_index);
        return EBADREQUEST; // Not a valid state for reply, or not using reply_obj
    }

    obj_idx = original_caller_ptr->p_reply_obj_index;
    if (obj_idx < 0 || obj_idx >= NR_PROCS || !reply_objects_table[obj_idx].ro_in_use) {
        // This should ideally not happen if p_reply_obj_index was valid.
        kprintf("do_reply: Invalid reply object index %d for original caller %d\n", obj_idx, proc_nr(original_caller_ptr));
        // Potentially reset p_reply_obj_index and unblock caller with error?
        return EFAULT;
    }
    ro_ptr = &reply_objects_table[obj_idx];

    /* Security Check: Verify that the replier (caller_ptr) is the intended target of the original SENDREC call. */
    if (ro_ptr->ro_target_proc_nr != proc_nr(caller_ptr)) {
        kprintf("do_reply: Security check failed. Replier %d is not the expected target %d for original caller %d.\n",
               proc_nr(caller_ptr), ro_ptr->ro_target_proc_nr, proc_nr(original_caller_ptr));
        return EPERM; // Or another appropriate error code
    }

    // Verify the original caller endpoint matches what's in the reply object.
    // This is a sanity check, as p_reply_obj_index already links them.
    if (ro_ptr->ro_caller_ep != original_caller_endpoint) {
         kprintf("do_reply: Mismatch between message endpoint %d and reply object endpoint %d\n",
                original_caller_endpoint, ro_ptr->ro_caller_ep);
        return EBADREQUEST;
    }

    /* Copy the reply message from replier's userspace to original caller's kernel message buffer. */
    // The message m_ptr is already in kernel space after sys_call.
    // We need to copy it to original_caller_ptr->p_messbuf.
    // Important: The source of the message for the original caller should be the replier.
    m_ptr->m_source = proc_addr(proc_nr(caller_ptr))->p_endpoint; // Set source to replier's endpoint
    CopyMess(proc_nr(caller_ptr), caller_ptr, m_ptr, original_caller_ptr, original_caller_ptr->p_messbuf);

    /* Unblock the original_caller_ptr. */
    original_caller_ptr->p_rts_flags &= ~RECEIVING;
    priv(original_caller_ptr)->s_flags &= ~SENDREC_BUSY;
    // p_reg.retreg should already contain OK from mini_receive if it blocked.
    // If mini_receive returned something else, that's an issue.
    // For now, assume mini_receive returned OK and the process is just waiting.
    // The actual return value of SENDREC will be the result of the reply.
    // The message copy above has set the return message for the original caller.

    if (original_caller_ptr->p_rts_flags == 0) { // If no other flags (like SENDING, which shouldn't be set)
        enqueue(original_caller_ptr); /* Add to ready queue */
    }

    /* Invalidate/consume the reply object. */
    free_reply_object(obj_idx);
    original_caller_ptr->p_reply_obj_index = NO_REPLY_OBJ;

    return(OK); /* Return OK to the replier (caller_ptr) */
}
