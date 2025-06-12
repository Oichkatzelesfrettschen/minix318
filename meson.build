# Meson build configuration for the MINIX project
# The kernel sources are written in C11.
project('minix', 'c', default_options : ['c_std=c2x'])
# Configure global optimization

# Select the build architecture.
arch = get_option('arch')

# Export the architecture to all subdirectories.
add_project_arguments('-DARCH=' + arch, language: 'c')
add_project_arguments('-O' + get_option('opt_level'), language: 'c')

# Add the MINIX source directory to the build.
subdir('minix')
