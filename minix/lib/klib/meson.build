# minix/lib/klib/meson.build
project('capability_klib', 'c') // Renamed project

klib_sources = [
    'src/kmemory_c23.c',
    'src/kstring_c23.c',
    'src/kcapability_dag.c',
    'src/kcapability_dag_test.c',
    'src/kcapability_posix.c',
    'src/kcrypto.c',
    'src/khelpers.c'
]

klib_include_dirs = include_directories('include')

capability_klib = static_library('capability_klib', // Renamed library
    klib_sources,
    include_directories: klib_include_dirs,
    c_args: ['-std=c23', '-DKERNEL_SPACE', '-DKDEBUG',
             '-DMATHEMATICAL_VERIFICATION']
    # pic: false // Explicitly false if this lib is kernel only and won't be linked to user space directly
)

capability_klib_dep = declare_dependency( // Renamed dependency variable
    include_directories: klib_include_dirs,
    link_with: capability_klib
)
