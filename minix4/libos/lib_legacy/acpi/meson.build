# meson.build for ACPI library

opt_level = get_option('opt_level')

acpi_includes = include_directories(
  '.',
  '../klib/include',
  '../../kernel/include',
  '../../kernel/arch/i386',
  '../../kernel/uts/i86pc/sys'
)

acpi_sources = files(
  'acpi.c',
  'acpi_i386.c',
  'acpinex_drv.c',
  'acpinex_event.c'
)

acpi_c_args = [
  '-std=c99',
  '-D_MINIX_KERNEL',
  '-O' + opt_level
]

acpi_lib = static_library(
  'acpi',
  acpi_sources,
  include_directories: acpi_includes,
  c_args: acpi_c_args,
  install: false
)

acpi_dep = declare_dependency(
  link_with: acpi_lib,
  include_directories: acpi_includes
)
