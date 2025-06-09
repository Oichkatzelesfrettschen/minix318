# Meson build configuration for the MINIX project
# The kernel sources are written in C11.
project(
    'minix',
    'c',
    default_options: ['c_std=c11']
)

# Select the build architecture.
arch = get_option('arch')

# Export the architecture to all subdirectories.
add_project_arguments('-DARCH=' + arch, language: 'c')

# Add the MINIX source directory to the build.
subdir('minix')
