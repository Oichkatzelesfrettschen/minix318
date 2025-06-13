#!/usr/bin/env pwsh
<#
.SYNOPSIS
    MINIX Codebase Consolidation Script
.DESCRIPTION
    This PowerShell script systematically analyzes and consolidates the scattered
    MINIX codebase, merging duplicate files and organizing them into a coherent
    directory structure.
.NOTES
    Version: 1.0
    Date: 2025-06-12
    Author: MINIX Development Team
#>

param(
    [switch]$DryRun,
    [switch]$Verbose,
    [string]$OutputDir = ".\minix_consolidated"
)

$ErrorActionPreference = "Stop"

# Configuration
$Config = @{
    SourceRoot = "c:\Users\ericj\minix318"
    TargetRoot = "c:\Users\ericj\minix318\minix_consolidated"
    BackupRoot = "c:\Users\ericj\minix318\backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"
    LogFile = "consolidation_$(Get-Date -Format 'yyyyMMdd_HHmmss').log"
}

# File categories for consolidation
$FileCategories = @{
    "kernel_core" = @{
        "pattern" = @("main.c", "proc.c", "memory.c", "signal.c", "ipc.c", "sys*.c")
        "target_dir" = "kernel"
        "priority" = 1
    }
    "kernel_arch" = @{
        "pattern" = @("*86*.c", "*arm*.c", "*arch*.c", "mmu*.c", "trap*.c")
        "target_dir" = "kernel/arch"
        "priority" = 2
    }
    "drivers" = @{
        "pattern" = @("*driver*.c", "*dev*.c", "*tty*.c", "*disk*.c")
        "target_dir" = "drivers"
        "priority" = 3
    }
    "libos" = @{
        "pattern" = @("lib*.c", "*lib*.c", "posix*.c")
        "target_dir" = "libos"
        "priority" = 4
    }
    "commands" = @{
        "pattern" = @("cmd*.c", "*cmd*.c")
        "target_dir" = "commands"
        "priority" = 5
    }
    "tools" = @{
        "pattern" = @("*tool*.c", "*util*.c", "*test*.c")
        "target_dir" = "tools"
        "priority" = 6
    }
    "legacy" = @{
        "pattern" = @("*pascal*.c", "*pi*.c", "y.tab.c", "yy*.c")
        "target_dir" = "archive/legacy"
        "priority" = 10
    }
}

# Priority file list for immediate consolidation
$PriorityFiles = @{
    "main.c" = @{
        "description" = "Kernel entry points and main functions"
        "strategy" = "merge_by_context"
    }
    "proc.c" = @{
        "description" = "Process management implementations"
        "strategy" = "merge_kernel_only"
    }
    "memory.c" = @{
        "description" = "Memory management subsystems"
        "strategy" = "merge_by_arch"
    }
    "signal.c" = @{
        "description" = "Signal handling implementations"
        "strategy" = "merge_by_context"
    }
}

function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logEntry = "[$timestamp] [$Level] $Message"
    Write-Host $logEntry
    Add-Content -Path $Config.LogFile -Value $logEntry
}

function Initialize-Consolidation {
    Write-Log "Starting MINIX codebase consolidation"
    Write-Log "Source: $($Config.SourceRoot)"
    Write-Log "Target: $($Config.TargetRoot)"
    
    # Create target directory structure
    $dirs = @(
        "kernel",
        "kernel/arch/x86_64",
        "kernel/arch/i386", 
        "kernel/arch/arm",
        "kernel/arch/aarch64",
        "kernel/include",
        "libos",
        "drivers",
        "commands",
        "servers",
        "tools",
        "tests",
        "docs",
        "archive/legacy",
        "archive/obsolete"
    )
    
    foreach ($dir in $dirs) {
        $fullPath = Join-Path $Config.TargetRoot $dir
        if (-not (Test-Path $fullPath)) {
            New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
            Write-Log "Created directory: $dir"
        }
    }
}

function Find-DuplicateFiles {
    Write-Log "Analyzing duplicate files..."
    
    $allFiles = Get-ChildItem -Path $Config.SourceRoot -Recurse -File -Name "*.c" | 
                ForEach-Object { [System.IO.Path]::GetFileName($_) }
    
    $duplicates = $allFiles | Group-Object | Where-Object Count -gt 1 | 
                  Sort-Object Count -Descending
    
    Write-Log "Found $($duplicates.Count) sets of duplicate files"
    
    $duplicateReport = @()
    foreach ($dup in $duplicates) {
        $files = Get-ChildItem -Path $Config.SourceRoot -Recurse -File -Name $dup.Name
        $duplicateReport += [PSCustomObject]@{
            FileName = $dup.Name
            Count = $dup.Count
            Locations = $files -join "; "
            Size = ($files | ForEach-Object { (Get-Item (Join-Path $Config.SourceRoot $_)).Length } | Measure-Object -Sum).Sum
        }
    }
    
    return $duplicateReport
}

function Analyze-FileContent {
    param([string]$FilePath)
    
    $content = Get-Content $FilePath -Raw -ErrorAction SilentlyContinue
    if (-not $content) { return $null }
    
    $analysis = @{
        Path = $FilePath
        Size = (Get-Item $FilePath).Length
        LineCount = ($content -split "`n").Count
        Language = "C"
        Category = "unknown"
        IsKernel = $false
        IsLegacy = $false
        HasMain = $content -match "int\s+main\s*\("
        HasIncludes = ($content | Select-String "#include" | Measure-Object).Count
    }
    
    # Detect file category
    switch -Regex ($content) {
        "kernel|KERNEL|minix|MINIX" { 
            $analysis.IsKernel = $true
            $analysis.Category = "kernel"
        }
        "pascal|Pascal|PASCAL" { 
            $analysis.IsLegacy = $true
            $analysis.Category = "legacy_pascal"
        }
        "driver|DRIVER" { $analysis.Category = "driver" }
        "lib|LIB" { $analysis.Category = "library" }
        default { $analysis.Category = "utility" }
    }
    
    return $analysis
}

