# minix/lib/klib/meson.build
# project('capability_klib', 'c') # Renamed project (legacy)

klib_sources = [
    'src/kmemory_c23.c',
    'src/kstring_c23.c',
    'src/kcapability_dag.c',
    'src/kcapability_dag_test.c',
    'src/kcapability_posix.c',
    'src/kcrypto.c',
    'src/khelpers.c'
]

klib_include_dirs = include_directories(['include', '../../kernel/include'])

capability_klib = static_library('capability_klib', # Renamed library
    klib_sources,
    include_directories: klib_include_dirs,
    c_args: [
        '-DKERNEL_SPACE',
        '-DKDEBUG',
        '-DMATHEMATICAL_VERIFICATION',
        '-O' + get_option('opt_level')
    ]
    # pic: false # Explicitly false if this lib is kernel only and won't be linked to user space directly
)

capability_klib_dep = declare_dependency( # Renamed dependency variable
    include_directories: klib_include_dirs,
    link_with: capability_klib
)

# project('klib', 'c',
#   default_options : ['warning_level=2', 'c_std=c23'])

# Common C arguments for klib
klib_c_args = [
  '-ffreestanding',
  '-fno-builtin',
  '-Wall',
  '-Wextra',
  '-O' + get_option('opt_level'),
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
# This makes 'klib_include_dir' available via klib_proj.get_variable() (legacy)
# meson.override_variable('klib_include_dir', klib_inc)
