# Meson build file for MINIX core components

# The project currently only builds the kernel.  Additional
# subdirectories can be added here when they gain Meson support.
arch = get_option('arch')

# Ensure that the build directory contains a 'machine' link pointing to
# the selected architecture's headers.  This mirrors the setup used by
# the historical make-based build system.
machine_src = join_paths(meson.current_source_dir(), 'include', 'arch', arch, 'include')
machine_dst = join_paths(meson.current_build_dir(), 'include', 'machine')

run_command('mkdir', '-p', join_paths(meson.current_build_dir(), 'include'))
run_command('ln', '-sfn', machine_src, machine_dst)

subdir('lib/klib') # Added for our new capability_klib
subdir('lib/acpi')
subdir('lib/libc') # Added this line
subdir('kernel')
