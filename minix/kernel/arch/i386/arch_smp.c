/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#include <machine/cmos.h>
#include <machine/bios.h>

#include <minix/kernel_types.h>
#include <sys/kassert.h>
#include <klib/include/kprintf.h> // Provides kprintf, kprintf_stub might be a weak alias or separate
#include <klib/include/kstring.h> // For strcmp, kmemcpy
#include <klib/include/kmemory.h> // For phys_copy etc.

#include "kernel/spinlock.h"
#include "kernel/smp.h"
#include "apic.h"
#include "acpi.h"
#include "kernel/clock.h"

#include "glo.h"

#include <minix/clhlock.h> // For CLH types and functions
#include "kernel/proc.h"    // For struct proc definition, BEG_PROC_ADDR, END_PROC_ADDR, proc_addr, isemptyp
#include <stdatomic.h>      // For atomic_store_explicit, atomic_init etc.
#include "kernel/kernel.h"  // For get_cpulocal_var for cpuid
#include "arch_proto.h"     // For arch_pause()

// If kprintf_stub is not provided by kprintf.h or elsewhere as a weak alias for kprintf for example
#if !defined(kprintf_stub)
void kprintf_stub(const char *fmt, ...) {
    // Minimal stub, actual kernel should provide a real kprintf or kprintf_stub
    (void)fmt;
}
#endif


void trampoline(void);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
extern volatile u32_t __ap_id, __ap_pt;
extern volatile struct desctableptr_s __ap_gdt, __ap_idt;
extern u32_t __ap_gdt_tab, __ap_idt_tab;
extern void * __trampoline_end;

extern u32_t busclock[CONFIG_MAX_CPUS];
extern int panicking;

static int volatile ap_cpu_ready;
static int volatile cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
static unsigned char apicid2cpuid[255];
unsigned char cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

static void smp_reinit_vars(void);

/* These are initialized in protect.c */
extern struct segdesc_s gdt[GDT_SIZE];
extern struct gatedesc_s idt[IDT_SIZE];
extern struct tss_s tss[CONFIG_MAX_CPUS];
extern int prot_init_done;	/* Indicates they are ready */

static phys_bytes trampoline_base;

