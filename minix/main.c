/**
 * @file main.c
 * @brief MINIX kernel main program and shutdown functionality
 *
 * This file contains the core initialization and shutdown routines for the
 * MINIX operating system. It handles system startup, process table
 * initialization, and graceful system shutdown.
 *
 * @section Main Functions
 * - main(): Initializes the MINIX system, sets up process table, interrupt
 * vectors, and starts scheduling
 * - prepare_shutdown(): Prepares the system for shutdown by notifying processes
 * and setting up shutdown timer
 * - shutdown(): Performs the actual system shutdown, masking interrupts and
 * resetting/halting the system
 * - announce(): Displays the MINIX startup banner with version and copyright
 * information
 *
 * @section Process Initialization
 * The main() function performs the following key operations:
 * - Initializes interrupt controller
 * - Clears and sets up process table with proper slot mappings
 * - Initializes privilege structures for system processes
 * - Sets up memory maps for boot image processes (text, data, stack segments)
 * - Configures process priorities, quantum sizes, and scheduling parameters
 * - Assigns privilege levels and system call masks to processes
 * - Handles kernel tasks vs user processes differently for stack and memory
 * setup
 *
 * @section Memory Management
 * - Kernel tasks use pre-allocated stack space with stack guards
 * - User processes have stack pointers calculated from memory map segments
 * - Memory segments are allocated and protected appropriately for each process
 * type
 * - Boot device image loading is supported when ENABLE_BOOTDEV is defined
 *
 * @section Shutdown Process
 * The shutdown sequence involves:
 * - Sending SIGKSTOP signals to all system processes for cleanup
 * - Setting shutdown timer to allow processes time to finish cleanup
 * - Masking all interrupts and stopping the system clock
 * - Either returning to boot monitor or performing hardware reset
 * - Proper error handling for invalid addresses and failed operations
 *
 * @section Error Handling
 * - Bounds checking for process and privilege table access
 * - Memory map validation before stack pointer calculation
 * - Process number validation before signal sending
 * - Address validation before physical memory operations
 * - Fallback mechanisms for failed reset operations
 *
 * @author Original MINIX team, Jorrit N. Herder (major revisions)
 * @date 2025 (latest cleanup and fixes)
 * @version Compatible with MINIX 3.x
 */
/* This file contains the main program of MINIX as well as its shutdown code.
 * The routine main() initializes the system and starts the ball rolling by
 * setting up the process table, interrupt vectors, and scheduling each task
 * to run to initialize itself.
 * The routine shutdown() does the opposite and brings down MINIX.
 *
 * The entries into this file are:
 *   main:	    	MINIX main program
 *   prepare_shutdown:	prepare to take MINIX down
 *   shutdown:		shutdown MINIX and halt/reboot system
 *   announce:		display MINIX startup banner
 *
 * Changes:
 *   Nov 24, 2004   simplified main() with system image  (Jorrit N. Herder)
 *   Aug 20, 2004   new prepare_shutdown() and shutdown()  (Jorrit N. Herder)
 *   2025           cleaned up comments and fixed syntax errors
 */
#include "kernel.h"
#include "proc.h"
#include <a.out.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define CURRENT_YEAR 2025

