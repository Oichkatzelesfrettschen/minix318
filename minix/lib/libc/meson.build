project('libc', 'c',
  default_options : ['warning_level=2', 'c_std=c11']) # Using c11 for libc, can be c23 if desired

# Common C arguments for libc
# -D_MINIX_SYSTEM is often used for system libraries/servers.
# -D_POSIX_SOURCE or _POSIX_C_SOURCE might be needed for POSIX compliance.
libc_c_args = [
  '-D_MINIX_SYSTEM',
  '-D_POSIX_C_SOURCE=200809L',
  '-Wall',
  '-Wextra',
  # Add other flags as necessary for Minix libc
]

# Include directory for Minix-specific headers (e.g., <minix/com.h>, <minix/ipc.h>)
# This path is relative to minix/lib/libc/meson.build, going up to minix/ and then into include/
minix_includes = include_directories('../../include')

# Include directory for libc's own internal headers (if any, e.g., for arch-specific parts)
# For now, assume headers might be within the same directory or specific subdirs.
libc_internal_includes = include_directories(
    '.', # Current directory
    'sys-minix', # For system-specific call wrappers
    'arch/@0@'.format(host_machine.cpu_family()) # For arch-specific libc files, if any
    # Add other libc internal include paths here
)


# Source files for libc
# This is a partial list. A real libc would have many more files.
libc_sources = [
  # Standard C library functions (examples, many more would be here)
  # 'stdio/printf.c',
  # 'stdio/scanf.c',
  # 'stdlib/malloc.c',
  # 'stdlib/free.c',
  # 'string/memcpy.c',
  # 'string/strlen.c',
  # ... etc.

  # System call wrappers (Minix specific)
  'sys-minix/access.c',
  'sys-minix/brk.c',
  'sys-minix/chdir.c',
  'sys-minix/chmod.c',
  'sys-minix/chown.c',
  'sys-minix/chroot.c',
  'sys-minix/close.c',
  'sys-minix/dup.c',
  'sys-minix/execve.c',
  'sys-minix/_exit.c',
  'sys-minix/fcntl.c',
  'sys-minix/fork.c',
  'sys-minix/fstat.c',
  'sys-minix/fsync.c',
  'sys-minix/ftruncate.c',
  'sys-minix/getcwd.c',
  'sys-minix/getdents.c',
  'sys-minix/getgid.c',
  'sys-minix/getpid.c',
  'sys-minix/getuid.c',
  'sys-minix/ioctl.c',
  'sys-minix/kill.c',
  'sys-minix/link.c',
  'sys-minix/lseek.c',
  'sys-minix/mkdir.c',
  'sys-minix/mknod.c',
  'sys-minix/mount.c',
  'sys-minix/open.c',
  'sys-minix/pipe.c',
  'sys-minix/read.c',
  'sys-minix/reboot.c',
  'sys-minix/rename.c',
  'sys-minix/rmdir.c',
  'sys-minix/stat.c',
  'sys-minix/stime.c',
  'sys-minix/sync.c',
  'sys-minix/time.c',
  'sys-minix/times.c',
  'sys-minix/umask.c',
  'sys-minix/umount.c',
  'sys-minix/uname.c',
  'sys-minix/unlink.c',
  'sys-minix/utime.c',
  'sys-minix/wait.c',
  'sys-minix/waitpid.c',
  'sys-minix/write.c',
  # ... many more syscall wrappers

  # New file for mathematical capability based open
  # Purpose: Provides the libc interface for the SYS_MATH_OPEN kernel call.
  'sys-minix/mathematical_posix.c',
]

# Add architecture-specific sources if they exist
# Example: libc_sources += files('arch/' + host_machine.cpu_family() + '/setjmp.S')

# Static library definition for libc
libc_lib = static_library('c', libc_sources,
  c_args : libc_c_args,
  include_directories : [minix_includes, libc_internal_includes],
  install : true
)

# Dependency object for other Meson projects to use
libc_dep = declare_dependency(
  link_with : libc_lib,
  include_directories : [minix_includes, libc_internal_includes] # Programs linking libc also need minix headers
)

# For subproject access if libc_includes are needed directly by a parent project
meson.override_variable('libc_include_dirs', [minix_includes, libc_internal_includes])
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