static u32_t ap_lin_addr(void *vaddr)
{
	KASSERT(trampoline_base);
	return (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
void copy_trampoline(void)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	KASSERT(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	KASSERT(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	KASSERT(prot_init_done);
	kmemcpy(&__ap_gdt_tab, gdt, sizeof(gdt));
	kmemcpy(&__ap_idt_tab, idt, sizeof(idt)); // Corrected from gdt to idt
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = sizeof(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = sizeof(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

extern int booting_cpu;	/* tell protect.c what to do */

static void smp_start_aps(void)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	struct proc *bootstrap_pt = get_cpulocal_var(ptproc);

	phys_copy(BIOS_RESET_VECTOR_ADDR, (phys_bytes) &biosresetvector, sizeof(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX_REG, CMOS_RTC_SHUTDOWN_REG); // Use manifest constants
	outb(RTC_DATA_REG, CMOS_RTC_SHUTDOWN_JUMP); // Use manifest constants for 0xA

	KASSERT(bootstrap_pt);
	KASSERT(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	KASSERT(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, BIOS_RESET_VECTOR_ADDR, sizeof(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	for (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		if((apic_id() == cpuid2apicid[cpu]) && // Using apic_id() from apic.h
				(apic_id() == bsp_lapic_id)) {
			continue;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, sizeof(__ap_id));
		mfence(); // Ensure __ap_id write is visible before AP starts
		if (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			kprintf("WARNING cannot boot cpu %d (INIT/SIPI failed)\n", cpu);
			continue;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		while (lapic_read(LAPIC_TIMER_CCR_OFF)) { // Use offset for LAPIC registers
			if (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				break;
			}
		}
		if (ap_cpu_ready == -1) {
			kprintf("WARNING : CPU %d didn't boot (timeout)\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, BIOS_RESET_VECTOR_ADDR, sizeof(u32_t));

	outb(RTC_INDEX_REG, CMOS_RTC_SHUTDOWN_REG); // Use manifest constants
	outb(RTC_DATA_REG, CMOS_RTC_SHUTDOWN_NONE); // Use manifest constants for 0

	bsp_finish_booting();
	NOT_REACHABLE;
}

void smp_halt_cpu (void) 
{
	NOT_IMPLEMENTED; // This should be a proper halt
}

void smp_shutdown_aps(void)
{
	unsigned cpu;

	if (ncpus == 1)
		goto exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK(); // Make sure BKL is defined and can be used here

	for (cpu = 0; cpu < ncpus; cpu++) {
		if (cpu == get_cpulocal_var(cpu_id)) // Use current CPU ID getter
			continue;
		if (!cpu_test_flag(cpu, CPU_IS_READY)) {
			kprintf("CPU %d didn't boot, not halting.\n", cpu);
			continue;
		}

		cpu_down = -1;
		barrier(); // Ensure cpu_down is visible
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		while (cpu_down != cpu); // busy wait, consider timeout
		kprintf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = NULL; // Use NULL for pointers
    lapic_eoi_addr = NULL; // Use NULL for pointers
	return;
}

static void ap_finish_booting(void)
{
	unsigned cpu = get_cpulocal_var(cpu_id); // Ensure this is set for APs

	/* inform the world of our presence. */
	ap_cpu_ready = cpu;

	spinlock_lock(&boot_lock);
	BKL_LOCK();

	kprintf("CPU %d is up\n", cpu);

	cpu_identify();

	lapic_enable(cpu); // lapic_enable should take current cpu_id
	fpu_init();

	if (ap_cpu_init_timer(system_hz)) { // app_cpu_init_timer is a typo in user code, should be ap_
		panic("FATAL : failed to initialize timer interrupts CPU %d, "
				"cannot continue without any clock source!", cpu);
	}

	get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);

	ap_boot_finished(cpu);
	spinlock_unlock(&boot_lock);

	switch_to_user();
	NOT_REACHABLE;
}

void smp_ap_boot(void)
{
    // __ap_id is set by BSP before AP starts. It's the logical CPU ID for this AP.
	switch_k_stack((char *)get_k_stack_top(__ap_id) -
			X86_STACK_TOP_RESERVED, ap_finish_booting);
}

static void smp_reinit_vars(void)
{
	lapic_addr = NULL;
    lapic_eoi_addr = NULL;
	ioapic_enabled = 0;
	ncpus = 1;
}

static void tss_init_all(void)
{
	unsigned cpu;
	for(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_stack_top(cpu)); 
}

static int discover_cpus(void)
{
	struct acpi_madt_lapic *cpu_info_lapic_entry; // Renamed to avoid conflict

	// Reset ncpus for discovery if this can be called multiple times or ensure it's 0
	ncpus = 0;
	while (ncpus < CONFIG_MAX_CPUS && (cpu_info_lapic_entry = acpi_get_lapic_next())) {
		apicid2cpuid[cpu_info_lapic_entry->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu_info_lapic_entry->apic_id;
		kprintf("CPU %3d: Local APIC ID %3d\n", ncpus, cpu_info_lapic_entry->apic_id);
		ncpus++;
	}
	return ncpus;
}

void smp_init (void)
{
	if (!discover_cpus() || ncpus < 1) { // Ensure at least one CPU (BSP) is found
		ncpus = 1;
		goto uniproc_fallback;
	}

	lapic_addr = (lapic_t*) LOCAL_APIC_DEF_ADDR; // Cast to lapic_t*
	ioapic_enabled = 0;

	tss_init_all();

    // Initialize the chosen per-process lock implementation
	init_proclock_impl(CONFIG_PROCLOCK_IMPL); // Use defined config string

	bsp_cpu_id = apicid2cpuid[apic_id()]; // apic_id() from apic.h
    get_cpulocal_var(cpu_id) = bsp_cpu_id; // Set BSP's own CPU ID

	if (!lapic_enable(bsp_cpu_id)) { // lapic_enable should take current cpu_id
		kprintf("ERROR : failed to initialize BSP Local APIC\n");
		goto uniproc_fallback;
	}

	bsp_lapic_id = apic_id();
	
	acpi_init();

	if (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = NULL;
		goto uniproc_fallback;
	}

	ioapic_enable_all();
	
	if (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0);
	idt_reload();

	BOOT_VERBOSE(kprintf("SMP initialized with %d CPUs\n", ncpus));

	if (ncpus > 1) {
	    switch_k_stack((char *)get_k_stack_top(bsp_cpu_id) -
			    X86_STACK_TOP_RESERVED, smp_start_aps);
    } else {
        bsp_finish_booting(); // No APs to start, BSP finishes directly
    }
	
	return;

uniproc_fallback:
	apic_idt_init(1);
	idt_reload();
	smp_reinit_vars ();
	intr_init(0);
	kprintf("WARNING : SMP initialization failed, falling back to uniprocessor mode.\n");
    bsp_finish_booting(); // BSP finishes booting in UP mode
}
	
void arch_smp_halt_cpu(void)
{
	cpu_down = get_cpulocal_var(cpu_id); // Use current CPU ID getter
	barrier();
	BKL_UNLOCK(); // Make sure BKL is defined
	for(;;);
}

void arch_send_smp_schedule_ipi(unsigned cpu_target) // Renamed arg for clarity
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu_target, APIC_IPI_DEST);
}

/* ========================================================================= */
/* CLH Lock integration for per-process locks                              */
/* ========================================================================= */

static void init_clh_state(struct proc *p)
{
    // Use clh_proc_state_t as defined in minix/clhlock.h
    clh_proc_state_t *state = &p->p_clh_state;
    state->current_node_idx = 0;
    // my_node will be set by clhlock_lock before clhlock_unlock needs it.
    // Initializing to one of the nodes is fine.
    state->my_node = &state->nodes[0];
    state->my_pred = NULL;

    atomic_init(&state->nodes[0].locked, false);
    atomic_init(&state->nodes[1].locked, false);
}

static void _clh_lock_proc(struct proc *p)
{
    // p->p_lock is now clhlock_t in struct proc
    clhlock_lock(&p->p_lock, &p->p_clh_state);
}

static void _clh_unlock_proc(struct proc *p)
{
    clhlock_unlock(&p->p_lock, &p->p_clh_state);
}

// Define proclock_impl_t if not defined elsewhere (e.g. kernel/smp.h)
#ifndef PROCLOCK_IMPL_T_DEFINED
#define PROCLOCK_IMPL_T_DEFINED
typedef struct proclock_impl_t {
    void (*lock_proc)(struct proc *p);
    void (*unlock_proc)(struct proc *p);
    // Add trylock if needed: int (*trylock_proc)(struct proc *p);
    // void (*assert_locked)(struct proc *p);
} proclock_impl_t;
#endif

struct proclock_impl_t proclock_impl;

void init_proclock_impl(const char *name)
{
    kprintf("init_proclock_impl choosing lock type: %s\n", name);
    if (!strcmp(name, "clh")) {
        proclock_impl.lock_proc = _clh_lock_proc;
        proclock_impl.unlock_proc = _clh_unlock_proc;
        // TODO: proclock_impl.trylock_proc = _clh_trylock_proc; (needs wrapper)
        // TODO: proclock_impl.assert_locked = _clh_assert_locked_proc; (needs wrapper)

        struct proc *p;
        kprintf("Initializing CLH locks for all process slots...\n");
        for (p = BEG_PROC_ADDR; p < END_PROC_ADDR; p++) {
            // isemptyp(p) might not be fully reliable if p_rts_flags isn't set yet for all.
            // However, initializing all is generally safe.
            clhlock_init(&p->p_lock);
            init_clh_state(p);
        }
        kprintf("Kernel configured to use CLH per-process locks.\n");
    } else if (!strcmp(name, "spinlock")) {
        // Placeholder for standard spinlock implementation
        // proclock_impl.lock_proc = _arch_spin_lock_proc;
        // proclock_impl.unlock_proc = _arch_spin_unlock_proc;
        kprintf("Kernel configured to use Spinlock per-process locks (Spinlock actual implementation pending).\n");
        // As a fallback, ensure proc locks do something safe or nothing.
        // For now, let's make them no-ops if not CLH to avoid null ptr calls.
        // A real system would have a default spinlock implementation.
        proclock_impl.lock_proc = NULL; // Or a dummy function
        proclock_impl.unlock_proc = NULL; // Or a dummy function
    } else {
        kprintf("Warning: Unknown proclock type '%s'. Defaulting to no-op process locks.\n", name);
        proclock_impl.lock_proc = NULL;
        proclock_impl.unlock_proc = NULL;
        // panic("Unknown proclock type selected for process locks");
    }
}
