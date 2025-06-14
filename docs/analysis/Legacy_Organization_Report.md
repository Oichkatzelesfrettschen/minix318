# Legacy Directory Organization Report

Generated: 2025-06-13 18:27:32

## Organization Summary

### LibOS Legacy (minix4/libos_legacy/)
- Organized into functional subdirectories: core, filesystem, ipc, memory, capabilities, device, posix, utilities, tests, demos, shell, networking, synchronization, capnp, math, runtime, commands

### Commands Legacy (userspace/commands_legacy/)
- Organized into categories: system, development, multimedia, networking, text, utilities, printing, hardware, communication, packaging, testing, build

### Drivers Legacy (minix4/exokernel/drivers_legacy/)
- Organized into categories: bus, storage, network, input, graphics, audio, firmware
- Moved ieee1275 to firmware category

### MINIX Directory (minix/)
- Organized into functional subdirectories: core, memory, process, ipc, signals, io, filesystem, device, synchronization, string, debug, utilities, text, legacy, misc, build, boot, headers, tests
- Moved major directories to appropriate locations:
  - commands â†’ userspace/minix_commands
  - drivers â†’ minix4/exokernel/minix_drivers
  - servers â†’ minix4/microkernel/servers
  - kernel â†’ minix4/exokernel/kernel_legacy
  - lib â†’ minix4/libos/lib_legacy
  - include â†’ minix4/include_legacy
  - usr.bin â†’ userspace/usr_bin_legacy
  - usr.sbin â†’ userspace/usr_sbin_legacy
  - docs â†’ docs/minix_legacy

## Next Steps

1. Review and refine the organization
2. Update build system to reflect new structure
3. Consolidate duplicate files within categories
4. Begin modernization of organized code
5. Update documentation and references

