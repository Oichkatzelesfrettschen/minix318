```meson
#------------------------------------------------------------------------------
# minix/lib/libc/meson.build
#
# Builds the custom mathematical‐POSIX extensions to the MINIX C library.
# Defines a static component 'libc_custom_math' that other parts of the system
# can link against for math‐verified POSIX functionality.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------  
# Options  
#------------------------------------------------------------------------------  
# Allow overriding the C standard for this component (default: C11)
option(
  'libc_c_std',
  type        : 'string',
  value       : 'c23',
  choices     : ['c11', 'c17', 'c23'],
  description : 'C standard to use when compiling libc custom math'
)

#------------------------------------------------------------------------------  
# Include Directories  
#------------------------------------------------------------------------------  
# - '.'            : internal headers for this component
# - '../../include': core MINIX libc headers (mathposix.h, ipc.h, callnr.h, etc.)
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
# - math_control.c       : Control interfaces (to be implemented)
libc_sources = files(
  'sys-minix/mathematical_posix.c',
  'sys-minix/mathematical_init.c',
  'sys-minix/math_control.c'
)

#------------------------------------------------------------------------------  
# Compiler Arguments  
#------------------------------------------------------------------------------  
# Uses the chosen C standard, enables all warnings as errors, and respects
# the top‐level optimization level.
libc_c_args = [
  '-std=' + get_option('libc_c_std'),
  '-Wall',
  '-Wextra',
  '-Werror',
  '-O' + get_option('opt_level')
]

#------------------------------------------------------------------------------  
# Static Library Definition  
#------------------------------------------------------------------------------  
# Builds a static library 'libc_custom_math' for mathematical/POSIX extensions.
libc_custom_math = static_library(
  'libc_custom_math',            # library name
  libc_sources,                  # source files
  include_directories : libc_include_dirs,
  c_args              : libc_c_args,
  install             : true,    # install into userland libc path
  install_dir         : 'lib'    # adjust to your Minix layout
)

#------------------------------------------------------------------------------  
# Dependency Object  
#------------------------------------------------------------------------------  
# Exposes include paths and link libraries for other Meson subprojects.
libc_custom_math_dep = declare_dependency(
  include_directories : libc_include_dirs,
  link_with           : libc_custom_math
)

#------------------------------------------------------------------------------  
# Build Notice  
#------------------------------------------------------------------------------  
message('Configured libc_custom_math with C standard: ' + get_option('libc_c_std'))
```
