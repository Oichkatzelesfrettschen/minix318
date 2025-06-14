# MINIX4 Synthesis Plan Report

Generated: 2025-06-13 19:42:45

## Executive Summary

This synthesis plan consolidates **2230 duplicate filename groups** into unified, modern MINIX4 implementations. The plan prioritizes critical system components and provides detailed strategies for each synthesis target.

### Synthesis Statistics

- **Total synthesis targets**: 2230
- **High priority targets**: 11
- **Low effort syntheseses**: 204
- **Medium effort syntheseses**: 22
- **High effort syntheseses**: 2004

### Synthesis Strategies

- **Identical merge**: 204 (files are identical, simple copy)
- **Function merge**: 22 (mostly common functions, moderate effort)
- **Careful synthesis**: 2004 (significant differences, careful analysis needed)

## High Priority Synthesis Targets

The following targets should be synthesized first:

### 1. main.c
- **Priority**: 1 (1=highest)
- **Target path**: `minix4/exokernel/main_unified.c`
- **Description**: Unified exokernel entry point
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 98
- **Total functions**: 22
- **Common functions**: 4
- **Unique functions**: 18
- **Sources**:
  - `userspace\minix_commands\devmand\main.c`
  - `userspace\commands_legacy\system\devmand\main.c`
  - `tar\test\main.c`
  - `minix4\microkernel\servers\pm\main.c`
  - `minix4\microkernel\servers\ramdiskfs\main.c`
  - `minix4\microkernel\servers\rs\main.c`
  - `minix4\microkernel\servers\netiso\xebec\main.c`
  - `minix4\libos_legacy\ed\main.c`
  - `minix4\libos_legacy\fsck_ext2fs\main.c`
  - `minix4\libos_legacy\ksh\main.c`
  - `minix4\libos_legacy\ls\main.c`
  - `minix4\libos_legacy\sh\main.c`
  - `minix4\libos\lib_legacy\libpuffs\main.c`
  - `minix4\libos\lib_legacy\libsffs\main.c`
  - `minix4\libos\lib_legacy\util\gmake-3.66\main.c`
  - `minix4\libos\lib_legacy\openssl\sunos\SUNVIDEO\main.c`
  - `minix4\libos\lib_legacy\openssl\sunos\SUNVIDEO\LASERDISC\lasertool\main.c`
  - `minix4\libos\lib_legacy\openssl\lib\posix\netiso\xebec\main.c`
  - `minix4\libos\lib_legacy\libsqlite\src\main.c`
  - `minix4\libos\lib_legacy\libarchive\test\main.c`
  - `minix4\libos\lib_legacy\i386\boot\bsd\main.c`
  - `minix4\exokernel\kernel_legacy\main.c`
  - `minix4\exokernel\minix_drivers\system\random\main.c`
  - `minix4\exokernel\minix_drivers\storage\filter\main.c`
  - `minix4\exokernel\minix_drivers\bus\pci\main.c`
  - `minix4\exokernel\kernel_legacy\include\main.c`
  - `minix4\exokernel\kernel_legacy\uts\common\os\main.c`
  - `minix4\exokernel\kernel_legacy\tools\cscope-fast\main.c`
  - `minix4\exokernel\kernel_legacy\libl\lib\main.c`
  - `minix4\exokernel\kernel_legacy\contrib\ast\src\cmd\ksh93\sh\main.c`
  - `minix4\exokernel\kernel_legacy\common\ficl\main.c`
  - `minix4\exokernel\kernel_legacy\common\mapfiles\gen\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\acctadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\auditreduce\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\awk\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\bart\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cdrw\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\dumpadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\filesync\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\fmtmsg\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\format\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\genmsg\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\lofiadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\logadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\mail\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\mailx\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\mandoc\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\oawk\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\praudit\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\prtdiag\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\prtfru\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\ramdiskadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sed\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sh\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sort\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\truss\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\installf\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgadd\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgchk\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgcond\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkginstall\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgmk\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgproto\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgremove\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgrm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\svr4pkg\pkgtrans\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\size\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\mcs\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\lex\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\elfwrap\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\elfdump\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\demo_rdb\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\ar\common\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sendmail\libmilter\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\sendmail\src\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\krb5\krb5kdc\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\isns\isnsd\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\hal\hald-runner\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\fs.d\ufs\fsck\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\fs.d\udfs\fsck\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.sbin\in.routed\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\bridged\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.ndpd\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.ripngd\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.bin\ftp\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.bin\pppd\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.bin\rdist\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.bin\telnet\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.bin\tftp\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\lib\nwamd\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-crypto\tpmadm\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\backup\restore\main.c`
  - `minix4\exokernel\kernel_legacy\cmd\abi\spectrans\parser\main.c`
  - `minix4\exokernel\kernel_legacy\boot\i386\loader\main.c`
  - `minix4\exokernel\kernel_legacy\boot\efi\loader\main.c`
  - `minix\core\main.c`
  - `archive\minix_legacy\main.c`
