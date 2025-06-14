#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#minix / lib / klib / meson.build
#
#Static libraries for MINIX kernel:
#- capability_klib : capability verification, math syscalls, auditing, cache
#- klib : core kernel support(memory, strings, panic, debug, etc.)
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#Allow overriding the top‐level optimization level
opt_level = get_option('opt_level')

#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Common Include Directories
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Public headers for both libraries and kernel headers for capability
common_includes = include_directories(
  'include',
  '../../kernel/include'
)

#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#capability_klib
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Sources for the capability‐verification and math‐syscall library
cap_klib_sources = [
  'src/kmemory_c23.c',          
  'src/kstring_c23.c',          
  'src/kcapability_dag.c',
  'src/kcapability_dag_test.c',
  'src/kcapability_posix.c',
  'src/kcrypto.c',
  'src/khelpers.c'
]

#Compiler arguments for capability_klib
cap_klib_c_args = [
  '-std=c2x',                    # enforce C23 (use c2x for GCC)
  '-DKERNEL_SPACE',              # kernel‐only build
  '-DKDEBUG',                    # enable debug macros
  '-DMATHEMATICAL_VERIFICATION', # enable verification hooks
  '-O' + opt_level               # optimization level
]

#Build the static capability library
capability_klib = static_library(
  'capability_klib',
  cap_klib_sources,
  include_directories : common_includes,
  c_args              : cap_klib_c_args,
  install             : false     # not installed to user‐space
)

#Exportable dependency for Meson subprojects
capability_klib_dep = declare_dependency(
  include_directories : common_includes,
  link_with           : capability_klib
)

#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#klib(core kernel support)
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Sources for the core kernel support library
core_klib_sources = [
  'src/kmemory_c23.c',                   
  'src/kstring_c23.c',
  'src/kcore.c',
  'src/kconv.c',
  'src/kdebug.c',
  'src/kalloc.c',
  'src/kcapability_dag.c',
  'src/kassert_metrics.c',               
  'arch/i386/kcpu_detect_features_arch.c'
]

#Compiler arguments for klib
core_klib_c_args = [
  '-std=c2x',
  '-ffreestanding',     
  '-fno-builtin',       
  '-Wall',              
  '-Wextra',            
  '-Werror',            
  '-O' + opt_level,     
  '-D_MINIX_KERNEL',    
  '-D_KLIB'             
]

#Include directory for klib public headers
klib_inc = include_directories('include')

#Build the static core library
klib_lib = static_library(
  'klib',
  core_klib_sources,
  include_directories : klib_inc,
  c_args              : core_klib_c_args,
  install             : false
)

#Exportable dependency for other projects
klib_dep = declare_dependency(
  link_with           : klib_lib,
  include_directories : klib_inc
)

#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#Functional Gaps / TODO
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#- Ensure 'arch/i386/kcpu_detect_features_arch.c' is guarded or replaced for
#x86_64.
#- Introduce option for building PIC vs non - PIC if needed in future.
#- Consider refactoring duplicate sources(kmemory / kstring) into a shared
#helper.
#- Add Meson test targets for capability_klib and klib to verify correctness.
