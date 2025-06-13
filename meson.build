# MINIX4 Kernel Build Configuration
# Modern Meson build system for MINIX4 operating system
# Version: 4.0.0

project('minix4-kernel', 'c', 'cpp',
  version : '4.0.0',
  license : 'BSD-3-Clause',
  default_options : [
    'c_std=c23',
    'cpp_std=c++23',
    'warning_level=3',
    'werror=false',  # Start with false, enable as we fix warnings
    'optimization=2',
    'debug=true',
    'strip=false',
    'b_ndebug=if-release',
    'b_sanitize=none'  # Kernel code can't use sanitizers
  ],
  meson_version : '>=0.60.0'  # More widely available version
)

# Project information
project_description = 'MINIX4 Modern Microkernel Operating System'

# Configuration
conf_data = configuration_data()
conf_data.set('MINIX4_VERSION_MAJOR', 4)
conf_data.set('MINIX4_VERSION_MINOR', 0) 
conf_data.set('MINIX4_VERSION_PATCH', 0)
conf_data.set('MINIX4_VERSION_STRING', '"4.0.0"')

# Compiler setup
cc = meson.get_compiler('c')

# Check for C23 support, fallback to C2x/C17
if not cc.has_argument('-std=c23')
  if cc.has_argument('-std=c2x')
    warning('C23 not supported, falling back to C2x')
    add_project_arguments('-std=c2x', language: 'c')
  else
    warning('C23/C2x not supported, falling back to C17')
    add_project_arguments('-std=c17', language: 'c')
  endif
else
  add_project_arguments('-std=c23', language: 'c')
endif

# Target architecture detection
target_arch = target_machine.cpu_family()
target_os = target_machine.system()

message('Building for architecture: @0@'.format(target_arch))
message('Target OS: @0@'.format(target_os))

# Architecture-specific configuration
arch_defines = []
if target_arch == 'x86_64'
  arch_defines += ['-DARCH_X86_64', '-DARCH_POINTER_SIZE=8']
elif target_arch == 'x86'
  arch_defines += ['-DARCH_I386', '-DARCH_POINTER_SIZE=4']
elif target_arch.startswith('arm')
  if target_arch == 'aarch64'
    arch_defines += ['-DARCH_AARCH64', '-DARCH_POINTER_SIZE=8']
  else
    arch_defines += ['-DARCH_ARM', '-DARCH_POINTER_SIZE=4']
  endif
else
  message('Unsupported target architecture: @0@, continuing anyway'.format(target_arch))
  arch_defines += ['-DARCH_UNKNOWN', '-DARCH_POINTER_SIZE=8']
endif

# Add architecture defines globally
add_project_arguments(arch_defines, language: 'c')

# Common compiler flags for kernel
kernel_c_flags = [
  '-ffreestanding',
  '-fno-builtin',
  '-fno-stack-protector', 
  '-Wall',
  '-Wextra',
  '-Wno-unused-parameter',
  '-fdata-sections',
  '-ffunction-sections'
]

# Add kernel flags globally
add_project_arguments(kernel_c_flags, language: 'c')

# Debug flags
if get_option('debug')
  add_project_arguments(['-DDEBUG', '-g'], language: 'c')
endif

# Include directories - make them available globally
inc_dirs = include_directories(
  'minix/kernel',
  'minix/kernel/include', 
  'minix/include',
  'minix'
)

# Build subdirectories
subdir('minix')
