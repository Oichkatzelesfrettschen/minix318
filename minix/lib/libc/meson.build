# minix/lib/libc/meson.build
project('minix_libc', 'c')

libc_include_dirs = include_directories(
    '.',                            // For internal libc headers if any
    '../../include',                // For <mathposix.h>, <minix/ipc.h>, <minix/callnr.h> etc.
                                    // Adjust if mathposix.h is in minix/include/minix/
    '../../include/posix'           // If POSIX headers like <sys/types.h> are there
)

libc_sources = files(
    'sys-minix/mathematical_posix.c', // Moved in previous step
    'sys-minix/mathematical_init.c', // Added this line
    'sys-minix/math_control.c'        // Will be created in a future step
    // Other libc sources would be listed here in a full libc build
)

# Compiler arguments for libc (might differ from kernel)
# These should be relatively standard C flags.
libc_c_args = ['-std=c11'] # Defaulting to c11 as per root meson.build for project 'minix'
                            # The new C files mathematical_posix.c and math_control.c should be C11 compatible.


# Define libc. This might be a static library that gets linked into a final shared one,
# or directly the shared library if appropriate for Minix.
# For now, defining as a static library 'libc_component' for the parts we are adding.
# A real Minix build might have a more complex setup.
libc_component = static_library('libc_custom_math',
    libc_sources,
    include_directories: libc_include_dirs,
    c_args: libc_c_args,
    # dependencies: [] # Add any other lib dependencies if needed
)

# Declare a dependency for other parts of the build system if they need to link against these components.
# However, libc is usually special-cased by the linker or provided by a toolchain.
# For now, this makes our added components available if something else needs them explicitly.
libc_custom_math_dep = declare_dependency(
    include_directories: libc_include_dirs,
    link_with: libc_component
)

# If this Meson file is meant to build the *entire* libc, it would be much larger
# and would produce a shared library (if Minix uses shared libc) or a more comprehensive static lib.
# The goal here is to ensure our new files are compiled and linked appropriately.