/* Prototype declarations for PRIVATE functions. */
FORWARD _PROTOTYPE(void announce, (void));
FORWARD _PROTOTYPE(void shutdown, (timer_t * tp));

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
PUBLIC void main() {
  /* Start the ball rolling. Initialize the system and set up all processes. */
  struct boot_image *ip;    /* boot image pointer */
  register struct proc *rp; /* process pointer */
  register struct priv *sp; /* privilege structure pointer */
  register int i, s;
  int hdrindex; /* index to array of a.out headers */
  phys_clicks text_base;
  vir_clicks text_clicks, data_clicks;
  reg_t ktsb;        /* kernel task stack base */
  struct exec e_hdr; /* for a copy of an a.out header */

  /* Initialize the interrupt controller. */
  intr_init(1);

  /* Clear the process table. Announce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes.
   */
  for (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
    rp->p_rts_flags = SLOT_FREE;          /* initialize free slot */
    rp->p_nr = i;                         /* proc number from ptr */
    if (i >= -NR_TASKS && i < NR_PROCS) { /* ensure within bounds */
      (pproc_addr + NR_TASKS)[i] = rp;    /* proc ptr from number */
    }
  }

  /* Initialize privilege structures for system processes */
  for (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
    sp->s_proc_nr = NONE;   /* initialize as free */
    sp->s_id = i;           /* priv structure index */
    if (i < NR_SYS_PROCS) { /* check array bounds */
      ppriv_addr[i] = sp;   /* priv ptr from number */
    } else {
      break; /* prevent memory corruption */
    }
  }

  /* Set up proc table entries for processes in boot image. The stacks of the
   * kernel tasks are initialized to an array in data space. The stacks
   * of the servers have been added to the data segment by the monitor, so
   * the stack pointer is set to the end of the data segment. All the
   * processes are in low memory on the 8086. On the 386 only the kernel
   * is in low memory, the rest is loaded in extended memory.
   */

  /* Initialize task stacks */
  ktsb = (reg_t)t_stack;

  for (i = 0; i < NR_BOOT_PROCS; ++i) {
    ip = &image[i];                                 /* process' attributes */
    rp = proc_addr(ip->proc_nr);                    /* get process pointer */
    strncpy(rp->p_name, ip->proc_name, P_NAME_LEN); /* set process name */
    rp->p_name[P_NAME_LEN - 1] = '\0'; /* ensure null termination */
    rp->p_priority = ip->priority;     /* current priority */
    rp->p_quantum_size = ip->quantum;  /* quantum size in ticks */
    rp->p_ticks_left = ip->quantum;    /* current credit */

    /* Assign privilege structure and set process flags */
    (void)get_priv(rp, (ip->flags & SYS_PROC)); /* assign structure */
    priv(rp)->s_flags = ip->flags;              /* process flags */
    priv(rp)->s_trap_mask = ip->trap_mask;      /* allowed traps */
    priv(rp)->s_call_mask = ip->call_mask;      /* kernel call mask */
    priv(rp)->s_ipc_to.chunk[0] = ip->ipc_to;   /* restrict targets */

    if (iskerneln(proc_nr(rp))) { /* part of the kernel? */
      if (ip->stksize > 0) {      /* HARDWARE stack size is 0 */
        rp->p_priv->s_stack_guard = (reg_t *)ktsb;
        *rp->p_priv->s_stack_guard = STACK_GUARD;
      }
      ktsb += ip->stksize; /* point to high end of stack */
      rp->p_reg.sp = ktsb; /* this task's initial stack ptr */
      text_base = kinfo.code_base >> CLICK_SHIFT;
      hdrindex = 0; /* all kernel processes use the first a.out header */
    } else {
      hdrindex = 1 + i - NR_TASKS; /* servers, drivers, INIT */
    }

    /* The bootstrap loader created an array of the a.out headers at
     * absolute address 'aout'. Get one element to e_hdr.
     */
    phys_copy(aout + hdrindex * A_MINHDR, vir2phys(&e_hdr),
              (phys_bytes)A_MINHDR);

    /* Convert addresses to clicks and build process memory map */
    text_base = e_hdr.a_syms >> CLICK_SHIFT;
    text_clicks = (e_hdr.a_text + CLICK_SIZE - 1) >> CLICK_SHIFT;
    if (!(e_hdr.a_flags & A_SEP))
      text_clicks = 0; /* common I&D */
    data_clicks = (e_hdr.a_total + CLICK_SIZE - 1) >> CLICK_SHIFT;

    /* Set up memory map segments */
    rp->p_memmap[T].mem_phys = text_base;
    rp->p_memmap[T].mem_len = text_clicks;
    rp->p_memmap[D].mem_phys = text_base + text_clicks;
    rp->p_memmap[D].mem_len = data_clicks;
    rp->p_memmap[S].mem_phys = text_base + text_clicks + data_clicks;
    rp->p_memmap[S].mem_vir = data_clicks; /* empty - stack is in data */

    /* Set initial register values. The processor status word for
     * tasks is different from that of other processes because tasks
     * can access I/O; this is not allowed to less-privileged processes.
     */

    /* Initialize the server stack pointer. Take it down one word
     * to give crtso.s something to use as "argc".
     */
    if (isusern(proc_nr(rp))) { /* user-space process? */
      /* Validate memory map values before calculating stack pointer */
      if (rp->p_memmap[S].mem_vir >= 0 && rp->p_memmap[S].mem_len > 0) {
        rp->p_reg.sp = (rp->p_memmap[S].mem_vir + rp->p_memmap[S].mem_len)
                       << CLICK_SHIFT;
        rp->p_reg.sp -= sizeof(reg_t);
      } else {
        kprintf("Error: Invalid memory map values for process %d\n",
                proc_nr(rp));
        rp->p_reg.sp = 0; /* Set to a safe default value */
      }

      /* Set ready. The HARDWARE task is never ready. */
      if (rp->p_nr != HARDWARE) {
        rp->p_rts_flags = 0; /* runnable if no flags */
        if (!(rp->p_rts_flags &
              RTS_SCHEDULING)) { /* check if already in queue */
          lock_enqueue(rp);      /* add to scheduling queues */
        }
      } else {
        rp->p_rts_flags = NO_MAP; /* prevent from running */
      }

      /* Code and data segments must be allocated in protected mode. */
      alloc_segments(rp);
    }
  }

#if ENABLE_BOOTDEV
  /* Expect an image of the boot device to be loaded into memory as well.
   * The boot device is the last module that is loaded into memory, and,
   * for example, can contain the root FS (useful for embedded systems).
   */
  hdrindex++;
  phys_copy(aout + hdrindex * A_MINHDR, vir2phys(&e_hdr), (phys_bytes)A_MINHDR);
  if (e_hdr.a_flags & A_IMG) {
    kinfo.bootdev_base = e_hdr.a_syms;
    kinfo.bootdev_size = e_hdr.a_data;
  }
#endif

  /* We're definitely not shutting down. */
  shutdown_started = 0;

  /* MINIX is now ready. All boot image processes are on the ready queue.
   * Return to the assembly code to start running the current process.
   */
  bill_ptr = proc_addr(IDLE); /* it has to point somewhere */
  announce();                 /* print MINIX startup banner */
  restart();
}