- **Common functions**: `put_major`, `cleanup`, `sig_int`, `get_major`

### 2. proc.c
- **Priority**: 2 (1=highest)
- **Target path**: `minix4/exokernel/process/proc_unified.c`
- **Description**: Unified process management
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 9
- **Total functions**: 9
- **Common functions**: 0
- **Unique functions**: 9
- **Sources**:
  - `userspace\usr_bin_legacy\trace\proc.c`
  - `minix4\libos_legacy\csh\proc.c`
  - `minix4\exokernel\kernel_legacy\proc.c`
  - `minix4\exokernel\kernel_legacy\uts\common\os\proc.c`
  - `minix4\exokernel\kernel_legacy\cmd\auditreduce\proc.c`
  - `minix4\exokernel\kernel_legacy\cmd\svc\startd\proc.c`
  - `minix\process\proc.c`
  - `archive\minix_legacy\proc.c`
  - `archive\legacy\pascal\proc.c`

### 3. alloc.c
- **Priority**: 3 (1=highest)
- **Target path**: `minix4/exokernel/memory/alloc_unified.c`
- **Description**: Unified memory allocation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 12
- **Total functions**: 2
- **Common functions**: 0
- **Unique functions**: 2
- **Sources**:
  - `minix4\microkernel\servers\pxk\alloc.c`
  - `minix4\microkernel\servers\lib\libc\port\threads\alloc.c`
  - `minix4\libos_legacy\csh\alloc.c`
  - `minix4\libos_legacy\ksh\alloc.c`
  - `minix4\libos\lib_legacy\pxk\alloc.c`
  - `minix4\libos\lib_legacy\openssl\sunos\pxk\alloc.c`
  - `minix4\exokernel\kernel_legacy\alloc.c`
  - `minix4\exokernel\kernel_legacy\tools\cscope-fast\alloc.c`
  - `minix4\exokernel\kernel_legacy\cmd\fm\modules\common\eversholt\alloc.c`
  - `minix4\exokernel\kernel_legacy\cmd\fm\eversholt\common\alloc.c`
  - `minix\memory\alloc.c`
  - `archive\minix_legacy\alloc.c`

### 4. memory.c
- **Priority**: 3 (1=highest)
- **Target path**: `minix4/exokernel/memory/memory_unified.c`
- **Description**: Unified memory management
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 7
- **Total functions**: 1
- **Common functions**: 0
- **Unique functions**: 1
- **Sources**:
  - `minix4\microkernel\core\memory\memory.c`
  - `minix4\libos\lib_legacy\libprtdiag\common\memory.c`
  - `minix4\exokernel\minix_drivers\storage\memory\memory.c`
  - `minix4\exokernel\kernel_legacy\tools\ctf\common\memory.c`
  - `minix4\exokernel\kernel_legacy\cmd\mdb\common\modules\genunix\memory.c`
  - `minix4\exokernel\kernel_legacy\arch\earm\memory.c`
  - `minix4\exokernel\kernel_legacy\arch\i386\memory.c`

### 5. signal.c
- **Priority**: 4 (1=highest)
- **Target path**: `minix4/exokernel/signals/signal_unified.c`
- **Description**: Unified signal handling
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 11
- **Total functions**: 0
- **Common functions**: 0
- **Unique functions**: 0
- **Sources**:
  - `minix4\microkernel\servers\lib\libc\port\sys\signal.c`
  - `minix4\libos\lib_legacy\libucb\sparc\sys\signal.c`
  - `minix4\libos\lib_legacy\libucb\i386\sys\signal.c`
  - `minix4\libos\lib_legacy\brand\lx\lx_brand\common\signal.c`
  - `minix4\exokernel\kernel_legacy\signal.c`
  - `minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\features\signal.c`
  - `minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\misc\signal.c`
  - `minix4\exokernel\kernel_legacy\cmd\sendmail\libmilter\signal.c`
  - `minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\signal.c`
  - `minix4\exokernel\kernel_legacy\cmd\csh\i386\signal.c`
  - `minix4\exokernel\kernel_legacy\cmd\csh\sparc\signal.c`

