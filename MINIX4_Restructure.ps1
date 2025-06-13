#!/usr/bin/env pwsh
<#
.SYNOPSIS
    MINIX4 Complete File Structure Reorganization Script
.DESCRIPTION
    Comprehensive PowerShell script to restructure the chaotic 44,000+ file
    MINIX codebase into a clean, modular, hybrid exokernel + microkernel + libOS design.
    
    This script implements the complete restructuring plan outlined in
    MINIX4_RESTRUCTURING_PLAN.md
.PARAMETER DryRun
    Run in test mode without actually moving files
.PARAMETER Phase
    Execute specific phase of restructuring (1-3)
.PARAMETER Backup
    Create backup before restructuring
.EXAMPLE
    .\MINIX4_Restructure.ps1 -DryRun -Phase 1
    .\MINIX4_Restructure.ps1 -Backup -Phase 1
.NOTES
    Author: MINIX Development Team
    Version: 4.0.0
    Date: 2025-06-13
#>

[CmdletBinding()]
param(
    [switch]$DryRun,
    [ValidateRange(1,3)][int]$Phase = 1,
    [switch]$Backup,
    [switch]$Force
)

# Script configuration
$ErrorActionPreference = "Stop"
$ProgressPreference = "Continue"

# Color definitions for output
$Colors = @{
    Success = "Green"
    Warning = "Yellow"
    Error = "Red"
    Info = "Cyan"
    Header = "Magenta"
}

function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White",
        [switch]$NoNewline
    )
    
    $params = @{
        Object = $Message
        ForegroundColor = $Color
    }
    if ($NoNewline) { $params.NoNewline = $true }
    
    Write-Host @params
}

function Write-Header {
    param([string]$Text)
    Write-ColorOutput "`n" + ("="*80) -Color $Colors.Header
    Write-ColorOutput " $Text" -Color $Colors.Header
    Write-ColorOutput ("="*80) -Color $Colors.Header
}

function Write-Phase {
    param([string]$Text)
    Write-ColorOutput "`n" + ("-"*60) -Color $Colors.Info
    Write-ColorOutput " PHASE: $Text" -Color $Colors.Info
    Write-ColorOutput ("-"*60) -Color $Colors.Info
}

function Test-PathSafely {
    param([string]$Path)
    try {
        return Test-Path $Path -ErrorAction Stop
    }
    catch {
        return $false
    }
}