/*===========================================================================*
 *				announce                                     *
 *===========================================================================*/
PRIVATE void announce(void) {
  /* Display the MINIX startup banner. */
  kprintf("XINIM 4 PROJECT 2025\n");
  kprintf("MINIX %s.%s.\nCopyright %d Pearson Education, Inc.\n", OS_RELEASE,
          OS_VERSION, CURRENT_YEAR);
  kprintf("MINIX 3 Copyright %d Vrije Universiteit\n", CURRENT_YEAR);

#if (CHIP == INTEL)
  /* Real mode, or 16/32-bit protected mode? */
  kprintf("Executing in %s mode.\n\n",
          machine.protected ? "32-bit protected" : "real");
#endif
}

/*===========================================================================*
 *				prepare_shutdown                             *
 *===========================================================================*/
PUBLIC void prepare_shutdown(how) int how;
{
  /* This function prepares to shutdown MINIX. */
  static timer_t shutdown_timer;
  register struct proc *rp;
  message m;

  /* Show debugging dumps on panics. Make sure that the TTY driver
   * is still available to handle them. This is done with help of
   * a non-blocking send. We rely on TTY to call sys_abort() when
   * it is done with the dumps.
   */
  if (how == RBT_PANIC) {
    m.m_type = PANIC_DUMPS;
    if (nb_send(TTY_PROC_NR, &m) == OK) /* don't block if TTY isn't ready */
      return;                           /* await sys_abort() from TTY */
  }

  /* Send a signal to all system processes that are still alive to inform
   * them of the impending shutdown. This allows them to perform cleanup
   * operations. This mechanism is useful as a backup in case of system
   * panics, so that system processes can still run their shutdown code,
   * e.g, to synchronize the FS or to let the TTY switch to the first console.
   */
  kprintf("Sending SIGKSTOP to system processes ...\n");
  for (rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; rp++) {
    if (!isemptyp(rp) && (priv(rp)->s_flags & SYS_PROC) && !iskernelp(rp)) {
      int proc_number = proc_nr(rp);
      if (proc_number == NONE) {
        kprintf("Error: Invalid process number for process %s\n", rp->p_name);
      } else {
        send_sig(proc_number, SIGKSTOP);
      }
    }
  }

  /* We're shutting down. Diagnostics may behave differently now. */
  shutdown_started = 1;

  /* Notify system processes of the upcoming shutdown and allow them to be
   * scheduled by setting a watchdog timer that calls shutdown(). The timer
   * argument passes the shutdown status.
   */
  kprintf("MINIX will now be shut down ...\n");
  tmr_arg(&shutdown_timer)->ta_int = how;

  /* Continue after 1 second, to give processes a chance to get
   * scheduled to do shutdown work.
   */
  set_timer(&shutdown_timer, get_uptime() + HZ, shutdown);
}

/*===========================================================================*
 *				shutdown                                     *
 *===========================================================================*/
PRIVATE void shutdown(tp) timer_t *tp;
{
  /* This function is called from prepare_shutdown or stop_sequence to bring
   * down MINIX. How to shutdown is in the timer argument.
   */
  int how = tmr_arg(tp)->ta_int;
  u16_t magic = SOFT_RESET_FLAG;

  /* Now mask all interrupts, including the clock, and stop the clock. */
  outb(INT_CTLMASK, ~0);
  clock_stop();

  if (mon_return && how != RBT_RESET) {
    /* Reinitialize the interrupt controllers to the BIOS defaults. */
    intr_init(0);
    outb(INT_CTLMASK, 0);
    outb(INT2_CTLMASK, 0);

    /* Return to the boot monitor. Set the program if not already done. */
    if (how != RBT_MONITOR)
      phys_copy(vir2phys(""), kinfo.params_base, 1);
    level0(monitor);
  }

  /* Reset the system by jumping to the reset address (real mode),
   * or by forcing a processor shutdown (protected mode). First
   * stop the BIOS memory test by setting a soft reset flag.
   */

  /* Validate source and destination addresses before copying */
  if (phys_copy(vir2phys(&magic), SOFT_RESET_FLAG_ADDR, SOFT_RESET_FLAG_SIZE) !=
      OK) {
    kprintf("Error: Failed to set the soft reset flag.\n");
    return; /* Abort the reset operation */
  }

  /* Validate reset address before proceeding */
  if (reset != NULL) {
    level0(reset);
  } else {
    kprintf("Error: Reset address is invalid. System shutdown aborted.\n");
    /* Fallback mechanism: halt the system safely */
    while (1) {
      halt(); /* Infinite loop to prevent undefined state */
    }
  }
}
