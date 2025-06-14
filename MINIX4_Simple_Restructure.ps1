#!/usr/bin/env pwsh
# MINIX4 Simple Restructuring Script

param([switch]$DryRun)

Write-Host "===============================================" -ForegroundColor Magenta
Write-Host " MINIX4 RESTRUCTURING SCRIPT" -ForegroundColor Magenta
Write-Host "===============================================" -ForegroundColor Magenta

if ($DryRun) {
    Write-Host "🔍 DRY RUN MODE - No files will be moved" -ForegroundColor Yellow
} else {
    Write-Host "⚠️  LIVE MODE - Files will be moved!" -ForegroundColor Red
    Write-Host "Press Enter to continue or Ctrl+C to cancel..."
    Read-Host
}

# Create directory structure
Write-Host "`nCreating MINIX4 directory structure..." -ForegroundColor Cyan

$Dirs = @(
    "minix4",
    "minix4/exokernel",
    "minix4/exokernel/arch/x86_64",
    "minix4/exokernel/arch/i386",
    "minix4/microkernel/core/timing",
    "minix4/microkernel/core/process",
    "minix4/libos/posix/libc/stdlib",
    "minix4/libos/posix/libc/string",
    "userspace/applications/demos",
    "archive/unix_v6v7/pascal",
    "archive/unix_v6v7/yacc",
    "archive/minix_legacy"
)

foreach ($Dir in $Dirs) {
    if ($DryRun) {
        Write-Host "  [DRY] Would create: $Dir" -ForegroundColor Yellow
    } else {
        New-Item -ItemType Directory -Path $Dir -Force | Out-Null
        Write-Host "  ✓ Created: $Dir" -ForegroundColor Green
    }
}

# Move Pascal files
Write-Host "`nMoving Pascal interpreter files..." -ForegroundColor Cyan
$PascalFiles = @("lookup.c", "lval.c", "rec.c", "rval.c", "tree.c", "type.c", "var.c")
foreach ($File in $PascalFiles) {
    if (Test-Path $File) {
        if ($DryRun) {
            Write-Host "  [DRY] Would move: $File → archive/unix_v6v7/pascal/" -ForegroundColor Yellow
        } else {
            Move-Item -Path $File -Destination "archive/unix_v6v7/pascal/$File" -Force
            Write-Host "  ✓ Moved: $File → archive/unix_v6v7/pascal/" -ForegroundColor Green
        }
    }
}

# Move YACC files
Write-Host "`nMoving YACC files..." -ForegroundColor Cyan
$YaccFiles = Get-ChildItem -Name "yy*.c", "y.tab.*"
foreach ($File in $YaccFiles) {
    if ($DryRun) {
        Write-Host "  [DRY] Would move: $File → archive/unix_v6v7/yacc/" -ForegroundColor Yellow
    } else {
        Move-Item -Path $File -Destination "archive/unix_v6v7/yacc/$File" -Force
        Write-Host "  ✓ Moved: $File → archive/unix_v6v7/yacc/" -ForegroundColor Green
    }
}

# Move demo files
Write-Host "`nMoving demo and test files..." -ForegroundColor Cyan
$DemoFiles = Get-ChildItem -Name "*_demo.c", "*_test.c"
foreach ($File in $DemoFiles) {
    if ($DryRun) {
        Write-Host "  [DRY] Would move: $File → userspace/applications/demos/" -ForegroundColor Yellow
    } else {
        Move-Item -Path $File -Destination "userspace/applications/demos/$File" -Force
        Write-Host "  ✓ Moved: $File → userspace/applications/demos/" -ForegroundColor Green
    }
}

# Move specific files
Write-Host "`nMoving specific system files..." -ForegroundColor Cyan

# Move malloc.c to stdlib
if (Test-Path "malloc.c") {
    if ($DryRun) {
        Write-Host "  [DRY] Would move: malloc.c → minix4/libos/posix/libc/stdlib/" -ForegroundColor Yellow
    } else {
        Move-Item -Path "malloc.c" -Destination "minix4/libos/posix/libc/stdlib/malloc.c" -Force
        Write-Host "  ✓ Moved: malloc.c → minix4/libos/posix/libc/stdlib/" -ForegroundColor Green
    }
}

# Move string.c to string lib
if (Test-Path "string.c") {
    if ($DryRun) {
        Write-Host "  [DRY] Would move: string.c → minix4/libos/posix/libc/string/" -ForegroundColor Yellow
    } else {
        Move-Item -Path "string.c" -Destination "minix4/libos/posix/libc/string/string.c" -Force
        Write-Host "  ✓ Moved: string.c → minix4/libos/posix/libc/string/" -ForegroundColor Green
    }
}

# Archive remaining root files
Write-Host "`nArchiving remaining root files..." -ForegroundColor Cyan
$RootFiles = Get-ChildItem -Name "*.c" | Where-Object { $_ -notlike "*unified*" -and $_ -notlike "*modern*" }
foreach ($File in $RootFiles) {
    if ($DryRun) {
        Write-Host "  [DRY] Would archive: $File → archive/minix_legacy/" -ForegroundColor Yellow
    } else {
        Move-Item -Path $File -Destination "archive/minix_legacy/$File" -Force
        Write-Host "  ✓ Archived: $File → archive/minix_legacy/" -ForegroundColor Green
    }
}

Write-Host "`n===============================================" -ForegroundColor Magenta
Write-Host " RESTRUCTURING COMPLETE!" -ForegroundColor Green
Write-Host "===============================================" -ForegroundColor Magenta

Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "  1. Update build system for new structure" -ForegroundColor White
Write-Host "  2. Consolidate duplicate files in minix/ directory" -ForegroundColor White
Write-Host "  3. Test compilation" -ForegroundColor White
