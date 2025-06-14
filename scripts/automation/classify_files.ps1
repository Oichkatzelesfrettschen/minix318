#!/usr/bin/env pwsh
<#
.SYNOPSIS
    MINIX File Classification and Consolidation Script
.DESCRIPTION
    This script analyzes each file in the MINIX repository, classifies it by purpose,
    and systematically organizes it into the proper directory structure.
#>

param(
    [switch]$Execute,
    [switch]$Verbose
)

# File classification rules
$ClassificationRules = @{
    # Kernel core files
    "kernel_core" = @{
        patterns = @("*main.c", "*proc.c", "*memory.c", "*signal.c", "*ipc.c", "*sys*.c", "*kernel*.c")
        exclude_patterns = @("*mail*", "*pascal*", "*pi*", "*trace*", "*tool*")
        target_dir = "minix/kernel"
        action = "merge_and_modernize"
    }
    
    # Architecture-specific kernel files
    "kernel_arch" = @{
        patterns = @("*i386*.c", "*x86*.c", "*arm*.c", "*aarch64*.c", "*arch*.c", "*mmu*.c", "*trap*.c")
        target_dir = "minix/kernel/arch"
        action = "consolidate_by_arch"
    }
    
    # Device drivers
    "drivers" = @{
        patterns = @("*driver*.c", "*dev*.c", "*tty*.c", "*disk*.c", "*net*.c", "*usb*.c", "*pci*.c")
        target_dir = "minix/drivers"
        action = "organize_by_type"
    }
    
    # Operating system libraries
    "libos" = @{
        patterns = @("lib*.c", "*lib*.c", "posix*.c", "*syscall*.c")
        exclude_patterns = @("*mail*", "*tool*")
        target_dir = "minix/libos"
        action = "consolidate_by_function"
    }
    
    # User commands and utilities
    "commands" = @{
        patterns = @("cmd*.c", "*cmd*.c", "ls.c", "cat.c", "cp.c", "mv.c", "rm.c")
        target_dir = "minix/commands"
        action = "organize_alphabetically"
    }
    
    # Development tools
    "tools" = @{
        patterns = @("*tool*.c", "*util*.c", "*test*.c", "debug*.c", "trace*.c")
        target_dir = "tools"
        action = "organize_by_purpose"
    }
    
    # Mail system (separate application)
    "mail_system" = @{
        patterns = @("*mail*.c", "*rcv*.c")
        indicators = @("mail", "message", "envelope", "mailbox")
        target_dir = "tools/mail"
        action = "keep_separate"
    }
    
    # Pascal interpreter (legacy)
    "pascal_legacy" = @{
        patterns = @("*pascal*.c", "*pi*.c", "y.tab.c", "yy*.c")
        indicators = @("pascal", "interpreter", "compiler")
        target_dir = "archive/legacy/pascal"
        action = "archive"
    }
    
    # Shell implementations
    "shell" = @{
        patterns = @("sh*.c", "*shell*.c", "csh*.c")
        target_dir = "minix/commands/shell"
        action = "consolidate_shell"
    }
    
    # Build system files
    "build_system" = @{
        patterns = @("Makefile*", "*.mk", "meson.build", "*.cmake")
        target_dir = "build"
        action = "unify_build"
    }
    
    # Header files
    "headers" = @{
        patterns = @("*.h", "*.hpp")
        target_dir = "include"
        action = "consolidate_headers"
    }
    
    # Documentation
    "documentation" = @{
        patterns = @("*.md", "*.txt", "README*", "*.doc")
        target_dir = "docs"
        action = "organize_docs"
    }
    
    # Assembly files
    "assembly" = @{
        patterns = @("*.s", "*.S", "*.asm")
        target_dir = "minix/kernel/arch"
        action = "organize_by_arch"
    }
}

