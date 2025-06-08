/**
 * @file main.c
 * @brief MINIX kernel main program and system initialization
 *
 * This file contains the core initialization routines for the MINIX kernel.
 * It handles the complete boot process from early system setup to process
 * scheduling, including:
 * - Boot parameter processing and environment variable parsing
 * - Process table initialization for boot image processes
 * - Privilege and security setup for kernel tasks and system processes
 * - Memory management initialization
 * - Interrupt and timer system setup
 * - SMP (Symmetric Multi-Processing) configuration when enabled
 * - System shutdown and halt procedures
 *
 * Key Functions:
 * - kmain(): Primary kernel entry point that orchestrates the boot process
 * - bsp_finish_booting(): Completes boot on the bootstrap processor
 * - cstart(): Early system initialization before main kernel startup
 * - prepare_shutdown()/minix_shutdown(): System shutdown procedures
 * - announce(): Displays the MINIX startup banner
 * - env_get()/get_value(): Environment variable access for boot parameters
 *
 * The initialization process sets up all kernel tasks, system processes,
 * and user processes from the boot image, configures their privileges,
 * and transitions control to the scheduler to begin normal operation.
 *
 * @note This implementation uses kernel-specific string and memory functions
 *       (kstring.h, kmemory.h) instead of standard library functions.
 * @note SMP support is conditionally compiled based on CONFIG_SMP flag.
 * @note The kernel transitions from initialization mode to user mode scheduling
 *       at the end of the boot process.
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
 */
// #include <string.h> // Replaced by kstring.h
// #include <stdlib.h> // Replaced by katoi placeholder or removed
// #include <assert.h> // Replaced by KASSERT_PLACEHOLDER

// #include <string.h> // Replaced by kstring.h
// #include <stdlib.h> // Replaced by katoi placeholder or removed
// #include <assert.h> // Replaced by KASSERT_PLACEHOLDER

#include <minix/endpoint.h>
#include <machine/vmparam.h>
#include <minix/u64.h>
#include <minix/board.h>
#include <sys/reboot.h> // Keep for reboot flags like RB_POWERDOWN
#include <sys/reboot.h> // Keep for reboot flags like RB_POWERDOWN
#include "clock.h"
#include "direct_utils.h"
#include "hw_intr.h"
#include "arch_proto.h"

#ifdef CONFIG_SMP
#include "smp.h"
#endif
#ifdef USE_WATCHDOG
#include "watchdog.h"
#endif
#include "spinlock.h"

// Kernel includes added:
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <minix/kernel_types.h>
#include <sys/kassert.h>

// Includes for Capability DAG
#include "../lib/klib/include/klib.h" // For kprintf_stub, kpanic, and kcapability_dag_run_mathematical_tests
#include <minix/kcapability_dag.h> // For kcapability_dag_t and kcapability_dag_create
#include <minix/kcap.h> // For INITIAL_KERNEL_CAPABILITY_CAPACITY and kernel_capability_dag extern


/* dummy for linking */
char *** _penviron;

/* Prototype declarations for PRIVATE functions. */
static void announce(void);

// Definition of the global DAG pointer is NOT here, will be in kcap.c or similar.
// extern struct kcapability_dag* kernel_capability_dag; // Already in kcap.h

/**
 * @brief Initializes the mathematical capability framework.
 *
 * This function is called early during kernel startup to:
 * 1. Run mathematical verification tests for the Capability DAG implementation.
 * 2. Create and initialize the global kernel_capability_dag instance.
 * It panics if either the tests fail (implicitly via KASSERT in tests) or
 * if the global DAG cannot be created.
 */
