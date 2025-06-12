#------------------------------------------------------------------------------
# minix/lib/klib/meson.build
#
# Static libraries for MINIX kernel:
#  - capability_klib : capability‐verification, math syscalls, auditing, cache
#  - klib            : core kernel support (memory, strings, panic, debug, etc.)
#------------------------------------------------------------------------------

# Allow overriding optimization level in parent project
opt_level = get_option('opt_level')

#------------------------------------------------------------------------------  
# Include directories  
#------------------------------------------------------------------------------  
# Kernel headers (for both libraries)
common_includes = include_directories(
  'include',               # lib/klib public headers
  '../../kernel/include'   # kernel headers for capability
)

#------------------------------------------------------------------------------  
# capability_klib  
#------------------------------------------------------------------------------  
# Sources for the capability verification & math‐syscall library
cap_klib_sources = [
  'src/kmemory_c23.c',           # memory helpers in C23
  'src/kstring_c23.c',           # string routines in C23
  'src/kcapability_dag.c',       # capability‐DAG implementation
  'src/kcapability_dag_test.c',  # self‐tests for DAG
  'src/kcapability_posix.c',     # POSIX compatibility shims
  'src/kcrypto.c',               # basic cryptographic utilities
  'src/khelpers.c'               # miscellaneous helpers
]

# Compiler arguments for capability_klib
cap_klib_c_args = [
  '-std=c23',                    # enforce C23
  '-DKERNEL_SPACE',              # kernel‐only build
  '-DKDEBUG',                    # enable debug macros
  '-DMATHEMATICAL_VERIFICATION', # enable verification hooks
  '-O' + opt_level               # optimization level from top‐level
]

# Build the static capability library
capability_klib = static_library(
  'capability_klib',             # library name
  cap_klib_sources,
  include_directories : common_includes,
  c_args              : cap_klib_c_args,
  install             : false     # not installed to user‐space
)

# Exportable dependency object for Meson subprojects
capability_klib_dep = declare_dependency(
  include_directories : common_includes,
  link_with           : capability_klib
)

#------------------------------------------------------------------------------  
# klib (core kernel support)  
#------------------------------------------------------------------------------  
# Sources for the core kernel support library
core_klib_sources = [
  'src/kmemory_c23.c',                   # duplicated for core, reuses C23
  'src/kstring_c23.c',                   # as above
  'src/kcore.c',                         # core routines (alloc, free)
  'src/kconv.c',                         # numeric conversions
  'src/kdebug.c',                        # debug print & panic
  'src/kcapability_dag.c',               # shared with capability lib
  'src/kassert_metrics.c',               # assertion & metrics stubs
  'arch/i386/kcpu_detect_features_arch.c' # arch‐specific CPU detection
]

# Compiler arguments for core klib
core_klib_c_args = [
  '-std=c23',            # unify on C23
  '-ffreestanding',      # freestanding, no libc
  '-fno-builtin',        # no implicit builtins
  '-Wall',               # all warnings
  '-Wextra',             # extra warnings
  '-Werror',             # treat warnings as errors
  '-O' + opt_level,      # optimization
  '-D_MINIX_KERNEL',     # kernel‐mode build
  '-D_KLIB'              # klib build flag
]

# Include directory for klib public headers
klib_inc = include_directories('include')

# Build the static core library
klib_lib = static_library(
  'klib', 
  core_klib_sources,
  include_directories : klib_inc,
  c_args              : core_klib_c_args,
  install             : false
)

# Exportable dependency for other Meson projects
klib_dep = declare_dependency(
  link_with           : klib_lib,
  include_directories : klib_inc
)

#------------------------------------------------------------------------------
# EOF: minix/lib/klib/meson.build
#------------------------------------------------------------------------------