### 6. sig.c
- **Priority**: 4 (1=highest)
- **Target path**: `minix4/exokernel/signals/sig_unified.c`
- **Description**: Unified signal operations
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 3
- **Total functions**: 0
- **Common functions**: 0
- **Unique functions**: 0
- **Sources**:
  - `minix4\exokernel\kernel_legacy\sig.c`
  - `minix4\exokernel\kernel_legacy\uts\common\os\sig.c`
  - `minix\signals\sig.c`

### 7. ipc.c
- **Priority**: 5 (1=highest)
- **Target path**: `minix4/exokernel/ipc/ipc_unified.c`
- **Description**: Unified IPC implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 8
- **Total functions**: 6
- **Common functions**: 0
- **Unique functions**: 6
- **Sources**:
  - `userspace\usr_bin_legacy\trace\service\ipc.c`
  - `minix4\libos_legacy\ipc\ipc.c`
  - `minix4\libos\lib_legacy\libbdev\ipc.c`
  - `minix4\libos\lib_legacy\openssl\lib\libposix\ipc.c`
  - `minix4\exokernel\kernel_legacy\uts\common\os\ipc.c`
  - `minix4\exokernel\kernel_legacy\cmd\truss\ipc.c`
  - `minix4\exokernel\kernel_legacy\cmd\mdb\common\modules\ipc\ipc.c`
  - `minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.cdpd\ipc.c`

### 8. printf.c
- **Priority**: 6 (1=highest)
- **Target path**: `minix4/libos/libc/printf_unified.c`
- **Description**: Unified printf implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 13
- **Total functions**: 2
- **Common functions**: 0
- **Unique functions**: 2
- **Sources**:
  - `minix4\libos_legacy\utilities\printf.c`
  - `minix4\libos\lib_legacy\libucb\port\stdio\printf.c`
  - `minix4\libos\lib_legacy\libsqlite\src\printf.c`
  - `minix4\libos\lib_legacy\libfakekernel\common\printf.c`
  - `minix4\libos\lib_legacy\libddekit\src\printf.c`
  - `minix4\exokernel\kernel_legacy\printf.c`
  - `minix4\exokernel\kernel_legacy\uts\common\os\printf.c`
  - `minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\stdio\printf.c`
  - `minix4\exokernel\kernel_legacy\cmd\csh\printf.c`
  - `minix4\exokernel\kernel_legacy\cmd\printf\printf.c`
  - `minix4\exokernel\kernel_legacy\cmd\vi\port\printf.c`
  - `minix4\exokernel\kernel_legacy\boot\libsa\printf.c`
  - `minix\utilities\printf.c`

### 9. string.c
- **Priority**: 7 (1=highest)
- **Target path**: `minix4/libos/libc/string_unified.c`
- **Description**: Unified string operations
- **Strategy**: function_merge
- **Estimated effort**: Medium
- **Source files**: 8
- **Total functions**: 7
- **Common functions**: 7
- **Unique functions**: 0
- **Sources**:
  - `userspace\minix_commands\cawf\string.c`
  - `userspace\commands_legacy\text\cawf\string.c`
  - `minix4\libos\posix\libc\string\string.c`
  - `minix4\exokernel\kernel_legacy\string.c`
  - `minix4\exokernel\kernel_legacy\contrib\ast\src\cmd\ksh93\sh\string.c`
  - `minix4\exokernel\kernel_legacy\common\util\string.c`
  - `minix4\exokernel\kernel_legacy\cmd\sh\string.c`
  - `minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\string.c`
- **Common functions**: `Endword`, `Setroman`, `Findchar`, `Findhy`, `Str2word`, `Delstr`, `Asmname`

