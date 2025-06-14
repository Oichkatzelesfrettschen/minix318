# MINIX4 Legacy Directory Organization Script
# This script organizes the "plopped" legacy directories with proper internal structure

param(
    [string]$WorkspaceRoot = "c:\Users\ericj\minix318",
    [switch]$DryRun = $false,
    [switch]$Verbose = $false
)

$ErrorActionPreference = "Stop"

function Write-Log {
    param($Message, $Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $(
        switch ($Level) {
            "ERROR" { "Red" }
            "WARN" { "Yellow" }
            "SUCCESS" { "Green" }
            default { "White" }
        }
    )
}

function Create-DirectoryIfNotExists {
    param($Path)
    if (-not (Test-Path $Path)) {
        if (-not $DryRun) {
            New-Item -ItemType Directory -Path $Path -Force | Out-Null
        }
        Write-Log "Created directory: $Path" "SUCCESS"
    }
}

function Move-FileWithLogging {
    param($Source, $Destination)
    $destDir = Split-Path $Destination -Parent
    Create-DirectoryIfNotExists $destDir
    
    if (-not $DryRun) {
        Move-Item $Source $Destination -Force
    }
    Write-Log "Moved: $Source -> $Destination"
}

function Organize-LibOSLegacy {
    param($BasePath)
    
    Write-Log "Organizing LibOS Legacy directory..." "INFO"
    
    $libosPath = Join-Path $BasePath "minix4\libos_legacy"
    
    # Create organized subdirectories
    $subdirs = @{
        "core" = @("init.c", "main.c", "proc*.c", "sched.c", "sched.h", "sched_helpers.c")
        "filesystem" = @("fs.c", "fs.h", "fs_ufs.c", "file.c", "file.h", "fileserver.c", "libfs.h", "mkfs.c")
        "ipc" = @("ipc.c", "ipc_queue.c", "ipc_queue.h", "ipc_test.c", "chan.c", "door.c", "msg_router.c")
        "memory" = @("umalloc.c", "resource_account.c")
        "capabilities" = @("cap.c", "caplib.c", "caplib_example.c", "capwrap.c", "capwrap.h", "lambda_cap.c")
        "device" = @("driver.h", "irq_client.c", "irq_client.h", "kbdserv.c", "pingdriver.c", "iommu.c")
        "posix" = @("posix.c", "posix.h", "*posix*test*.c", "termios.c", "termios.h", "termios_demo.c")
        "utilities" = @("ulib.c", "printf.c", "echo.c", "grep.c", "cat.c", "ls.c", "wc.c", "kill.c", "ln.c", "mkdir.c", "rm.c")
        "tests" = @("*test*.c", "usertests.c", "forktest.c", "nbtest.c", "stressfs.c", "exo_unit_test.c")
        "demos" = @("*demo*.c", "blink.c", "zombie.c", "phi.c")
        "shell" = @("sh.c", "rcrs.c")
        "networking" = @("rcmd/", "rcp/")
        "synchronization" = @("sleeplock.h", "spinlock.h")
        "capnp" = @("capnp_helpers.c", "capnp_helpers.h", "capnp/")
        "math" = @("math_core.c")
        "runtime" = @("affine_runtime.c", "affine_runtime.h", "registration.c", "start_guest.c")
        "commands" = @("cat/", "chmod/", "chown/", "cp/", "csh/", "date/", "dd/", "df/", "domainname/", "echo/", "ed/", "expr/", "fsck/", "fsck_ext2fs/", "getfacl/", "hostname/", "ifconfig/", "init/", "kill/", "ksh/", "ln/", "ls/", "mkdir/", "mv/", "pax/", "ps/", "pwd/", "rm/", "rmdir/", "sh/", "sleep/", "stty/", "sync/", "test/", "tests/", "stubs/")
    }
    
    foreach ($subdir in $subdirs.Keys) {
        $subdirPath = Join-Path $libosPath $subdir
        Create-DirectoryIfNotExists $subdirPath
        
        foreach ($pattern in $subdirs[$subdir]) {
            $files = Get-ChildItem -Path $libosPath -Name $pattern -ErrorAction SilentlyContinue
            foreach ($file in $files) {
                $sourcePath = Join-Path $libosPath $file
                $destPath = Join-Path $subdirPath $file
                if (Test-Path $sourcePath -PathType Leaf) {
                    Move-FileWithLogging $sourcePath $destPath
                }
            }
        }
    }
}

function Organize-CommandsLegacy {
    param($BasePath)
    
    Write-Log "Organizing Commands Legacy directory..." "INFO"
    
    $commandsPath = Join-Path $BasePath "userspace\commands_legacy"
    
    # Create functional subdirectories
    $categories = @{
        "system" = @("at/", "atnormalize/", "autopart/", "backup/", "cleantmp/", "cron/", "crontab/", "devmand/", "devsize/", "fdisk/", "format/", "fsck.mfs/", "MAKEDEV/", "minix-service/", "mount/", "umount/", "part/", "partition/", "repartition/", "ramdisk/", "readclock/", "setup/", "svrctl/", "sysenv/", "update/", "updateboot/", "update_asr/", "update_bootcfg/", "version/")
        "development" = @("DESCRIBE/", "ifdef/", "gcov-pull/", "sprofalyze/", "sprofdiff/", "srccrc/", "swifi/", "synctree/")
        "multimedia" = @("playwave/", "recwave/", "screendump/", "vol/")
        "networking" = @("fetch/", "netconf/", "remsync/", "slip/", "zmodem/")
        "text" = @("cawf/", "look/", "spell/", "term/", "termcap/", "tget/")
        "utilities" = @("compress/", "crc/", "dosread/", "eject/", "fix/", "isoread/", "loadfont/", "loadkeys/", "loadramdisk/", "mt/", "prep/", "printroot/", "progressbar/", "rawspeed/", "rotate/", "truncate/", "worldstone/", "writeisofs/")
        "printing" = @("lp/", "lpd/")
        "hardware" = @("cdprobe/", "dhrystone/", "intr/", "lspci/")
        "communication" = @("mail/")
        "packaging" = @("pkgin_all/", "pkgin_cd/", "pkgin_sets/")
        "testing" = @("profile/")
    }
    
    foreach ($category in $categories.Keys) {
        $categoryPath = Join-Path $commandsPath $category
        Create-DirectoryIfNotExists $categoryPath
        
        foreach ($command in $categories[$category]) {
            $sourcePath = Join-Path $commandsPath $command
            $destPath = Join-Path $categoryPath $command
            if (Test-Path $sourcePath) {
                Move-FileWithLogging $sourcePath $destPath
            }
        }
    }
    
    # Move Makefiles to build directory
    $buildPath = Join-Path $commandsPath "build"
    Create-DirectoryIfNotExists $buildPath
    
    $makefiles = @("Makefile", "Makefile.inc")
    foreach ($makefile in $makefiles) {
        $sourcePath = Join-Path $commandsPath $makefile
        if (Test-Path $sourcePath) {
            $destPath = Join-Path $buildPath $makefile
            Move-FileWithLogging $sourcePath $destPath
        }
    }
}

function Organize-DriversLegacy {
    param($BasePath)
    
    Write-Log "Organizing Drivers Legacy directory..." "INFO"
    
    $driversPath = Join-Path $BasePath "minix4\exokernel\drivers_legacy"
    
    # The ieee1275 directory is already there, just need to organize any additional drivers
    # Create category subdirectories for future organization
    $categories = @("bus", "storage", "network", "input", "graphics", "audio", "firmware")
    
    foreach ($category in $categories) {
        $categoryPath = Join-Path $driversPath $category
        Create-DirectoryIfNotExists $categoryPath
    }
    
    # Move ieee1275 to firmware category
    $ieee1275Source = Join-Path $driversPath "ieee1275"
    $ieee1275Dest = Join-Path $driversPath "firmware\ieee1275"
    if (Test-Path $ieee1275Source) {
        Move-FileWithLogging $ieee1275Source $ieee1275Dest
    }
}

function Organize-MinixDirectory {
    param($BasePath)
    
    Write-Log "Organizing main MINIX directory..." "INFO"
    
    $minixPath = Join-Path $BasePath "minix"
    
    # Create organized subdirectories in minix/
    $subdirs = @{
        "core" = @("main.c", "init*.c", "panic.h", "config*.h", "param.h", "defs.h", "types*.h", "const.c")
        "memory" = @("alloc.c", "mmu_unified.h", "pgtab.h")
        "process" = @("proc.c", "exit.c", "wait.s")
        "ipc" = @("ipc*.h", "msg*.c", "msg*.h", "pipe.c", "chan.h")
        "signals" = @("sig*.c", "signal.s")
        "io" = @("iget.c", "rdwri.c", "fio.c")
        "filesystem" = @("stat.c", "stat.h", "buf.h")
        "device" = @("iommu*.c", "iommu*.h", "pci.h", "usb.h", "interrupt.h", "ddekit.h")
        "synchronization" = @("lock.c", "lock.h", "sleeplock.h", "spinlock.h", "condvar.h", "semaphore.h")
        "string" = @("kstring.c", "hash.c", "conv.c")
        "debug" = @("debug.h", "assert.h", "error.c", "perror.c")
        "utilities" = @("printf*.c", "printf*.h", "printf*.s", "prf.c", "qsort.c", "printenv.c")
        "text" = @("text.c", "case.c", "clas.c", "nstr_graph.c", "nstr_graph.h")
        "legacy" = @("*.h", "*.s", "opcode*", "OPnames.h", "TRdata.c", "tree.h", "treen.s", "yy.h", "yycopy.s", "Version.c")
        "misc" = @("ato.c", "fdec.c", "from.c", "func.c", "getNAME.c", "lab.c", "last.c", "man.c", "mkstr.c", "nami.c", "nl.c", "reset.c", "slp.c", "stub.c", "subr.c", "system.c", "tset.c", "tset.del.h", "tty.c")
        "build" = @("Makefile*", "meson.build")
        "boot" = @("bootasm.S", "test_bootasm.S")
        "headers" = @("attribs.h", "inline.h", "klib.h", "resources.h", "thread.h", "timer.h", "user.h")
        "tests" = @("test_*.c", "test_*.h", "tests/")
    }
    
    foreach ($subdir in $subdirs.Keys) {
        $subdirPath = Join-Path $minixPath $subdir
        Create-DirectoryIfNotExists $subdirPath
        
        foreach ($pattern in $subdirs[$subdir]) {
            $files = Get-ChildItem -Path $minixPath -Name $pattern -ErrorAction SilentlyContinue
            foreach ($file in $files) {
                $sourcePath = Join-Path $minixPath $file
                $destPath = Join-Path $subdirPath $file
                if (Test-Path $sourcePath -PathType Leaf) {
                    Move-FileWithLogging $sourcePath $destPath
                }
            }
        }
    }
    
    # Handle special directories that should be moved whole
    $specialDirs = @("commands", "drivers", "servers", "kernel", "lib", "include", "usr.bin", "usr.sbin", "docs", "limine_src", "microwindows_src", "llvm")
    foreach ($dir in $specialDirs) {
        $sourcePath = Join-Path $minixPath $dir
        if (Test-Path $sourcePath) {
            # Determine appropriate destination
            $destPath = switch ($dir) {
                "commands" { Join-Path $BasePath "userspace\minix_commands" }
                "drivers" { Join-Path $BasePath "minix4\exokernel\minix_drivers" }
                "servers" { Join-Path $BasePath "minix4\microkernel\servers" }
                "kernel" { Join-Path $BasePath "minix4\exokernel\kernel_legacy" }
                "lib" { Join-Path $BasePath "minix4\libos\lib_legacy" }
                "include" { Join-Path $BasePath "minix4\include_legacy" }
                "usr.bin" { Join-Path $BasePath "userspace\usr_bin_legacy" }
                "usr.sbin" { Join-Path $BasePath "userspace\usr_sbin_legacy" }
                "docs" { Join-Path $BasePath "docs\minix_legacy" }
                default { Join-Path $BasePath "archive\minix_legacy\$dir" }
            }
            Move-FileWithLogging $sourcePath $destPath
        }
    }
}

# Main execution
Write-Log "Starting MINIX4 Legacy Directory Organization" "INFO"
Write-Log "Workspace: $WorkspaceRoot" "INFO"
Write-Log "Dry Run: $DryRun" "INFO"

try {
    # Organize each legacy directory
    Organize-LibOSLegacy $WorkspaceRoot
    Organize-CommandsLegacy $WorkspaceRoot
    Organize-DriversLegacy $WorkspaceRoot
    Organize-MinixDirectory $WorkspaceRoot
    
    Write-Log "Legacy directory organization completed successfully!" "SUCCESS"
    
    # Create summary report
    $reportPath = Join-Path $WorkspaceRoot "docs\analysis\Legacy_Organization_Report.md"
    $report = @"
# Legacy Directory Organization Report

Generated: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")

## Organization Summary

### LibOS Legacy (`minix4/libos_legacy/`)
- Organized into functional subdirectories: core, filesystem, ipc, memory, capabilities, device, posix, utilities, tests, demos, shell, networking, synchronization, capnp, math, runtime, commands

### Commands Legacy (`userspace/commands_legacy/`)
- Organized into categories: system, development, multimedia, networking, text, utilities, printing, hardware, communication, packaging, testing, build

### Drivers Legacy (`minix4/exokernel/drivers_legacy/`)
- Organized into categories: bus, storage, network, input, graphics, audio, firmware
- Moved ieee1275 to firmware category

### MINIX Directory (`minix/`)
- Organized into functional subdirectories: core, memory, process, ipc, signals, io, filesystem, device, synchronization, string, debug, utilities, text, legacy, misc, build, boot, headers, tests
- Moved major directories to appropriate locations:
  - commands → userspace/minix_commands
  - drivers → minix4/exokernel/minix_drivers
  - servers → minix4/microkernel/servers
  - kernel → minix4/exokernel/kernel_legacy
  - lib → minix4/libos/lib_legacy
  - include → minix4/include_legacy
  - usr.bin → userspace/usr_bin_legacy
  - usr.sbin → userspace/usr_sbin_legacy
  - docs → docs/minix_legacy

## Next Steps

1. Review and refine the organization
2. Update build system to reflect new structure
3. Consolidate duplicate files within categories
4. Begin modernization of organized code
5. Update documentation and references

"@
    
    if (-not $DryRun) {
        $report | Out-File -FilePath $reportPath -Encoding UTF8
        Write-Log "Created report: $reportPath" "SUCCESS"
    }
    
} catch {
    Write-Log "Error during organization: $($_.Exception.Message)" "ERROR"
    throw
}
