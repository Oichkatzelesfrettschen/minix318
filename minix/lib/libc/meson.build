```meson
#------------------------------------------------------------------------------
# minix/lib/libc/meson.build
#
# Builds the custom mathematical‐POSIX extensions to the MINIX C library.
# Defines a static component 'libc_custom_math' for new math/POSIX support.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------  
# Include Directories  
#------------------------------------------------------------------------------  
# - '.'                  : local headers for this component
# - '../../include'      : core MINIX libc headers (mathposix.h, ipc.h, callnr.h, etc.)
# - '../../include/posix': POSIX types & definitions (sys/types.h, unistd.h, ...)
libc_include_dirs = include_directories(
  '.',
  '../../include',
  '../../include/posix'
)

#------------------------------------------------------------------------------  
# Source Files  
#------------------------------------------------------------------------------  
# - mathematical_posix.c : POSIX wrappers with formal math verification
# - mathematical_init.c  : Initialization routines for the math subsystem
# - math_control.c       : Future control interfaces (to be implemented)
libc_sources = files(
  'sys-minix/mathematical_posix.c',
  'sys-minix/mathematical_init.c',
  'sys-minix/math_control.c'
)

#------------------------------------------------------------------------------  
# Compiler Arguments  
#------------------------------------------------------------------------------  
# Use C23 for portability within userland; treat warnings as errors
libc_c_args = [
  '-std=c23,
  '-Wall',
  '-Wextra',
  '-Werror'
]

#------------------------------------------------------------------------------  
# Static Library Definition  
#------------------------------------------------------------------------------  
# Builds a static component for mathematical/POSIX extensions.
libc_custom_math = static_library(
  'libc_custom_math',        # library name
  libc_sources,              # source files
  include_directories : libc_include_dirs,
  c_args              : libc_c_args,
  install             : true,           # install into system libc path if appropriate
  install_dir         : 'lib'           # adjust as needed for Minix userland
)

#------------------------------------------------------------------------------  
# Dependency Object  
#------------------------------------------------------------------------------  
# Provides include paths and link libraries for other Meson subprojects.
libc_custom_math_dep = declare_dependency(
  include_directories : libc_include_dirs,
  link_with           : libc_custom_math
)

#------------------------------------------------------------------------------  
# Notes / TODO  
#------------------------------------------------------------------------------  
# - In a full libc build, this file would be integrated into a shared/static libc.
# - 'math_control.c' is a placeholder for future interfaces; implement as required.
# - Adjust 'install_dir' to match your Minix filesystem layout.
#------------------------------------------------------------------------------
```