### 10. malloc.c
- **Priority**: 8 (1=highest)
- **Target path**: `minix4/libos/libc/malloc_unified.c`
- **Description**: Unified malloc implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 13
- **Total functions**: 5
- **Common functions**: 0
- **Unique functions**: 5
- **Sources**:
  - `minix4\microkernel\servers\pxk\malloc.c`
  - `minix4\microkernel\servers\netiso\xebec\malloc.c`
  - `minix4\microkernel\servers\lib\libc\port\gen\malloc.c`
  - `minix4\libos\posix\libc\stdlib\malloc.c`
  - `minix4\libos\lib_legacy\watchmalloc\common\malloc.c`
  - `minix4\libos\lib_legacy\openssl\lib\posix\netiso\xebec\malloc.c`
  - `minix4\libos\lib_legacy\libumem\common\malloc.c`
  - `minix4\libos\lib_legacy\libmalloc\common\malloc.c`
  - `minix4\libos\lib_legacy\i386\boot\malloc.c`
  - `minix4\exokernel\kernel_legacy\malloc.c`
  - `minix4\exokernel\kernel_legacy\stand\lib\sa\malloc.c`
  - `minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\vmalloc\malloc.c`
  - `minix4\exokernel\kernel_legacy\cmd\sgs\rtld\common\malloc.c`

### 11. syscall.c
- **Priority**: 9 (1=highest)
- **Target path**: `minix4/exokernel/syscalls/syscall_unified.c`
- **Description**: Unified system calls
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 4
- **Total functions**: 5
- **Common functions**: 0
- **Unique functions**: 5
- **Sources**:
  - `minix4\libos\lib_legacy\libc\sys\syscall.c`
  - `minix4\exokernel\kernel_legacy\syscall.c`
  - `minix4\exokernel\kernel_legacy\uts\sparc\os\syscall.c`
  - `minix4\exokernel\kernel_legacy\uts\intel\os\syscall.c`

### 12. 11printf.c
- **Priority**: 100 (1=highest)
- **Target path**: `minix4/libos/utilities/11printf_unified.c`
- **Description**: Unified 11printf implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 2
- **Total functions**: 0
- **Common functions**: 0
- **Unique functions**: 0
- **Sources**:
  - `minix\11printf.c`
  - `minix4\exokernel\kernel_legacy\11printf.c`

### 13. arbitrate.c
- **Priority**: 100 (1=highest)
- **Target path**: `minix4/libos/utilities/arbitrate_unified.c`
- **Description**: Unified arbitrate implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 2
- **Total functions**: 8
- **Common functions**: 2
- **Unique functions**: 6
- **Sources**:
  - `minix\arbitrate.c`
  - `minix4\exokernel\kernel_legacy\arbitrate.c`
- **Common functions**: `arbitrate_request`, `arbitrate_init`

### 14. call.c
- **Priority**: 100 (1=highest)
- **Target path**: `minix4/libos/utilities/call_unified.c`
- **Description**: Unified call implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 7
- **Total functions**: 10
- **Common functions**: 0
- **Unique functions**: 10
- **Sources**:
  - `minix\call.c`
  - `userspace\usr_bin_legacy\trace\call.c`
  - `minix4\libos\lib_legacy\libbdev\call.c`
  - `minix4\libos\lib_legacy\libfsdriver\call.c`
  - `minix4\exokernel\kernel_legacy\call.c`
  - `archive\minix_legacy\call.c`
  - `archive\legacy\pascal\call.c`

### 15. cset.c
- **Priority**: 100 (1=highest)
- **Target path**: `minix4/libos/utilities/cset_unified.c`
- **Description**: Unified cset implementation
- **Strategy**: careful_synthesis
- **Estimated effort**: High
- **Source files**: 5
- **Total functions**: 0
- **Common functions**: 0
- **Unique functions**: 0
- **Sources**:
  - `minix\cset.c`
  - `minix4\exokernel\kernel_legacy\cset.c`
  - `minix4\exokernel\kernel_legacy\cmd\tr\cset.c`
  - `archive\minix_legacy\cset.c`
  - `archive\legacy\pascal\cset.c`

## Implementation Steps

1. **Run synthesis script**: Execute the generated PowerShell script
2. **Review unified files**: Check synthesized implementations for correctness
3. **Update build system**: Modify Makefile/meson.build to use unified files
4. **Test compilation**: Ensure unified files compile without errors
5. **Run tests**: Execute test suite to verify functionality
6. **Commit changes**: Git commit the synthesized unified files

## Next Phase

After synthesis completion:
- Modernize synthesized code to C23 standards
- Add comprehensive error handling
- Implement capability-based security
- Create unit tests for all unified functions
- Optimize performance and reduce complexity
