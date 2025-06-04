# Meson build file for the MINIX kernel
#
# The kernel is built as a static library.  The sources are
# grouped into core, system call handlers, and architecture-
# specific files for the i386 port.

# Source files for the core kernel implementation
kernel_sources = files(
    'clock.c',
    'cpulocals.c',
    'debug.c',
    'interrupt.c',
    'main.c',
    'proc.c',
    'profile.c',
    'smp.c',
    'system.c',
    'table.c',
    'usermapped_data.c',
    'utility.c',
    'watchdog.c'
)

# System call handler sources
system_sources = files(
    'system/do_abort.c',
    'system/do_clear.c',
    'system/do_copy.c',
    'system/do_devio.c',
    'system/do_diagctl.c',
    'system/do_endksig.c',
    'system/do_exec.c',
    'system/do_exit.c',
    'system/do_fork.c',
    'system/do_getinfo.c',
    'system/do_getksig.c',
    'system/do_irqctl.c',
    'system/do_kill.c',
    'system/do_mcontext.c',
    'system/do_memset.c',
    'system/do_privctl.c',
    'system/do_runctl.c',
    'system/do_safecopy.c',
    'system/do_safememset.c',
    'system/do_schedctl.c',
    'system/do_schedule.c',
    'system/do_setalarm.c',
    'system/do_setgrant.c',
    'system/do_settime.c',
    'system/do_sigreturn.c',
    'system/do_sigsend.c',
    'system/do_sprofile.c',
    'system/do_statectl.c',
    'system/do_stime.c',
    'system/do_times.c',
    'system/do_trace.c',
    'system/do_umap.c',
    'system/do_umap_remote.c',
    'system/do_update.c',
    'system/do_vdevio.c',
    'system/do_vmctl.c',
    'system/do_vtimer.c',
    'system/do_vumap.c',
)

# Architecture-specific sources for the i386 port
# Only the i386 architecture is currently supported by the Meson build.
arch_i386_sources = files(
    'arch/i386/acpi.c',
    'arch/i386/apic.c',
    'arch/i386/apic_asm.S',
    'arch/i386/arch_clock.c',
    'arch/i386/arch_do_vmctl.c',
    'arch/i386/arch_reset.c',
    'arch/i386/arch_smp.c',
    'arch/i386/arch_system.c',
    'arch/i386/arch_watchdog.c',
    'arch/i386/breakpoints.c',
    'arch/i386/debugreg.S',
    'arch/i386/direct_tty_utils.c',
    'arch/i386/do_iopenable.c',
    'arch/i386/do_readbios.c',
    'arch/i386/do_sdevio.c',
    'arch/i386/exception.c',
    'arch/i386/head.S',
    'arch/i386/i8259.c',
    'arch/i386/io_inb.S',
    'arch/i386/io_inl.S',
    'arch/i386/io_intr.S',
    'arch/i386/io_inw.S',
    'arch/i386/io_outb.S',
    'arch/i386/io_outl.S',
    'arch/i386/io_outw.S',
    'arch/i386/klib.S',
    'arch/i386/memory.c',
    'arch/i386/mpx.S',
    'arch/i386/oxpcie.c',
    'arch/i386/pg_utils.c',
    'arch/i386/pre_init.c',
    'arch/i386/protect.c',
    'arch/i386/trampoline.S',
    'arch/i386/usermapped_data_arch.c',
    'arch/i386/usermapped_glo_ipc.S',
)

# Combine the different source groups into the final list
all_kernel_sources = kernel_sources + system_sources + arch_i386_sources

# Build the kernel as a static library.
static_library(
    'minixkernel',
    all_kernel_sources,
    include_directories: include_directories(
        '.',
        'arch/i386',
        'system',
        '..',
        '../include',
        'arch/i386/include',
        '../include/arch/i386/include'
    )
)
