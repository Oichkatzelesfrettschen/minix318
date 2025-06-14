#------------------------------------------------------------------------------
# minix/lib/libc/meson.build
#
# Static component for mathematical/POSIX extensions to the MINIX C library.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------  
# Build options
#------------------------------------------------------------------------------
# The C standard for this component is controlled by the top-level
# 'libc_c_std' option declared in meson_options.txt.

#------------------------------------------------------------------------------  
# Include Directories  
#------------------------------------------------------------------------------  
# - '.'                  : local headers for this component
# - '../../include'      : core MINIX libc headers (mathposix.h, ipc.h, callnr.h, etc.)
# - '../../include/posix': POSIX types & definitions (sys/types.h, unistd.h, ...)
libc_include_dirs = include_directories(
  '.',
  '../../include'
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
# Use the selected C standard, enable all warnings as errors, and apply
# the top-level optimization level.
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
  'libc_custom_math',
  libc_sources,
  include_directories : libc_include_dirs,
  c_args              : libc_c_args,
  install             : true,        # install into userland libc path
  install_dir         : 'lib'        # adjust to match Minix layout
)

#------------------------------------------------------------------------------  
# Dependency Object  
#------------------------------------------------------------------------------  
# Exposes include paths and link libraries for downstream Meson projects.
libc_custom_math_dep = declare_dependency(
  include_directories : libc_include_dirs,
  link_with           : libc_custom_math
)

#------------------------------------------------------------------------------  
# Functional Gaps / TODO  
#------------------------------------------------------------------------------  
# - Integrate this component into the main libc build (shared/static libc).  
# - Verify that 'sys-minix' source files exist; add fallbacks or generate stubs.  
# - Populate math_control.c with actual control APIs or remove if unneeded.  
# - Add Meson test cases to validate mathematical_posix and initialization logic.  
#------------------------------------------------------------------------------
