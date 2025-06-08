project('klib', 'c',
  default_options : ['warning_level=2', 'c_std=c23'])

# Common C arguments for klib
klib_c_args = [
  '-ffreestanding',
  '-fno-builtin',
  '-Wall',
  '-Wextra',
  '-Werror', # Treat warnings as errors
  '-D_MINIX_KERNEL',
  '-D_KLIB'
]

# Include directory for klib's own headers
klib_inc = include_directories('include')

# Source files for klib
klib_sources = [
  'src/kmemory_c23.c',
  'src/kstring_c23.c',
  'src/kcore.c',
  'src/kconv.c',
  'src/kdebug.c',
  'src/kcapability_dag.c',
  'src/kassert_metrics.c', # Assertion metrics collection stubs
  'arch/i386/kcpu_detect_features_arch.c'
]

# Static library definition
klib_lib = static_library('klib', klib_sources,
  c_args : klib_c_args,
  include_directories : klib_inc
)

# Dependency object for other Meson projects to use
klib_dep = declare_dependency(
  link_with : klib_lib,
  include_directories : klib_inc
)

# For subproject access if klib_inc is needed directly by the parent project
# This makes 'klib_include_dir' available via klib_proj.get_variable()
meson.override_variable('klib_include_dir', klib_inc)