void kernel_initialize_mathematical_foundation(void) {
    // Use kprintf_stub from klib.h for early kernel messages
    kprintf_stub("Initializing mathematical capability framework...\n");

    /*
     * Run mathematical verification tests to prove that our capability
     * DAG implementation maintains all required mathematical properties
     * in the actual kernel environment.
     * KASSERTs within the tests will panic on failure.
     */
    kcapability_dag_run_mathematical_tests();

    kprintf_stub("Capability DAG mathematical tests passed.\n");

    /*
     * Initialize the global kernel capability DAG that will manage
     * all system-wide capability relationships.
     * Note: kernel_capability_dag is declared extern in kcap.h
     * and will be defined globally in a separate .c file (e.g. kcap.c).
     */
    kernel_capability_dag = kcapability_dag_create(INITIAL_KERNEL_CAPABILITY_CAPACITY);

    if (!kernel_capability_dag) {
        kpanic("Failed to initialize kernel capability DAG - mathematical foundation compromised");
    }

    // Assuming kprintf_stub or a kprintf that can handle _BitInt directly or via casting
    // For safety, let's cast node_capacity for kprintf_stub.
    kprintf_stub("Mathematical capability framework initialized successfully.\n");
    kprintf_stub("Kernel capability DAG created with %u node capacity.\n",
            (unsigned int)kernel_capability_dag->node_capacity);
}