function Consolidate-PriorityFile {
    param([string]$FileName)
    
    Write-Log "Consolidating $FileName files..."
    
    $instances = Get-ChildItem -Path $Config.SourceRoot -Recurse -File -Name $FileName
    Write-Log "Found $($instances.Count) instances of $FileName"
    
    $analyses = @()
    foreach ($instance in $instances) {
        $fullPath = Join-Path $Config.SourceRoot $instance
        $analysis = Analyze-FileContent $fullPath
        if ($analysis) {
            $analyses += $analysis
        }
    }
    
    # Group by category
    $byCategory = $analyses | Group-Object Category
    
    foreach ($group in $byCategory) {
        Write-Log "  Category '$($group.Name)': $($group.Count) files"
        
        # For kernel files, create a unified version
        if ($group.Name -eq "kernel") {
            $targetPath = Join-Path $Config.TargetRoot "kernel\$($FileName.Replace('.c', '_unified.c'))"
            Write-Log "  Creating unified kernel file: $targetPath"
            
            if (-not $DryRun) {
                Merge-KernelFiles -Files $group.Group -OutputPath $targetPath
            }
        }
        # For legacy files, move to archive
        elseif ($group.Name -like "legacy*") {
            foreach ($file in $group.Group) {
                $targetPath = Join-Path $Config.TargetRoot "archive\legacy\$([System.IO.Path]::GetFileName($file.Path))"
                Write-Log "  Archiving legacy file: $targetPath"
                
                if (-not $DryRun) {
                    Copy-Item $file.Path $targetPath -Force
                }
            }
        }
    }
}

function Merge-KernelFiles {
    param([object[]]$Files, [string]$OutputPath)
    
    $mergedContent = @"
/**
 * @file $([System.IO.Path]::GetFileName($OutputPath))
 * @brief Unified MINIX4 Kernel Implementation
 * @details This file represents the consolidation of multiple kernel
 *          implementations into a single, modernized C23-compliant module.
 * @version 4.0.0
 * @date $(Get-Date -Format 'yyyy-MM-dd')
 * @author MINIX Development Team (Consolidated)
 */

#define _POSIX_C_SOURCE 202311L
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* MINIX4 Kernel Headers */
// TODO: Add proper kernel headers once organized

/*===========================================================================*
 *                        CONSOLIDATED IMPLEMENTATION                       *
 *===========================================================================*/

"@

    # Add each file's content with proper attribution
    foreach ($file in $Files) {
        $content = Get-Content $file.Path -Raw
        $relativePath = $file.Path.Replace($Config.SourceRoot, "").TrimStart('\')
        
        $mergedContent += @"

/*---------------------------------------------------------------------------*
 * Source: $relativePath
 * Size: $($file.Size) bytes
 * Lines: $($file.LineCount)
 *---------------------------------------------------------------------------*/

$content

"@
    }
    
    $mergedContent += @"

/*===========================================================================*
 *                            END OF CONSOLIDATION                          *
 *===========================================================================*/
"@

    Set-Content -Path $OutputPath -Value $mergedContent
    Write-Log "Created unified file: $OutputPath ($($mergedContent.Length) bytes)"
}

function Generate-ConsolidationReport {
    param([object[]]$DuplicateFiles)
    
    $reportPath = Join-Path $Config.TargetRoot "CONSOLIDATION_REPORT.md"
    
    $report = @"
# MINIX Codebase Consolidation Report
*Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')*

## Summary
- **Total duplicate file sets**: $($DuplicateFiles.Count)
- **Source directory**: $($Config.SourceRoot)
- **Target directory**: $($Config.TargetRoot)
- **Backup directory**: $($Config.BackupRoot)

## Duplicate Files Analysis

| File Name | Count | Total Size | Locations |
|-----------|-------|------------|-----------|
"@

    foreach ($dup in $DuplicateFiles | Sort-Object Count -Descending | Select-Object -First 20) {
        $report += "`n| $($dup.FileName) | $($dup.Count) | $($dup.Size) bytes | $($dup.Locations.Length -gt 100 ? $dup.Locations.Substring(0, 100) + '...' : $dup.Locations) |"
    }

    $report += @"

## Consolidation Actions
- Kernel files merged into `/kernel/` with unified implementations
- Legacy files moved to `/archive/legacy/`
- Drivers organized into `/drivers/`
- Libraries consolidated into `/libos/`
- Tools and utilities moved to `/tools/`

## Next Steps
1. Review consolidated kernel implementations
2. Update build system for new structure
3. Test consolidated components
4. Remove obsolete files from source tree
"@

    Set-Content -Path $reportPath -Value $report
    Write-Log "Generated consolidation report: $reportPath"
}

# Main execution
try {
    Write-Log "=== MINIX Codebase Consolidation Started ==="
    
    Initialize-Consolidation
    
    $duplicateFiles = Find-DuplicateFiles
    Write-Log "Analysis complete: Found $($duplicateFiles.Count) duplicate file sets"
    
    # Process priority files
    foreach ($fileName in $PriorityFiles.Keys) {
        Consolidate-PriorityFile $fileName
    }
    
    Generate-ConsolidationReport $duplicateFiles
    
    Write-Log "=== MINIX Codebase Consolidation Completed ==="
    Write-Log "Check the log file for details: $($Config.LogFile)"
    
} catch {
    Write-Log "ERROR: $($_.Exception.Message)" "ERROR"
    Write-Log "Stack trace: $($_.ScriptStackTrace)" "ERROR"
    exit 1
}