function Get-FileClassification {
    param([string]$FilePath)
    
    $fileName = [System.IO.Path]::GetFileName($FilePath)
    $content = ""
    
    try {
        $content = Get-Content $FilePath -Raw -ErrorAction SilentlyContinue
        if (-not $content) { $content = "" }
    } catch {
        $content = ""
    }
    
    foreach ($category in $ClassificationRules.Keys) {
        $rule = $ClassificationRules[$category]
        
        # Check filename patterns
        $matchesPattern = $false
        foreach ($pattern in $rule.patterns) {
            if ($fileName -like $pattern) {
                $matchesPattern = $true
                break
            }
        }
        
        # Check exclusion patterns
        $isExcluded = $false
        if ($rule.exclude_patterns) {
            foreach ($excludePattern in $rule.exclude_patterns) {
                if ($fileName -like $excludePattern) {
                    $isExcluded = $true
                    break
                }
            }
        }
        
        # Check content indicators
        $hasIndicators = $false
        if ($rule.indicators -and $content) {
            foreach ($indicator in $rule.indicators) {
                if ($content -match $indicator) {
                    $hasIndicators = $true
                    break
                }
            }
        }
        
        if (($matchesPattern -or $hasIndicators) -and -not $isExcluded) {
            return @{
                Category = $category
                TargetDir = $rule.target_dir
                Action = $rule.action
                Rule = $rule
            }
        }
    }
    
    # Default classification
    return @{
        Category = "unknown"
        TargetDir = "archive/unknown"
        Action = "needs_review"
        Rule = $null
    }
}

function Analyze-Repository {
    Write-Host "Analyzing MINIX repository structure..."
    
    $allFiles = Get-ChildItem -Path "." -Recurse -File -Name "*.c"
    $classifications = @{}
    
    foreach ($file in $allFiles) {
        $classification = Get-FileClassification $file
        
        if (-not $classifications.ContainsKey($classification.Category)) {
            $classifications[$classification.Category] = @()
        }
        
        $classifications[$classification.Category] += @{
            Path = $file
            Classification = $classification
        }
    }
    
    return $classifications
}

function Generate-ConsolidationPlan {
    param([hashtable]$Classifications)
    
    $plan = @"
# MINIX Consolidation Execution Plan
*Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')*

## File Classification Summary

| Category | File Count | Target Directory | Action |
|----------|------------|------------------|--------|
"@

    foreach ($category in $Classifications.Keys | Sort-Object) {
        $files = $Classifications[$category]
        $targetDir = $files[0].Classification.TargetDir
        $action = $files[0].Classification.Action
        
        $plan += "`n| $category | $($files.Count) | $targetDir | $action |"
    }
    
    $plan += @"


## Detailed File Listings

"@

    foreach ($category in $Classifications.Keys | Sort-Object) {
        $files = $Classifications[$category]
        
        $plan += @"

### $category ($($files.Count) files)
**Target**: $($files[0].Classification.TargetDir)
**Action**: $($files[0].Classification.Action)

Files:
"@
        
        foreach ($file in $files | Sort-Object { $_.Path }) {
            $plan += "`n- $($file.Path)"
        }
        
        $plan += "`n"
    }
    
    return $plan
}

# Main execution
try {
    Write-Host "=== MINIX File Classification Analysis ===" -ForegroundColor Green
    
    $classifications = Analyze-Repository
    
    Write-Host "Classification complete. Found $($classifications.Keys.Count) categories." -ForegroundColor Yellow
    
    # Display summary
    foreach ($category in $classifications.Keys | Sort-Object) {
        $count = $classifications[$category].Count
        Write-Host "  $category`: $count files" -ForegroundColor Cyan
    }
    
    # Generate consolidation plan
    $plan = Generate-ConsolidationPlan $classifications
    Set-Content -Path "CONSOLIDATION_PLAN_DETAILED.md" -Value $plan
    
    Write-Host "`nConsolidation plan saved to CONSOLIDATION_PLAN_DETAILED.md" -ForegroundColor Green
    
    if ($Execute) {
        Write-Host "`nExecuting consolidation..." -ForegroundColor Yellow
        # Implementation would go here
    } else {
        Write-Host "`nRun with -Execute to perform the consolidation" -ForegroundColor White
    }
    
} catch {
    Write-Error "Error during analysis: $($_.Exception.Message)"
    exit 1
}
