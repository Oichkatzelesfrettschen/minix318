# meson.build - Complete kernel build configuration for MINIX
project('minix-kernel', 'c',
    version: '3.4.1-unofficial',
    license: 'BSD-mixed',
    default_options: [
        'c_std=c23',
        'warning_level=3', # Level 3 can be very noisy initially
        'b_staticpic=false', # Position Independent Code for static libs, not usually for kernel
        'b_pie=false',       # Position Independent Executable, no for kernel
        'b_lto=false',       # Link Time Optimization, disable for kernel dev
        # 'buildtype=debugoptimized' # Common for development
    ]
)

# Compiler instance
cc = meson.get_compiler('c')

# Detect architecture
arch = host_machine.cpu_family()
if arch == 'x86_64'
    arch_subdir = 'x86_64'
    arch_defines = ['-D__x86_64__', '-DARCH_X86_64']
    # Flags for x86_64 kernel
    arch_flags = ['-m64', '-mcmodel=kernel', '-mno-red-zone',
                  '-sse', '-sse2', '-fpu' '-mmx', #universal features
                  '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls']
elif arch == 'x86'
    arch_subdir = 'i386'
    arch_defines = ['-D__i386__', '-DARCH_I386']
    # Flags for i386 kernel
    arch_flags = ['-m32', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls']
else
    error('Unsupported architecture: ' + arch + '. Supported: x86, x86_64')
endif

# Kernel-specific compiler flags (common)
kernel_common_flags = [
    '-ffreestanding',      # No standard library
    '-fno-builtin',        # No built-in functions (e.g. printf, memcpy)
    '-fno-stack-protector',# No stack canary
    '-nostdinc',           # No standard include dirs
    '-nostdlib',           # No standard startup files or libraries
    '-Wall',
    '-Wextra',
    '-Wno-unused-parameter',
    '-Wno-missing-field-initializers',
    # '-g', # Debug symbols
]

# Combine common kernel flags with arch-specific flags and defines
kernel_c_args = kernel_common_flags + arch_flags + arch_defines
kernel_link_args = arch_flags # Linker also needs basic arch flags

# Include directories (relative to this meson.build file, i.e., minix/kernel/)
kernel_includes = include_directories(
    'include',                     # For <minix/kernel_types.h> etc.
    'include/arch/' + arch_subdir, # For arch-specific headers under include/arch/
    '.'                            # Current directory, for top-level kernel headers
    # klib's includes will be handled by its dependency object klib_dep
)

# Subproject for klib (located in ../lib/klib relative to this kernel directory)
klib_proj = subproject('klib', default_options : ['warning_level=2'], required: true) # Assuming klib is now in ../lib/klib
klib_dep = klib_proj.get_variable('klib_dep')

# Main kernel sources (core)
# Note: main.c, utility.c were refactored.
# system.c is the top-level one, not the directory.
kernel_core_sources = files(
    'clock.c',
    'cpulocals.c',
    'debug.c',
    'interrupt.c',
    'main.c',
    'proc.c',
    'profile.c',
    'smp.c',
    'system.c', # Top-level system.c
    'system.c', # Top-level system.c
    'table.c',
    'usermapped_data.c',
    'utility.c',
    'watchdog.c',
    '../lib/klib/src/kcapability_dag_test.c' # Added klib DAG tests
)

# System call handler sources (from minix/kernel/system/)
# This assumes all .c files in 'system/' are syscall handlers.
# A more explicit list might be safer if there are non-handler .c files.
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
# This is the end of the first (presumably correct) syscall_sources block.
# The duplicated arch_sources_list and subsequent blocks will be removed by this diff.
    all_c_sources + kernel_asm_sources, # all_c_sources now defined once after cleanup
    dependencies: [klib_dep], # Use klib_dep from subproject
    link_args: kernel_link_args,
    c_args: kernel_c_args,
    include_directories: kernel_includes, # kernel_includes now cleaned
    install: true,
    install_dir: 'boot' # Or appropriate install path
)

# Output some information
message('Building MINIX kernel for ' + arch)
message('Kernel C_ARGS: ' + ' '.join(kernel_c_args))
message('Kernel LINK_ARGS: ' + ' '.join(kernel_link_args))
message('Linker script: ' + linker_script)

# TODO: Add handling for assembly files (.S)
# TODO: Add custom targets for things like procoffsets.cf if needed
# TODO: Define what to do if x86_64 sources are missing (e.g. error or allow empty build)
# End of the first (and correct) block of TODOs. All subsequent duplicated content is removed by this change.