function New-DirectoryStructure {
    param([string]$BasePath)
    
    Write-ColorOutput "Creating MINIX4 directory structure..." -Color $Colors.Info
    
    # Define the complete new directory structure
    $NewStructure = @(
        # Core system directories
        "minix4",
        "minix4/exokernel",
        "minix4/exokernel/arch",
        "minix4/exokernel/arch/x86_64",
        "minix4/exokernel/arch/x86_64/boot",
        "minix4/exokernel/arch/x86_64/mm",
        "minix4/exokernel/arch/x86_64/interrupts", 
        "minix4/exokernel/arch/x86_64/io",
        "minix4/exokernel/arch/x86_64/include",
        "minix4/exokernel/arch/i386",
        "minix4/exokernel/arch/i386/boot",
        "minix4/exokernel/arch/i386/mm",
        "minix4/exokernel/arch/i386/interrupts",
        "minix4/exokernel/arch/i386/io", 
        "minix4/exokernel/arch/i386/include",
        "minix4/exokernel/arch/arm64",
        "minix4/exokernel/arch/riscv64",
        "minix4/exokernel/include",
        "minix4/exokernel/core",
        "minix4/exokernel/core/resource",
        "minix4/exokernel/core/resource/memory",
        "minix4/exokernel/core/resource/cpu",
        "minix4/exokernel/core/resource/io",
        "minix4/exokernel/core/protection",
        "minix4/exokernel/core/scheduling",
        "minix4/exokernel/core/ipc",
        "minix4/exokernel/drivers",
        "minix4/exokernel/drivers/storage",
        "minix4/exokernel/drivers/network",
        "minix4/exokernel/drivers/graphics",
        "minix4/exokernel/drivers/input",
        
        # Microkernel directories
        "minix4/microkernel",
        "minix4/microkernel/core",
        "minix4/microkernel/core/init",
        "minix4/microkernel/core/ipc",
        "minix4/microkernel/core/messaging",
        "minix4/microkernel/core/process",
        "minix4/microkernel/core/scheduling",
        "minix4/microkernel/core/security",
        "minix4/microkernel/core/namespace",
        "minix4/microkernel/core/timing",
        "minix4/microkernel/services",
        "minix4/microkernel/services/pm",
        "minix4/microkernel/services/vfs", 
        "minix4/microkernel/services/vm",
        "minix4/microkernel/services/net",
        "minix4/microkernel/services/device",
        "minix4/microkernel/services/security",
        "minix4/microkernel/include",
        "minix4/microkernel/protocols",
        
        # LibOS directories
        "minix4/libos",
        "minix4/libos/posix",
        "minix4/libos/posix/libc",
        "minix4/libos/posix/libc/stdlib",
        "minix4/libos/posix/libc/stdio",
        "minix4/libos/posix/libc/string",
        "minix4/libos/posix/libc/math",
        "minix4/libos/posix/libc/time",
        "minix4/libos/posix/libposix",
        "minix4/libos/posix/syscalls",
        "minix4/libos/posix/runtime",
        "minix4/libos/rump",
        "minix4/libos/rump/fs",
        "minix4/libos/rump/net",
        "minix4/libos/rump/dev",
        "minix4/libos/rump/crypto",
        "minix4/libos/specialized",
        "minix4/libos/specialized/minimal",
        "minix4/libos/specialized/realtime",
        "minix4/libos/specialized/secure", 
        "minix4/libos/specialized/hpc",
        "minix4/libos/include",
        
        # Compatibility layers
        "minix4/compatibility",
        "minix4/compatibility/minix3",
        "minix4/compatibility/unix",
        "minix4/compatibility/linux",
        "minix4/compatibility/posix",
        
        # Global includes
        "minix4/include",
        "minix4/include/minix4",
        "minix4/include/sys",
        "minix4/include/arch",
        "minix4/include/compat",
        
        # User-space components
        "userspace",
        "userspace/commands",
        "userspace/commands/core",
        "userspace/commands/admin",
        "userspace/commands/network",
        "userspace/commands/development",
        "userspace/libraries",
        "userspace/libraries/standard",
        "userspace/libraries/system",
        "userspace/libraries/third_party",
        "userspace/servers",
        "userspace/servers/file",
        "userspace/servers/network",
        "userspace/servers/database",
        "userspace/applications",
        "userspace/applications/editors",
        "userspace/applications/shells",
        "userspace/applications/demos",
        
        # Build system
        "build",
        "build/cmake",
        "build/meson",
        "build/makefiles",
        "build/config",
        "build/cross",
        
        # Archive and legacy
        "archive",
        "archive/minix_legacy",
        "archive/minix_legacy/root_files",
        "archive/minix_legacy/kernel_files", 
        "archive/unix_v6v7",
        "archive/unix_v6v7/yacc",
        "archive/unix_v6v7/pascal",
        "archive/deprecated",
        "archive/experimental",
        
        # Development
        "contrib",
        "packaging",
        "packaging/debian",
        "packaging/rpm",
        "packaging/ports",
        "release",
        "release/notes",
        "release/scripts",
        "release/artifacts"
    )
    
    $Total = $NewStructure.Count
    $Current = 0
    
    foreach ($Dir in $NewStructure) {
        $Current++
        $FullPath = Join-Path $BasePath $Dir
        
        Write-Progress -Activity "Creating Directory Structure" `
                      -Status "Creating: $Dir" `
                      -PercentComplete (($Current / $Total) * 100)
        
        if (-not $DryRun) {
            try {
                New-Item -ItemType Directory -Path $FullPath -Force -ErrorAction Stop | Out-Null
                Write-ColorOutput "  ✓ Created: $Dir" -Color $Colors.Success
            }
            catch {
                Write-ColorOutput "  ✗ Failed to create: $Dir - $($_.Exception.Message)" -Color $Colors.Error
                throw
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would create: $Dir" -Color $Colors.Warning
        }
    }
    
    Write-Progress -Activity "Creating Directory Structure" -Completed
    Write-ColorOutput "Directory structure creation completed!" -Color $Colors.Success
}

function Move-RootLevelFiles {
    param([string]$BasePath)
    
    Write-ColorOutput "Moving root-level chaos files..." -Color $Colors.Info
    
    # Define file movement mappings
    $FileMappings = @{
        # Core kernel files
        "clock.c" = "minix4/microkernel/core/timing/clock.c"
        "proc.c" = "minix4/microkernel/core/process/proc.c"
        "main.c" = "minix4/microkernel/core/init/main_root.c"
        "malloc.c" = "minix4/libos/posix/libc/stdlib/malloc.c"
        "string.c" = "minix4/libos/posix/libc/string/string.c"
        "printf.c" = "minix4/libos/posix/libc/stdio/printf.c"
        "error.c" = "minix4/libos/posix/libc/stdlib/error.c"
        
        # System utilities
        "stat.c" = "userspace/commands/core/stat.c"
        "ls.c" = "userspace/commands/core/ls.c"  
        "wumpus.c" = "userspace/applications/demos/wumpus.c"
        
        # Legacy/experimental files
        "y.tab.c" = "archive/unix_v6v7/yacc/y.tab.c"
        "y.tab.h" = "archive/unix_v6v7/yacc/y.tab.h"
        "yy.h" = "archive/unix_v6v7/yacc/yy.h"
        "yyparse.c" = "archive/unix_v6v7/yacc/yyparse.c"
        "yylex.c" = "archive/unix_v6v7/yacc/yylex.c"
        "yymain.c" = "archive/unix_v6v7/yacc/yymain.c"
        "yyerror.c" = "archive/unix_v6v7/yacc/yyerror.c"
        "yyget.c" = "archive/unix_v6v7/yacc/yyget.c"
        "yyput.c" = "archive/unix_v6v7/yacc/yyput.c"
        "yytree.c" = "archive/unix_v6v7/yacc/yytree.c"
        "yyseman.c" = "archive/unix_v6v7/yacc/yyseman.c"
        "yycosts.c" = "archive/unix_v6v7/yacc/yycosts.c"
        "yyoptions.c" = "archive/unix_v6v7/yacc/yyoptions.c"
        "yypanic.c" = "archive/unix_v6v7/yacc/yypanic.c"
        "yyrecover.c" = "archive/unix_v6v7/yacc/yyrecover.c"
        "yyid.c" = "archive/unix_v6v7/yacc/yyid.c"
        "yycopy.s" = "archive/unix_v6v7/yacc/yycopy.s"
        
        # Assembly files
        "fp1.s" = "minix4/exokernel/arch/i386/fp1.s"
        "fp2.s" = "minix4/exokernel/arch/i386/fp2.s"
        "fp3.s" = "minix4/exokernel/arch/i386/fp3.s"
        "fpx.s" = "minix4/exokernel/arch/i386/fpx.s"
        "printf.s" = "minix4/exokernel/arch/i386/printf.s"
        "putc.s" = "minix4/exokernel/arch/i386/putc.s"
        "putn.s" = "minix4/exokernel/arch/i386/putn.s"
        "signal.s" = "minix4/exokernel/arch/i386/signal.s"
        "treen.s" = "minix4/exokernel/arch/i386/treen.s"
        "wait.s" = "minix4/exokernel/arch/i386/wait.s"
        
        # Headers
        "0.h" = "archive/unix_v6v7/0.h"
        "opcode.h" = "archive/unix_v6v7/opcode.h"
        "OPnames.h" = "archive/unix_v6v7/OPnames.h"
        "tree.h" = "minix4/libos/posix/libc/tree.h"
        "send.h" = "minix4/microkernel/core/ipc/send.h"
        
        # Demo and test files
        "*_demo.c" = "userspace/applications/demos/"
        "*_test.c" = "tests/unit/"
    }
    
    $ProcessedFiles = 0
    $TotalFiles = $FileMappings.Count
    
    foreach ($SourcePattern in $FileMappings.Keys) {
        $DestPath = $FileMappings[$SourcePattern]
        $ProcessedFiles++
        
        Write-Progress -Activity "Moving Root Files" `
                      -Status "Processing: $SourcePattern" `
                      -PercentComplete (($ProcessedFiles / $TotalFiles) * 100)
        
        # Handle wildcard patterns
        if ($SourcePattern.Contains("*")) {
            $Files = Get-ChildItem -Path $BasePath -Filter $SourcePattern -File -ErrorAction SilentlyContinue
            foreach ($File in $Files) {
                $DestFile = Join-Path $BasePath $DestPath $File.Name
                Move-FileWithLogging -Source $File.FullName -Destination $DestFile
            }
        }
        else {
            $SourceFile = Join-Path $BasePath $SourcePattern
            if (Test-PathSafely $SourceFile) {
                $DestFile = Join-Path $BasePath $DestPath
                Move-FileWithLogging -Source $SourceFile -Destination $DestFile
            }
        }
    }
    
    Write-Progress -Activity "Moving Root Files" -Completed
}

function Move-MinixKernelFiles {
    param([string]$BasePath)
    
    Write-ColorOutput "Reorganizing minix/kernel/ chaos..." -Color $Colors.Info
    
    $KernelPath = Join-Path $BasePath "minix/kernel"
    if (-not (Test-PathSafely $KernelPath)) {
        Write-ColorOutput "Warning: minix/kernel/ directory not found" -Color $Colors.Warning
        return
    }
    
    # Define kernel file mappings
    $KernelMappings = @{
        # Architecture-specific files
        "arch/x86_64/*" = "minix4/exokernel/arch/x86_64/"
        "arch/i386/*" = "minix4/exokernel/arch/i386/"
        "arch/*" = "minix4/exokernel/arch/"
        
        # Core kernel components
        "main*.c" = "minix4/microkernel/core/init/"
        "proc*.c" = "minix4/microkernel/core/process/"
        "sched*.c" = "minix4/microkernel/core/scheduling/"
        "ipc*.c" = "minix4/microkernel/core/ipc/"
        "memory*.c" = "minix4/exokernel/core/resource/memory/"
        "kalloc*.c" = "minix4/exokernel/core/resource/memory/"
        "vm*.c" = "minix4/microkernel/services/vm/"
        "clock*.c" = "minix4/microkernel/core/timing/"
        "interrupt*.c" = "minix4/exokernel/arch/x86_64/interrupts/"
        "trap*.c" = "minix4/exokernel/arch/x86_64/interrupts/"
        
        # System services
        "fs/*" = "minix4/microkernel/services/vfs/"
        "pm/*" = "minix4/microkernel/services/pm/"
        "net/*" = "minix4/microkernel/services/net/"
        
        # Drivers
        "drivers/*" = "minix4/exokernel/drivers/"
        "*driver*.c" = "minix4/exokernel/drivers/"
        
        # Headers
        "include/*" = "minix4/include/"
        "*.h" = "minix4/include/"
        
        # Boot and initialization
        "boot/*" = "minix4/exokernel/arch/x86_64/boot/"
        "bootstrap/*" = "minix4/exokernel/arch/x86_64/boot/"
        "*boot*.c" = "minix4/exokernel/arch/x86_64/boot/"
        "*boot*.S" = "minix4/exokernel/arch/x86_64/boot/"
        
        # Legacy and deprecated
        "legacy/*" = "archive/minix_legacy/kernel_files/"
        "deprecated/*" = "archive/deprecated/"
        "compat/*" = "minix4/compatibility/"
        
        # Assembly files
        "*.S" = "minix4/exokernel/arch/x86_64/"
        "*.s" = "minix4/exokernel/arch/i386/"
        
        # Everything else to archive for manual sorting
        "*" = "archive/minix_legacy/kernel_files/"
    }
    
    # Process kernel file mappings
    foreach ($Pattern in $KernelMappings.Keys) {
        $DestDir = $KernelMappings[$Pattern]
        $SearchPath = Join-Path $KernelPath $Pattern
        
        Write-ColorOutput "  Processing kernel pattern: $Pattern" -Color $Colors.Info
        
        if ($Pattern.Contains("*") -or $Pattern.Contains("/")) {
            try {
                $Items = Get-ChildItem -Path $SearchPath -Recurse -ErrorAction SilentlyContinue
                foreach ($Item in $Items) {
                    $RelativePath = $Item.FullName.Substring($KernelPath.Length + 1)
                    $DestPath = Join-Path $BasePath $DestDir $Item.Name
                    
                    if ($Item.PSIsContainer) {
                        # Create directory structure
                        if (-not $DryRun) {
                            New-Item -ItemType Directory -Path $DestPath -Force -ErrorAction SilentlyContinue | Out-Null
                        }
                    }
                    else {
                        # Move file
                        Move-FileWithLogging -Source $Item.FullName -Destination $DestPath
                    }
                }
            }
            catch {
                Write-ColorOutput "  Warning: Could not process $Pattern - $($_.Exception.Message)" -Color $Colors.Warning
            }
        }
    }
}

function Move-FileWithLogging {
    param(
        [string]$Source,
        [string]$Destination
    )
    
    if (-not (Test-PathSafely $Source)) {
        return
    }
    
    $DestDir = Split-Path $Destination -Parent
    
    if (-not $DryRun) {
        try {
            # Ensure destination directory exists
            if (-not (Test-PathSafely $DestDir)) {
                New-Item -ItemType Directory -Path $DestDir -Force -ErrorAction Stop | Out-Null
            }
            
            # Move the file
            Move-Item -Path $Source -Destination $Destination -Force -ErrorAction Stop
            Write-ColorOutput "    ✓ Moved: $(Split-Path $Source -Leaf) → $Destination" -Color $Colors.Success
        }
        catch {
            Write-ColorOutput "    ✗ Failed to move: $Source - $($_.Exception.Message)" -Color $Colors.Error
        }
    }
    else {
        Write-ColorOutput "    [DRY RUN] Would move: $(Split-Path $Source -Leaf) → $Destination" -Color $Colors.Warning
    }
}

function Remove-EmptyDirectories {
    param([string]$BasePath)
    
    Write-ColorOutput "Cleaning up empty directories..." -Color $Colors.Info
    
    $EmptyDirs = Get-ChildItem -Path $BasePath -Recurse -Directory | 
                 Where-Object { (Get-ChildItem -Path $_.FullName -Force).Count -eq 0 } |
                 Sort-Object FullName -Descending
    
    foreach ($Dir in $EmptyDirs) {
        if (-not $DryRun) {
            try {
                Remove-Item -Path $Dir.FullName -Force -ErrorAction Stop
                Write-ColorOutput "  ✓ Removed empty directory: $($Dir.FullName)" -Color $Colors.Success
            }
            catch {
                Write-ColorOutput "  ✗ Failed to remove: $($Dir.FullName)" -Color $Colors.Warning
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would remove empty: $($Dir.FullName)" -Color $Colors.Warning
        }
    }
}

function Update-BuildConfiguration {
    param([string]$BasePath)
    
    Write-ColorOutput "Updating build configuration..." -Color $Colors.Info
    
    # Create main meson.build for new structure
    $MainMesonContent = @"
# MINIX4 Master Build Configuration
project('minix4', 'c', 'cpp',
  version : '4.0.0',
  default_options : [
    'c_std=c23',
    'cpp_std=c++23',
    'warning_level=3',
    'werror=true',
    'optimization=2',
    'debug=true',
    'b_pie=false',
    'b_staticpic=false'
  ]
)

# Include subdirectories
subdir('minix4/exokernel')
subdir('minix4/microkernel') 
subdir('minix4/libos')
subdir('userspace')
subdir('tests')
"@

    $MainMesonPath = Join-Path $BasePath "meson.build"
    if (-not $DryRun) {
        $MainMesonContent | Out-File -FilePath $MainMesonPath -Encoding UTF8
        Write-ColorOutput "  ✓ Created main meson.build" -Color $Colors.Success
    }
    else {
        Write-ColorOutput "  [DRY RUN] Would create main meson.build" -Color $Colors.Warning
    }
}

function Invoke-GitCommit {
    param(
        [string]$Message,
        [string]$BasePath
    )
    
    if ($DryRun) {
        Write-ColorOutput "[DRY RUN] Would commit: $Message" -Color $Colors.Warning
        return
    }
    
    try {
        Push-Location $BasePath
        git add .
        git commit -m $Message
        Write-ColorOutput "✓ Git commit successful: $Message" -Color $Colors.Success
    }
    catch {
        Write-ColorOutput "✗ Git commit failed: $($_.Exception.Message)" -Color $Colors.Error
    }
    finally {
        Pop-Location
    }
}

# MAIN EXECUTION
function Main {
    Write-Header "MINIX4 COMPLETE FILE STRUCTURE REORGANIZATION"
    Write-ColorOutput "Starting comprehensive restructuring of chaotic codebase..." -Color $Colors.Info
    
    $BasePath = Get-Location
    
    if ($Backup -and -not $DryRun) {
        Write-ColorOutput "Creating backup..." -Color $Colors.Info
        $BackupPath = "$BasePath-backup-$(Get-Date -Format 'yyyyMMdd-HHmmss')"
        Copy-Item -Path $BasePath -Destination $BackupPath -Recurse -Force
        Write-ColorOutput "✓ Backup created: $BackupPath" -Color $Colors.Success
    }
    
    switch ($Phase) {
        1 {
            Write-Phase "EMERGENCY CLEANUP AND STRUCTURE CREATION"
            New-DirectoryStructure -BasePath $BasePath
            Move-RootLevelFiles -BasePath $BasePath
            if (-not $DryRun) {
                Invoke-GitCommit -Message "Phase 1: Emergency cleanup and new directory structure" -BasePath $BasePath
            }
        }
        
        2 {
            Write-Phase "KERNEL REORGANIZATION AND CONSOLIDATION"
            Move-MinixKernelFiles -BasePath $BasePath
            Remove-EmptyDirectories -BasePath $BasePath
            if (-not $DryRun) {
                Invoke-GitCommit -Message "Phase 2: Kernel reorganization and consolidation" -BasePath $BasePath
            }
        }
        
        3 {
            Write-Phase "BUILD SYSTEM AND FINAL CLEANUP"
            Update-BuildConfiguration -BasePath $BasePath
            Remove-EmptyDirectories -BasePath $BasePath
            if (-not $DryRun) {
                Invoke-GitCommit -Message "Phase 3: Build system update and final cleanup" -BasePath $BasePath
            }
        }
    }
    
    Write-Header "RESTRUCTURING PHASE $Phase COMPLETED"
    
    if ($DryRun) {
        Write-ColorOutput "DRY RUN MODE - No actual changes were made" -Color $Colors.Warning
        Write-ColorOutput "Review the output above and run without -DryRun to execute" -Color $Colors.Info
    }
    else {
        Write-ColorOutput "Phase $Phase completed successfully!" -Color $Colors.Success
        Write-ColorOutput "Run the next phase when ready: -Phase $($Phase + 1)" -Color $Colors.Info
    }
}

# Execute main function
Main