void bsp_finish_booting(void)
{
  int i;
#if SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#endif /* SPROFILE */

  cpu_identify();

  vm_running = 0;
  krandom.random_sources = RANDOM_SOURCES;
  krandom.random_elements = RANDOM_ELEMENTS;

  /* MINIX is now ready. All boot image processes are on the ready queue.
   * Return to the assembly code to start running the current process. 
   */
  
  /* it should point somewhere */
  get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);
  get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
  announce();				/* print MINIX startup banner */

  /*
   * we have access to the cpu local run queue, only now schedule the processes.
   * We ignore the slots for the former kernel tasks
   */
  for (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  cycles_accounting_init();

  if (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#if DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#endif
#if DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#endif
#if DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#endif

#ifdef CONFIG_SMP
  cpu_set_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_count = ncpus;
  machine.bsp_id = bsp_cpu_id;
#else
  machine.processors_count = 1;
  machine.bsp_id = 0;
#endif
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_to_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
void kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  struct boot_image *ip;	/* boot image pointer */
  register struct proc *rp;	/* process pointer */
  register int i, j;
  static int bss_test;

  /* bss sanity check */
  KASSERT(bss_test == 0);
  KASSERT(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  kmemcpy(&kinfo, local_cbi, sizeof(kinfo)); // MODIFIED
  kmemcpy(&kmess, kinfo.kmess, sizeof(kmess)); // MODIFIED
  kmemcpy(&kinfo, local_cbi, sizeof(kinfo)); // MODIFIED
  kmemcpy(&kmess, kinfo.kmess, sizeof(kmess)); // MODIFIED

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#ifdef __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#endif
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  KASSERT(sizeof(kinfo.boot_procs) == sizeof(image));
  kmemcpy(kinfo.boot_procs, image, sizeof(kinfo.boot_procs)); // MODIFIED
  KASSERT(sizeof(kinfo.boot_procs) == sizeof(image));
  kmemcpy(kinfo.boot_procs, image, sizeof(kinfo.boot_procs)); // MODIFIED

  cstart();

  // Initialize the chosen Big Kernel Lock implementation
  // (e.g., CLH BKL, if selected over older spinlock BKL)
  // This needs to be done before the first BKL_LOCK() call.
  extern void init_global_clh_bkl(void); // Declaration
  init_global_clh_bkl(); // Call CLH BKL initialization

  BKL_LOCK();

  // Initialize the mathematical capability foundation
  kernel_initialize_mathematical_foundation();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   if(NR_BOOT_MODULES != kinfo.mbi.mi_mods_count)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_count);

  /* Set up proc table entries for processes in boot image. */
  for (i=0; i < NR_BOOT_PROCS; ++i) {
	int schedulable_proc;
	proc_nr_t proc_nr;
	int ipc_to_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_time_left = 0;
	if(i < NR_TASKS)			/* name (tasks only) */
		kstrlcpy(rp->p_name, ip->proc_name, sizeof(rp->p_name)); // MODIFIED
		kstrlcpy(rp->p_name, ip->proc_name, sizeof(rp->p_name)); // MODIFIED

	if(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->start_addr = mb_mod->mod_start;
		ip->len = mb_mod->mod_end - mb_mod->mod_start;
	}
	
	reset_proc_accounting(rp);

	/* See if this process is immediately schedulable.
	 * In that case, set its privileges now and allow it to run.
	 * Only kernel tasks and the root system process get to run immediately.
	 * All the other system processes are inhibited from running by the
	 * RTS_NO_PRIV flag. They can only be scheduled once the root system
	 * process has set their privileges.
	 */
	proc_nr = proc_nr(rp);
	schedulable_proc = (iskerneln(proc_nr) || isrootsysn(proc_nr) ||
		proc_nr == VM_PROC_NR);
	if(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (void) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    if(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_mask = SRV_T;
		ipc_to_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_priority = SRV_Q;
                rp->p_quantum_size_ms = SRV_QT;
	    }
	    else if(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_mask = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_to_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            else {
		KASSERT(isrootsysn(proc_nr));
		KASSERT(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_mask= SRV_T;     /* allowed traps */
                ipc_to_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_priority = SRV_Q;	          /* priority queue */
                rp->p_quantum_size_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            kmemset(&map, 0, sizeof(map)); // MODIFIED
            kmemset(&map, 0, sizeof(map)); // MODIFIED

            if (ipc_to_m == ALL_M) {
                for(j = 0; j < NR_SYS_PROCS; j++)
                    set_sys_bit(map, j);
            }

            fill_sendto_mask(rp, &map);

            /* Fill in kernel call mask. */
            for(j = 0; j < SYS_CALL_MASK_SIZE; j++) {
                priv(rp)->s_k_call_mask[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	else {
	    /* Don't let the process run for now. */
            RTS_SET(rp, RTS_NO_PRIV | RTS_NO_QUANTUM);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	if(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	if(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_SLOT_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  kmemcpy(kinfo.boot_procs, image, sizeof(kinfo.boot_procs)); // MODIFIED
  kmemcpy(kinfo.boot_procs, image, sizeof(kinfo.boot_procs)); // MODIFIED

#define IPCNAME(n) { \
	KASSERT((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	KASSERT(!ipc_call_names[n]);	\
	KASSERT((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	KASSERT(!ipc_call_names[n]);	\
	ipc_call_names[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  memory_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  add_memmap(&kinfo, kinfo.bootstrap_start, kinfo.bootstrap_len);

#ifdef CONFIG_SMP
  if (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } else if (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } else {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_finish_booting();
  }
#else
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_finish_booting();
#endif

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
static void announce(void)
{
  /* Display the MINIX startup banner. */
  kprintf_stub("\nMINIX %s. " // MODIFIED
  kprintf_stub("\nMINIX %s. " // MODIFIED
#ifdef PAE
"(PAE) "
#endif
#ifdef _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#endif
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  kprintf_stub("MINIX is open source software, see http://www.minix3.org\n"); // MODIFIED
  kprintf_stub("MINIX is open source software, see http://www.minix3.org\n"); // MODIFIED
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
void prepare_shutdown(const int how)
{
/* This function prepares to shutdown MINIX. */
  static minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  kprintf_stub("MINIX will now be shut down ...\n"); // MODIFIED
  kprintf_stub("MINIX will now be shut down ...\n"); // MODIFIED
  set_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
void minix_shutdown(int how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#ifdef CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  if (ncpus > 1)
	  smp_shutdown_aps();
#endif
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  direct_cls();
  if((how & RB_POWERDOWN) == RB_POWERDOWN)
	direct_print("MINIX has halted and will now power off.\n");
  else if(how & RB_HALT)
	direct_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  else
	direct_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
void cstart(void)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  register char *value;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  if ((value = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = 0; /* FIXME: atoi(value) was here, replace with katoi */ // MODIFIED
	  verboseboot = 0; /* FIXME: atoi(value) was here, replace with katoi */ // MODIFIED

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  value = env_get("ac_layout");
  if(value && (*value != '0')) { /* FIXME: atoi(value) was here, simple check for non-zero for now */ // MODIFIED
  if(value && (*value != '0')) { /* FIXME: atoi(value) was here, simple check for non-zero for now */ // MODIFIED
        kinfo.user_sp = (vir_bytes) USR_STACKTOP_COMPACT;
        kinfo.user_end = (vir_bytes) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  kstrlcpy(kinfo.release, OS_RELEASE, sizeof(kinfo.release)); // MODIFIED
  kstrlcpy(kinfo.version, OS_VERSION, sizeof(kinfo.version)); // MODIFIED
  kstrlcpy(kinfo.release, OS_RELEASE, sizeof(kinfo.release)); // MODIFIED
  kstrlcpy(kinfo.version, OS_VERSION, sizeof(kinfo.version)); // MODIFIED

  /* Initialize various user-mapped structures. */
  kmemset(&arm_frclock, 0, sizeof(arm_frclock)); // MODIFIED
  kmemset(&arm_frclock, 0, sizeof(arm_frclock)); // MODIFIED

  kmemset(&kuserinfo, 0, sizeof(kuserinfo)); // MODIFIED
  kmemset(&kuserinfo, 0, sizeof(kuserinfo)); // MODIFIED
  kuserinfo.kui_size = sizeof(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#ifdef USE_APIC
  value = env_get("no_apic");
  if(value)
	config_no_apic = (*value != '0'); /* FIXME: atoi(value) was here */ // MODIFIED (default to true if value is non-zero)
	config_no_apic = (*value != '0'); /* FIXME: atoi(value) was here */ // MODIFIED (default to true if value is non-zero)
  else
	config_no_apic = 1; /* Default if not set */
	config_no_apic = 1; /* Default if not set */
  value = env_get("apic_timer_x");
  if(value)
	config_apic_timer_x = 0; /* FIXME: atoi(value) was here, replace with katoi */ // MODIFIED (default to 0, needs proper katoi)
	config_apic_timer_x = 0; /* FIXME: atoi(value) was here, replace with katoi */ // MODIFIED (default to 0, needs proper katoi)
  else
	config_apic_timer_x = 1; /* Default if not set */
	config_apic_timer_x = 1; /* Default if not set */
#endif

#ifdef USE_WATCHDOG
  value = env_get("watchdog");
  if (value)
	  watchdog_enabled = (*value != '0'); /* FIXME: atoi(value) was here */ // MODIFIED (default to true if value is non-zero)
	  watchdog_enabled = (*value != '0'); /* FIXME: atoi(value) was here */ // MODIFIED (default to true if value is non-zero)
#endif

#ifdef CONFIG_SMP
  if (config_no_apic)
	  config_no_smp = 1;
  value = env_get("no_smp");
  if(value)
	config_no_smp = (*value != '0'); /* FIXME: atoi(value) was here */ // MODIFIED (default to true if value is non-zero)
	config_no_smp = (*value != '0'); /* FIXME: atoi(value) was here */ // MODIFIED (default to true if value is non-zero)
  else
	config_no_smp = 0; /* Default if not set */
	config_no_smp = 0; /* Default if not set */
#endif
  DEBUGEXTRA(("intr_init(0)\n"));

  intr_init(0);

  arch_init();
}

/*===========================================================================*
 *				get_value				     *
 *===========================================================================*/

char *get_value(
  const char *params,			/* boot monitor parameters */
  const char *name			/* key to look up */
)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
  register const char *namep;
  register char *envp;

  for (envp = (char *) params; *envp != 0;) {
	for (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	if (*namep == '\0' && *envp == '=') return(envp + 1);
	while (*envp++ != 0)
		;
  }
  return(NULL);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
char *env_get(const char *name)
{
	return get_value(kinfo.param_buf, name);
}

void cpu_print_freq(unsigned cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (unsigned long)(freq / 1000000)));
}

int is_fpu(void)
{
        return get_cpulocal_var(fpu_presence);
}
