# MINIX4 Core Components Build Configuration

# Architecture handling - use detected architecture from parent
arch = target_machine.cpu_family()

# Create include directory structure for compatibility
include_build_dir = join_paths(meson.current_build_dir(), 'include')
machine_link = join_paths(include_build_dir, 'machine')

# Create include directory
run_command('mkdir', '-p', include_build_dir, check: false)

# Handle architecture-specific includes
if target_machine.system() != 'windows'
  # Create symlink on Unix-like systems
  arch_include_src = join_paths(meson.current_source_dir(), 'include', 'arch', arch, 'include')
  run_command('ln', '-sfn', arch_include_src, machine_link, check: false)
endif

# Build order: libraries first, then kernel, then optional components
subdir('lib/klib')
subdir('kernel')

# Optional components based on build options
if get_option('build_libc')
  subdir('lib/libc')
endif

if get_option('build_libos')
  subdir('libos')
endif

if get_option('build_servers')
  subdir('servers')
endif

if get_option('build_drivers')
  subdir('drivers')  
endif

if get_option('build_commands')
  subdir('commands')
endif
