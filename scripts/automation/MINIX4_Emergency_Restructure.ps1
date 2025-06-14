#!/usr/bin/env pwsh
<#
.SYNOPSIS
    MINIX4 Emergency File Restructuring Script (Simplified)
.DESCRIPTION
    Simplified PowerShell script to perform the most critical file moves
    to transform the chaotic MINIX structure into a clean architecture.
.PARAMETER DryRun
    Run in test mode without actually moving files
#>

[CmdletBinding()]
param(
    [switch]$DryRun
)

# Script configuration
$ErrorActionPreference = "Stop"

function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

function Write-Header {
    param([string]$Text)
    Write-ColorOutput "`n" + ("="*80) -Color "Magenta"
    Write-ColorOutput " $Text" -Color "Magenta"
    Write-ColorOutput ("="*80) -Color "Magenta"
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
    Write-ColorOutput "Creating MINIX4 directory structure..." -Color "Cyan"
    
    # Define the new directory structure
    $NewDirs = @(
        "minix4",
        "minix4/exokernel",
        "minix4/exokernel/arch",
        "minix4/exokernel/arch/x86_64",
        "minix4/exokernel/arch/i386", 
        "minix4/exokernel/core",
        "minix4/exokernel/core/resource",
        "minix4/exokernel/core/resource/memory",
        "minix4/exokernel/drivers",
        "minix4/microkernel",
        "minix4/microkernel/core",
        "minix4/microkernel/core/init",
        "minix4/microkernel/core/process",
        "minix4/microkernel/core/timing",
        "minix4/microkernel/core/ipc",
        "minix4/microkernel/services",
        "minix4/microkernel/services/vfs",
        "minix4/microkernel/services/pm",
        "minix4/microkernel/services/vm",
        "minix4/libos",
        "minix4/libos/posix",
        "minix4/libos/posix/libc",
        "minix4/libos/posix/libc/stdlib",
        "minix4/libos/posix/libc/stdio",
        "minix4/libos/posix/libc/string",
        "minix4/libos/rump",
        "minix4/include",
        "userspace",
        "userspace/commands",
        "userspace/commands/core",
        "userspace/applications",
        "userspace/applications/demos",
        "archive",
        "archive/minix_legacy",
        "archive/unix_v6v7",
        "archive/unix_v6v7/yacc",
        "archive/unix_v6v7/pascal"
    )
    
    foreach ($Dir in $NewDirs) {
        if (-not $DryRun) {
            try {
                New-Item -ItemType Directory -Path $Dir -Force -ErrorAction Stop | Out-Null
                Write-ColorOutput "  ✓ Created: $Dir" -Color "Green"
            }
            catch {
                Write-ColorOutput "  ✗ Failed to create: $Dir" -Color "Red"
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would create: $Dir" -Color "Yellow"
        }
    }
}

function Move-RootChaosFiles {
    Write-ColorOutput "`nMoving root-level chaos files..." -Color "Cyan"
    
    # Critical file mappings
    $FileMappings = @{
        "clock.c" = "minix4/microkernel/core/timing/clock.c"
        "proc.c" = "minix4/microkernel/core/process/proc.c"
        "malloc.c" = "minix4/libos/posix/libc/stdlib/malloc.c"
        "string.c" = "minix4/libos/posix/libc/string/string.c"
        "printf.c" = "minix4/libos/posix/libc/stdio/printf.c"
        "stat.c" = "userspace/commands/core/stat.c"
        "wumpus.c" = "userspace/applications/demos/wumpus.c"
        
        # YACC files to archive
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
        "yyprint.c" = "archive/unix_v6v7/yacc/yyprint.c"
        "yycopy.s" = "archive/unix_v6v7/yacc/yycopy.s"
        
        # Assembly files
        "printf.s" = "minix4/exokernel/arch/i386/printf.s"
        "putc.s" = "minix4/exokernel/arch/i386/putc.s"
        "putn.s" = "minix4/exokernel/arch/i386/putn.s"
        "signal.s" = "minix4/exokernel/arch/i386/signal.s"
        "treen.s" = "minix4/exokernel/arch/i386/treen.s"
        "wait.s" = "minix4/exokernel/arch/i386/wait.s"
        
        # Headers
        "tree.h" = "minix4/include/tree.h"
        "send.h" = "minix4/microkernel/core/ipc/send.h"
        
        # Pascal interpreter files to archive
        "main copy.c" = "archive/unix_v6v7/pascal/main_copy.c"
        "lookup.c" = "archive/unix_v6v7/pascal/lookup.c"
        "lval.c" = "archive/unix_v6v7/pascal/lval.c"
        "rec.c" = "archive/unix_v6v7/pascal/rec.c"
        "receive.c" = "archive/unix_v6v7/pascal/receive.c"
        "rval.c" = "archive/unix_v6v7/pascal/rval.c"
        "send.c" = "archive/unix_v6v7/pascal/send.c"
        "subr.c" = "archive/unix_v6v7/pascal/subr.c"
        "TRdata.c" = "archive/unix_v6v7/pascal/TRdata.c"
        "tree.c" = "archive/unix_v6v7/pascal/tree.c"
        "type.c" = "archive/unix_v6v7/pascal/type.c"
        "var.c" = "archive/unix_v6v7/pascal/var.c"
        "Version.c" = "archive/unix_v6v7/pascal/Version.c"
        "0.h" = "archive/unix_v6v7/pascal/0.h"
        "opcode.h" = "archive/unix_v6v7/pascal/opcode.h"
        "OPnames.h" = "archive/unix_v6v7/pascal/OPnames.h"
    }
    
    foreach ($SourceFile in $FileMappings.Keys) {
        $DestPath = $FileMappings[$SourceFile]
        
        if (Test-PathSafely $SourceFile) {
            if (-not $DryRun) {
                try {
                    $DestDir = Split-Path $DestPath -Parent
                    if (-not (Test-PathSafely $DestDir)) {
                        New-Item -ItemType Directory -Path $DestDir -Force | Out-Null
                    }
                    Move-Item -Path $SourceFile -Destination $DestPath -Force
                    Write-ColorOutput "  ✓ Moved: $SourceFile → $DestPath" -Color "Green"
                }
                catch {
                    Write-ColorOutput "  ✗ Failed to move: $SourceFile" -Color "Red"
                }
            }
            else {
                Write-ColorOutput "  [DRY RUN] Would move: $SourceFile → $DestPath" -Color "Yellow"
            }
        }
    }
}

function Move-DemoFiles {
    Write-ColorOutput "`nMoving demo and test files..." -Color "Cyan"
    
    # Move demo files
    $DemoFiles = Get-ChildItem -Filter "*_demo.c" -ErrorAction SilentlyContinue
    foreach ($File in $DemoFiles) {
        $DestPath = "userspace/applications/demos/$($File.Name)"
        if (-not $DryRun) {
            try {
                Move-Item -Path $File.FullName -Destination $DestPath -Force
                Write-ColorOutput "  ✓ Moved demo: $($File.Name)" -Color "Green"
            }
            catch {
                Write-ColorOutput "  ✗ Failed to move demo: $($File.Name)" -Color "Red"
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would move demo: $($File.Name)" -Color "Yellow"
        }
    }
    
    # Move test files  
    $TestFiles = Get-ChildItem -Filter "*_test.c" -ErrorAction SilentlyContinue
    foreach ($File in $TestFiles) {
        $DestPath = "tests/unit/$($File.Name)"
        if (-not $DryRun) {
            try {
                if (-not (Test-PathSafely "tests/unit")) {
                    New-Item -ItemType Directory -Path "tests/unit" -Force | Out-Null
                }
                Move-Item -Path $File.FullName -Destination $DestPath -Force
                Write-ColorOutput "  ✓ Moved test: $($File.Name)" -Color "Green"
            }
            catch {
                Write-ColorOutput "  ✗ Failed to move test: $($File.Name)" -Color "Red"
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would move test: $($File.Name)" -Color "Yellow"
        }
    }
}

function Archive-RemainingChaos {
    Write-ColorOutput "`nArchiving remaining chaos files..." -Color "Cyan"
    
    # Archive remaining .c files that shouldn't be in root
    $RemainingFiles = Get-ChildItem -Filter "*.c" -ErrorAction SilentlyContinue |
                      Where-Object { $_.Name -notlike "*_demo.c" -and $_.Name -notlike "*_test.c" }
    
    foreach ($File in $RemainingFiles) {
        $DestPath = "archive/minix_legacy/$($File.Name)"
        if (-not $DryRun) {
            try {
                Move-Item -Path $File.FullName -Destination $DestPath -Force
                Write-ColorOutput "  ✓ Archived: $($File.Name)" -Color "Green"
            }
            catch {
                Write-ColorOutput "  ✗ Failed to archive: $($File.Name)" -Color "Red"
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would archive: $($File.Name)" -Color "Yellow"
        }
    }
}

function Show-Summary {
    Write-Header "PHASE 1 RESTRUCTURING SUMMARY"
    
    Write-ColorOutput "What Phase 1 accomplishes:" -Color "Cyan"
    Write-ColorOutput "✓ Creates clean MINIX4 hybrid architecture directories" -Color "Green"
    Write-ColorOutput "✓ Moves critical kernel files to proper locations" -Color "Green"  
    Write-ColorOutput "✓ Archives legacy YACC/Pascal files separately" -Color "Green"
    Write-ColorOutput "✓ Organizes demo and test files properly" -Color "Green"
    Write-ColorOutput "✓ Removes chaos from root directory" -Color "Green"
    
    Write-ColorOutput "`nNext steps after Phase 1:" -Color "Cyan"
    Write-ColorOutput "• Phase 2: Reorganize minix/kernel/ directory (300+ files)" -Color "Yellow"
    Write-ColorOutput "• Phase 3: Consolidate duplicates and finalize build system" -Color "Yellow"
    Write-ColorOutput "• Phase 4: Implement hybrid architecture properly" -Color "Yellow"
    
    if ($DryRun) {
        Write-ColorOutput "`n🚨 THIS WAS A DRY RUN - NO FILES WERE MOVED" -Color "Red"
        Write-ColorOutput "Run without -DryRun to execute the restructuring" -Color "Yellow"
    }
    else {
        Write-ColorOutput "`n✅ PHASE 1 COMPLETED SUCCESSFULLY!" -Color "Green"
        Write-ColorOutput "The chaos is now organized into a clean architecture!" -Color "Green"
    }
}

# MAIN EXECUTION
Write-Header "MINIX4 EMERGENCY FILE RESTRUCTURING - PHASE 1"

if ($DryRun) {
    Write-ColorOutput "🧪 RUNNING IN DRY-RUN MODE (no files will be moved)" -Color "Yellow"
}
else {
    Write-ColorOutput "⚠️  EXECUTING REAL MOVES - FILES WILL BE RELOCATED!" -Color "Red"
}

try {
    New-DirectoryStructure
    Move-RootChaosFiles
    Move-DemoFiles
    Archive-RemainingChaos
    Show-Summary
}
catch {
    Write-ColorOutput "❌ Error during restructuring: $($_.Exception.Message)" -Color "Red"
    exit 1
}

exit 0
