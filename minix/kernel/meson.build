#meson.build - Complete kernel build configuration for MINIX
#project('minix-kernel', 'c',
#version : '3.4.1-unofficial',
#license : 'BSD-mixed',
#default_options : [
#'c_std=c23', #Changed to c23 to match capability libs, or ensure compatibility
#'warning_level=3',
#'b_staticpic=false',
#'b_pie=false',
#'b_lto=false',
#]
#)

cc = meson.get_compiler('c')
#arch = host_machine.cpu_family() #use arch option from parent project
if arch == 'x86_64'
    arch_subdir = 'x86_64'
    arch_defines = ['-D__x86_64__', '-DARCH_X86_64']
    arch_flags = ['-m64', '-mcmodel=kernel', '-mno-red-zone', '-sse', '-sse2', '-fpu', '-mmx', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls']
elif arch == 'x86' or arch == 'i386'
    arch_subdir = 'i386'
    arch_defines = ['-D__i386__', '-DARCH_I386']
    arch_flags = ['-m32', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls']
else
    error('Unsupported architecture: ' + arch)
endif

kernel_common_flags = ['-ffreestanding', '-fno-builtin', '-fno-stack-protector', '-nostdinc', '-nostdlib', '-Wall', '-Wextra', '-Wno-unused-parameter', '-Wno-missing-field-initializers', '-O' + get_option('opt_level')]
kernel_c_args = kernel_common_flags + arch_flags + arch_defines
kernel_link_args = arch_flags

#Get dependency for capability_klib(defined in minix / lib / klib / meson.build)
#This assumes minix / meson.build called subdir('lib/klib') and exposes
#capability_klib_dep
#capability_klib_dep = dependency(
#'capability_klib',
#fallback : ['capability_klib', 'capability_klib_dep']
#)

#Add build option for profiling
#option_enable_profiling = get_option('enable_profiling')
#if option_enable_profiling
#add_project_arguments('-DCONFIG_PROFILING=1', language : 'c')
#message('Kernel profiling enabled.')
#else
#message('Kernel profiling disabled.')
#endif

#Add build option for MDLM
#option_enable_mdlm = get_option('enable_mdlm')
#if option_enable_mdlm
#add_project_arguments('-DCONFIG_MDLM=1', language : 'c')
#message('MDLM support enabled.')
#else
#message('MDLM support disabled.')
#endif

kernel_includes = include_directories(
    'include',
    '../include/arch/' + arch_subdir,
    '../lib/klib/include', # Existing klib includes
    '.',
    'capability' # For headers like capability_syscalls.h in minix/kernel/capability
)

#Existing Kernel library(klib)
klib_sources = files(
    'klib/kstring.c',
    'klib/kmemory.c',
    'klib/kprintf_stub.c',
    'klib/kpanic.c'
)
klib = static_library('klib', klib_sources, c_args: kernel_c_args, include_directories: kernel_includes, install: false)

#NEW : Kernel Capability Library sources
kernel_capability_sources = [
    'capability/capability_proof.c',
    'capability/capability_verify.c',
    'capability/math_syscalls.c',
    'capability/math_syscalls_extended.c', # Added this line
    'capability/capability_audit.c',
    'capability/capability_cache.c'
]

#NEW : Kernel Capability Library definition
kernel_capability_lib = static_library('kernel_capability',
    kernel_capability_sources,
    include_directories: kernel_includes, # Uses the same kernel_includes, which now includes 'capability' subdir
    c_args: kernel_c_args + ['-DMATHEMATICAL_VERIFICATION'], # Add specific defines if any, like from issue
    dependencies: [capability_klib_dep], # Depend on our new capability_klib
    install: false
)

#Main kernel sources(core)
kernel_core_sources = files(
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
    'watchdog.c',
    'clhlock.c',    # Generic CLH lock implementation
    'clh_bkl.c',    # CLH BKL specific instance and initialization
    'capability.c', # Capability table management
    'profile.c',    # Profiling framework (already added, ensure it's here)
    'k_posix.c',    # POSIX helper stubs
    'acpi_unified.c'
)

#System call handler sources
syscall_sources = []
syscall_files_find_result = run_command('find', 'system', '-maxdepth', '1', '-name', '*.c', '-print', check: true)
syscall_files_stdout = syscall_files_find_result.stdout().strip()
if syscall_files_stdout != ''
    syscall_files = syscall_files_stdout.split('\n')
    foreach f : syscall_files
        if f != ''
            syscall_sources += files(f)
        endif
    endforeach
endif

#Architecture - specific sources
arch_sources_list = []
if arch_subdir == 'i386'
  arch_sources_list = files(
    'arch/i386/apic.c',
    'arch/i386/arch_clock.c',
    'arch/i386/arch_do_vmctl.c',
    'arch/i386/arch_reset.c',
    'arch/i386/arch_smp.c',
    'arch/i386/arch_system.c',
    'arch/i386/arch_watchdog.c',
    'arch/i386/breakpoints.c',
    'arch/i386/direct_tty_utils.c',
    'arch/i386/do_iopenable.c',
    'arch/i386/do_readbios.c',
    'arch/i386/do_sdevio.c',
    'arch/i386/exception.c',
    'arch/i386/i8259.c',
    'arch/i386/memory.c',
    'arch/i386/oxpcie.c',
    'arch/i386/pg_utils.c',
    'arch/i386/pre_init.c',
    'arch/i386/protect.c',
    'arch/i386/usermapped_data_arch.c'
  )
elif arch_subdir == 'x86_64'
  noop_x86_64 = []
endif

#Define architecture - specific assembly sources
kernel_asm_sources = []
if arch == 'i386'
    kernel_asm_sources = files(
        'arch/i386/mpx.S',
        'arch/i386/head.S',
        'arch/i386/klib.S',
        'arch/i386/apic_asm.S'
    )
elif arch == 'x86_64'
    kernel_asm_sources = []
endif

#MDLM sources(initially empty, will be populated in later phases)
mdlm_sources = []
#if option_enable_mdlm
# #When MDLM source files are created, they will be added here:
#mdlm_sources += files(
#'mdlm_cap_dag.c', #MDLM Capability DAG component
# #'mdlm/thread_lattice.c', #Example future file
# #'mdlm/cap_dag.c', #Maybe meant proc_dag.c
# #'mdlm/security_lattice.c' #Example future file
#)
#message('MDLM sources enabled: mdlm_cap_dag.c')
#endif

all_c_sources = kernel_core_sources + syscall_sources + arch_sources_list + mdlm_sources
linker_script = meson.current_source_dir() / 'arch' / arch_subdir / 'kernel.lds'
if run_command('test', '-f', linker_script, check: false).returncode() != 0
    error('Linker script not found: ' + linker_script +
          '. Please ensure it exists for architecture ' + arch_subdir)
endif
kernel_link_args += ['-T', linker_script, '-Wl,--build-id=none']

#Build kernel executable
kernel = executable('kernel',
    all_c_sources + kernel_asm_sources,
    link_with: [klib, kernel_capability_lib], # Added kernel_capability_lib here
    link_args: kernel_link_args,
    c_args: kernel_c_args,
    include_directories: kernel_includes,
    install: true,
    install_dir: 'boot'
)

#Output some information
message('Building MINIX kernel for ' + arch)
message('Kernel C_ARGS: ' + ' '.join(kernel_c_args))
message('Kernel LINK_ARGS: ' + ' '.join(kernel_link_args))
message('Linker script: ' + linker_script)

#TODO : Add handling for assembly files(.S)
#TODO : Add custom targets for things like procoffsets.cf if needed
#TODO : Define what to do if x86_64 sources are missing(e.g.error or allow
#empty build)
