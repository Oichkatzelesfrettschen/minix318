#!/usr/bin/env pwsh
<#
.SYNOPSIS
    MINIX4 File Restructuring Script (Working Version)
.DESCRIPTION
    PowerShell script to perform critical file moves to transform
    the chaotic MINIX structure into a clean MINIX4 architecture.
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

function Move-FilesSafely {
    param(
        [string]$SourcePattern,
        [string]$Destination,
        [string]$Description
    )
    
    Write-ColorOutput "`nMoving $Description..." -Color "Cyan"
    
    $Files = Get-ChildItem -Path $SourcePattern -ErrorAction SilentlyContinue
    foreach ($File in $Files) {
        $DestPath = Join-Path $Destination $File.Name
        
        if (-not $DryRun) {
            try {
                Move-Item -Path $File.FullName -Destination $DestPath -Force -ErrorAction Stop
                Write-ColorOutput "  ✓ Moved: $($File.Name) → $Destination" -Color "Green"
            }
            catch {
                Write-ColorOutput "  ✗ Failed: $($File.Name) → $Destination" -Color "Red"
            }
        }
        else {
            Write-ColorOutput "  [DRY RUN] Would move: $($File.Name) → $Destination" -Color "Yellow"
        }
    }
}

function Perform-CriticalMoves {
    Write-Header "PERFORMING CRITICAL FILE MOVES"
    
    # Move Pascal interpreter files
    $PascalFiles = @("lookup.c", "lval.c", "rec.c", "rval.c", "tree.c", "type.c", "var.c", "proc.c")
    foreach ($File in $PascalFiles) {
        if (Test-PathSafely $File) {
            $DestPath = "archive/unix_v6v7/pascal/$File"
            if (-not $DryRun) {
                try {
                    Move-Item -Path $File -Destination $DestPath -Force
                    Write-ColorOutput "  ✓ Moved Pascal file: $File → archive/unix_v6v7/pascal/" -Color "Green"
                }
                catch {
                    Write-ColorOutput "  ✗ Failed to move: $File" -Color "Red"
                }
            }
            else {
                Write-ColorOutput "  [DRY RUN] Would move Pascal: $File → archive/unix_v6v7/pascal/" -Color "Yellow"
            }
        }
    }
    
    # Move YACC files
    $YaccFiles = @("y.tab.c", "y.tab.h", "yy*.c")
    foreach ($Pattern in $YaccFiles) {
        Move-FilesSafely $Pattern "archive/unix_v6v7/yacc" "YACC files"
    }
    
    # Move kernel timing files
    if (Test-PathSafely "minix/clock.c") {
        Move-FilesSafely "minix/clock.c" "minix4/microkernel/core/timing" "clock implementation"
    }
    
    # Move demo files
    Move-FilesSafely "*_demo.c" "userspace/applications/demos" "demo files"
    Move-FilesSafely "*_test.c" "userspace/applications/demos" "test files"
    
    # Move library files
    if (Test-PathSafely "malloc.c") {
        Move-FilesSafely "malloc.c" "minix4/libos/posix/libc/stdlib" "malloc implementation"
    }
    
    if (Test-PathSafely "string.c") {
        Move-FilesSafely "string.c" "minix4/libos/posix/libc/string" "string implementation"
    }
    
    # Move remaining mail system files  
    if (Test-PathSafely "main.c") {
        Move-FilesSafely "main.c" "archive/minix_legacy" "remaining main.c (mail system)"
    }
}

function Show-Summary {
    Write-Header "RESTRUCTURING SUMMARY"
    
    Write-ColorOutput "MINIX4 Restructuring Complete!" -Color "Green"
    Write-ColorOutput "`nNew Structure Created:" -Color "Cyan"
    Write-ColorOutput "  📁 minix4/           - Modern MINIX4 kernel and OS" -Color "White"
    Write-ColorOutput "  📁 userspace/        - All user applications and commands" -Color "White"
    Write-ColorOutput "  📁 archive/          - Legacy code preservation" -Color "White"
    
    Write-ColorOutput "`nNext Steps:" -Color "Yellow"
    Write-ColorOutput "  1. Update build system for new structure" -Color "White"
    Write-ColorOutput "  2. Consolidate remaining duplicate files" -Color "White"
    Write-ColorOutput "  3. Test compilation in new structure" -Color "White"
    Write-ColorOutput "  4. Update documentation and makefiles" -Color "White"
}

# Main execution
try {
    Write-Header "MINIX4 EMERGENCY RESTRUCTURING SCRIPT"
    
    if ($DryRun) {
        Write-ColorOutput "🔍 DRY RUN MODE - No files will be moved" -Color "Yellow"
    }
    else {
        Write-ColorOutput "⚠️  LIVE MODE - Files will be moved!" -Color "Red"
        Write-ColorOutput "Ctrl+C to cancel, Enter to proceed..." -Color "Yellow"
        Read-Host
    }
    
    New-DirectoryStructure
    Perform-CriticalMoves
    Show-Summary
    
    Write-ColorOutput "`n✅ Restructuring completed successfully!" -Color "Green"
}
catch {
    Write-ColorOutput "`n❌ Error occurred: $($_.Exception.Message)" -Color "Red"
    exit 1
}
