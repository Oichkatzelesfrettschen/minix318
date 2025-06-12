# Meson build configuration for the MINIX project
# The kernel sources are written in C11 and C23 with C11 --> C23 migration in progress.

project('minix', 'c', default_options : ['c_std=c2x'])
# Configure global optimization

# Detect C23 support and adjust the compilation standard accordingly.
cc = meson.get_compiler('c')
if cc.has_argument('-std=c23')
  add_project_arguments('-std=c23', language: 'c')
else
  # Fall back to C17 when the compiler lacks C23 support.
  add_project_arguments('-std=c17', language: 'c')
endif

# Select the build architecture.
arch = get_option('arch')

# Export the architecture to all subdirectories.
add_project_arguments('-DARCH=' + arch, language: 'c')
add_project_arguments('-O' + get_option('opt_level'), language: 'c')

# Add the MINIX source directory to the build.
subdir('minix')
