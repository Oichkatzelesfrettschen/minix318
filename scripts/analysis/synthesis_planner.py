#!/usr/bin/env python3
"""
MINIX4 Synthesis-Focused Duplicate Analysis
Creates synthesis targets and unified files for MINIX4 consolidation
"""

import os
import sys
import json
import hashlib
import subprocess
from pathlib import Path
from collections import defaultdict
from dataclasses import dataclass, asdict
from typing import Dict, List, Set, Tuple, Optional
import argparse
import time

@dataclass
class SynthesisTarget:
    """A target for file synthesis/unification"""
    name: str                    # Original filename (e.g., "main.c")
    unified_path: str           # Target unified file path
    description: str            # Purpose description
    priority: int               # Synthesis priority (lower = higher priority)
    source_files: List[str]     # List of source file paths
    total_functions: int        # Total unique functions across sources
    common_functions: List[str] # Functions present in multiple sources
    unique_functions: List[str] # Functions only in one source
    synthesis_strategy: str     # How to synthesize (merge, replace, etc.)
    estimated_effort: str       # Low/Medium/High effort estimate

class SynthesisPlanGenerator:
    """Generates synthesis plans for MINIX4 unification"""
    
    def __init__(self, workspace_root: str):
        self.workspace_root = Path(workspace_root)
        self.c_extensions = {'.c', '.h'}
        
    def extract_function_names(self, file_path: Path) -> List[str]:
        """Extract function names from a C file"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception:
            return []
        
        import re
        # Match function definitions (simplified)
        func_pattern = r'^[a-zA-Z_][a-zA-Z0-9_*\s]+\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*\{'
        functions = []
        
        for line in content.split('\n'):
            line = line.strip()
            if line and not line.startswith('//') and not line.startswith('/*'):
                match = re.match(func_pattern, line)
                if match:
                    func_name = match.group(1)
                    if func_name not in ['if', 'while', 'for', 'switch']:  # Skip keywords
                        functions.append(func_name)
        
        return functions
    
    def get_file_hash(self, file_path: Path) -> str:
        """Get MD5 hash of file content"""
        try:
            with open(file_path, 'rb') as f:
                return hashlib.md5(f.read()).hexdigest()
        except Exception:
            return ""
    
    def analyze_duplicates(self) -> Dict[str, List[Path]]:
        """Find all duplicate filenames in the workspace"""
        print("Scanning for duplicate filenames...")
        
        files_by_name = defaultdict(list)
        
        # Scan all C files
        for ext in self.c_extensions:
            for file_path in self.workspace_root.rglob(f"*{ext}"):
                if file_path.is_file():
                    filename = file_path.name
                    files_by_name[filename].append(file_path)
        
        # Filter to only duplicates
        duplicates = {name: files for name, files in files_by_name.items() 
                     if len(files) > 1}
        
        print(f"Found {len(duplicates)} duplicate filename groups")
        return duplicates
    
    def create_synthesis_targets(self, duplicates: Dict[str, List[Path]]) -> List[SynthesisTarget]:
        """Create synthesis targets for duplicate files"""
        
        # Priority mappings for different file types
        priority_map = {
            'main.c': (1, 'minix4/exokernel/main_unified.c', 'Unified exokernel entry point'),
            'proc.c': (2, 'minix4/exokernel/process/proc_unified.c', 'Unified process management'),
            'memory.c': (3, 'minix4/exokernel/memory/memory_unified.c', 'Unified memory management'),
            'alloc.c': (3, 'minix4/exokernel/memory/alloc_unified.c', 'Unified memory allocation'),
            'signal.c': (4, 'minix4/exokernel/signals/signal_unified.c', 'Unified signal handling'),
            'sig.c': (4, 'minix4/exokernel/signals/sig_unified.c', 'Unified signal operations'),
            'ipc.c': (5, 'minix4/exokernel/ipc/ipc_unified.c', 'Unified IPC implementation'),
            'printf.c': (6, 'minix4/libos/libc/printf_unified.c', 'Unified printf implementation'),
            'string.c': (7, 'minix4/libos/libc/string_unified.c', 'Unified string operations'),
            'malloc.c': (8, 'minix4/libos/libc/malloc_unified.c', 'Unified malloc implementation'),
            'syscall.c': (9, 'minix4/exokernel/syscalls/syscall_unified.c', 'Unified system calls'),
            'interrupt.c': (10, 'minix4/exokernel/arch/interrupt_unified.c', 'Unified interrupt handling'),
        }
        
        synthesis_targets = []
        
        for filename, file_list in sorted(duplicates.items(), 
                                        key=lambda x: priority_map.get(x[0], (999, '', ''))[0]):
            
            # Skip header files for now (focus on .c files)
            if filename.endswith('.h'):
                continue
            
            # Get priority info
            priority, unified_path, description = priority_map.get(
                filename, 
                (100, f'minix4/libos/utilities/{filename.replace(".c", "_unified.c")}', 
                 f'Unified {filename.replace(".c", "")} implementation')
            )
            
            # Analyze functions in each file
            all_functions = set()
            file_functions = {}
            
            for file_path in file_list:
                functions = self.extract_function_names(file_path)
                file_functions[str(file_path)] = functions
                all_functions.update(functions)
            
            # Find common vs unique functions
            common_functions = []
            unique_functions = []
            
            if len(file_list) > 1:
                for func in all_functions:
                    count = sum(1 for funcs in file_functions.values() if func in funcs)
                    if count > 1:
                        common_functions.append(func)
                    else:
                        unique_functions.append(func)
            
            # Determine synthesis strategy
            if len(set(self.get_file_hash(f) for f in file_list)) == 1:
                strategy = "identical_merge"  # All files are identical
                effort = "Low"
            elif len(common_functions) > len(unique_functions):
                strategy = "function_merge"   # Mostly common functions
                effort = "Medium" 
            else:
                strategy = "careful_synthesis"  # Significant differences
                effort = "High"
            
            target = SynthesisTarget(
                name=filename,
                unified_path=unified_path,
                description=description,
                priority=priority,
                source_files=[str(f.relative_to(self.workspace_root)) for f in file_list],
                total_functions=len(all_functions),
                common_functions=common_functions,
                unique_functions=unique_functions,
                synthesis_strategy=strategy,
                estimated_effort=effort
            )
            
            synthesis_targets.append(target)
        
        return sorted(synthesis_targets, key=lambda x: x.priority)
    
    def generate_synthesis_script(self, targets: List[SynthesisTarget]) -> str:
        """Generate a PowerShell script to perform the actual synthesis"""
        
        script_lines = [
            "# MINIX4 File Synthesis Script",
            "# Auto-generated synthesis plan for duplicate file unification",
            "",
            "param(",
            "    [string]$WorkspaceRoot = 'c:\\Users\\ericj\\minix318',",
            "    [switch]$DryRun = $false,",
            "    [switch]$Force = $false",
            ")",
            "",
            "$ErrorActionPreference = 'Stop'",
            "",
            "function Write-SynthesisLog {",
            "    param($Message, $Level = 'INFO')",
            "    $timestamp = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'",
            "    $color = switch ($Level) {",
            "        'ERROR' { 'Red' }",
            "        'WARN' { 'Yellow' }",
            "        'SUCCESS' { 'Green' }",
            "        'SYNTHESIS' { 'Magenta' }",
            "        default { 'White' }",
            "    }",
            "    Write-Host \"[$timestamp] [$Level] $Message\" -ForegroundColor $color",
            "}",
            "",
            "function Synthesize-IdenticalFiles {",
            "    param($SourceFiles, $TargetPath)",
            "    # For identical files, just copy the first one",
            "    $firstFile = $SourceFiles[0]",
            "    $sourceContent = Get-Content (Join-Path $WorkspaceRoot $firstFile) -Raw",
            "    return $sourceContent",
            "}",
            "",
            "function Synthesize-Functions {",
            "    param($SourceFiles, $TargetPath, $CommonFunctions, $UniqueFunctions)",
            "    # Merge functions from multiple files",
            "    $allContent = @()",
            "    ",
            "    # Add unified header",
            "    $allContent += Create-UnifiedHeader $TargetPath",
            "    ",
            "    # Process each source file",
            "    foreach ($sourceFile in $SourceFiles) {",
            "        $content = Get-Content (Join-Path $WorkspaceRoot $sourceFile) -Raw",
            "        # Extract and add functions (simplified)",
            "        $allContent += \"/* From: $sourceFile */\"",
            "        $allContent += $content",
            "        $allContent += \"\"",
            "    }",
            "    ",
            "    return ($allContent -join \"`n\")",
            "}",
            "",
            "function Create-UnifiedHeader {",
            "    param($TargetPath)",
            "    $filename = Split-Path $TargetPath -Leaf",
            "    return @\"",
            "/**",
            " * @file $filename",
            " * @brief Unified implementation synthesized from multiple legacy sources",
            " * @details This file was automatically generated by the MINIX4 synthesis",
            " *          process to consolidate duplicate implementations into a single,",
            " *          modern, C23-compliant file.",
            " *",
            " * @version 4.0.0",
            " * @date $(Get-Date -Format 'yyyy-MM-dd')",
            " * @author MINIX4 Synthesis Engine",
            " * @copyright Copyright (c) 2025 MINIX Project",
            " */",
            "",
            "#pragma once",
            "",
            "#define _POSIX_C_SOURCE 202311L",
            "",
            "/* Standard C23 headers */",
            "#include <stdint.h>",
            "#include <stddef.h>",
            "#include <stdbool.h>",
            "#include <stdatomic.h>",
            "",
            "/* MINIX4 system headers */",
            "#include <minix4/kernel_types.h>",
            "#include <minix4/config.h>",
            "",
            "\"@",
            "}",
            "",
            "Write-SynthesisLog 'Starting MINIX4 file synthesis process' 'SYNTHESIS'",
            "Write-SynthesisLog \"Workspace: $WorkspaceRoot\" 'INFO'",
            "Write-SynthesisLog \"Dry Run: $DryRun\" 'INFO'",
            "",
        ]
          # Add synthesis commands for each target
        for i, target in enumerate(targets[:10], 1):  # Limit to top 10 for now
            common_funcs = "', '".join(target.common_functions[:5])  # Limit to avoid long lines
            unique_funcs = "', '".join(target.unique_functions[:5])
            
            script_lines.extend([
                f"# === SYNTHESIS TARGET {i}: {target.name} ===",
                f"Write-SynthesisLog 'Synthesizing: {target.name}' 'SYNTHESIS'",
                f"$target{i}Sources = @(",
                *[f"    '{source}'," for source in target.source_files],
                ")",
                f"$target{i}Path = '{target.unified_path}'",
                f"$target{i}Strategy = '{target.synthesis_strategy}'",
                "",
                f"if ($target{i}Strategy -eq 'identical_merge') {{",
                f"    $content{i} = Synthesize-IdenticalFiles $target{i}Sources $target{i}Path",
                f"}} else {{",
                f"    $content{i} = Synthesize-Functions $target{i}Sources $target{i}Path @('{common_funcs}') @('{unique_funcs}')",
                f"}}",
                "",
                f"# Create target directory",
                f"$targetDir{i} = Split-Path (Join-Path $WorkspaceRoot $target{i}Path) -Parent",
                f"if (-not (Test-Path $targetDir{i})) {{",
                f"    if (-not $DryRun) {{ New-Item -ItemType Directory -Path $targetDir{i} -Force | Out-Null }}",
                f"    Write-SynthesisLog \"Created directory: $targetDir{i}\" 'SUCCESS'",
                f"}}",
                "",
                f"# Write synthesized file",
                f"$targetFullPath{i} = Join-Path $WorkspaceRoot $target{i}Path",
                f"if (-not $DryRun) {{",
                f"    $content{i} | Out-File -FilePath $targetFullPath{i} -Encoding UTF8",
                f"    Write-SynthesisLog \"Created unified file: $target{i}Path\" 'SUCCESS'",
                f"}} else {{",
                f"    Write-SynthesisLog \"[DRY RUN] Would create: $target{i}Path\" 'INFO'",
                f"}}",
                "",
                f"Write-SynthesisLog \"Synthesized {target.name}: {target.synthesis_strategy} strategy, {target.estimated_effort} effort\" 'INFO'",
                "",
            ])
        
        script_lines.extend([
            "Write-SynthesisLog 'MINIX4 synthesis process complete!' 'SUCCESS'",
            "Write-SynthesisLog 'Next steps:' 'INFO'",
            "Write-SynthesisLog '1. Review generated unified files' 'INFO'",
            "Write-SynthesisLog '2. Update build system references' 'INFO'",
            "Write-SynthesisLog '3. Run comprehensive tests' 'INFO'",
            "Write-SynthesisLog '4. Commit synthesized changes' 'INFO'"
        ])
        
        return "\n".join(script_lines)
    
    def generate_synthesis_report(self, targets: List[SynthesisTarget], output_file: str):
        """Generate comprehensive synthesis plan report"""
        
        report_data = {
            'metadata': {
                'generated': time.strftime('%Y-%m-%d %H:%M:%S'),
                'workspace': str(self.workspace_root),
                'total_targets': len(targets),
                'high_priority_targets': len([t for t in targets if t.priority <= 10])
            },
            'summary': {
                'by_priority': sorted([(t.name, t.priority, t.estimated_effort) for t in targets], 
                                    key=lambda x: x[1]),
                'by_effort': {
                    'low': len([t for t in targets if t.estimated_effort == 'Low']),
                    'medium': len([t for t in targets if t.estimated_effort == 'Medium']),
                    'high': len([t for t in targets if t.estimated_effort == 'High'])
                },
                'by_strategy': {
                    'identical_merge': len([t for t in targets if t.synthesis_strategy == 'identical_merge']),
                    'function_merge': len([t for t in targets if t.synthesis_strategy == 'function_merge']),
                    'careful_synthesis': len([t for t in targets if t.synthesis_strategy == 'careful_synthesis'])
                }
            },
            'targets': [asdict(t) for t in targets]
        }
        
        # Write JSON report
        json_file = output_file.replace('.md', '.json')
        with open(json_file, 'w') as f:
            json.dump(report_data, f, indent=2)
        
        # Write Markdown report
        with open(output_file, 'w') as f:
            f.write("# MINIX4 Synthesis Plan Report\n\n")
            f.write(f"Generated: {report_data['metadata']['generated']}\n\n")
            
            f.write("## Executive Summary\n\n")
            f.write(f"This synthesis plan consolidates **{len(targets)} duplicate filename groups** ")
            f.write("into unified, modern MINIX4 implementations. The plan prioritizes critical ")
            f.write("system components and provides detailed strategies for each synthesis target.\n\n")
            
            f.write("### Synthesis Statistics\n\n")
            f.write(f"- **Total synthesis targets**: {len(targets)}\n")
            f.write(f"- **High priority targets**: {report_data['metadata']['high_priority_targets']}\n")
            f.write(f"- **Low effort syntheseses**: {report_data['summary']['by_effort']['low']}\n")
            f.write(f"- **Medium effort syntheseses**: {report_data['summary']['by_effort']['medium']}\n")
            f.write(f"- **High effort syntheseses**: {report_data['summary']['by_effort']['high']}\n\n")
            
            f.write("### Synthesis Strategies\n\n")
            f.write(f"- **Identical merge**: {report_data['summary']['by_strategy']['identical_merge']} (files are identical, simple copy)\n")
            f.write(f"- **Function merge**: {report_data['summary']['by_strategy']['function_merge']} (mostly common functions, moderate effort)\n")
            f.write(f"- **Careful synthesis**: {report_data['summary']['by_strategy']['careful_synthesis']} (significant differences, careful analysis needed)\n\n")
            
            f.write("## High Priority Synthesis Targets\n\n")
            f.write("The following targets should be synthesized first:\n\n")
            
            for i, target in enumerate(targets[:15], 1):
                f.write(f"### {i}. {target.name}\n")
                f.write(f"- **Priority**: {target.priority} (1=highest)\n")
                f.write(f"- **Target path**: `{target.unified_path}`\n")
                f.write(f"- **Description**: {target.description}\n")
                f.write(f"- **Strategy**: {target.synthesis_strategy}\n")
                f.write(f"- **Estimated effort**: {target.estimated_effort}\n")
                f.write(f"- **Source files**: {len(target.source_files)}\n")
                f.write(f"- **Total functions**: {target.total_functions}\n")
                f.write(f"- **Common functions**: {len(target.common_functions)}\n")
                f.write(f"- **Unique functions**: {len(target.unique_functions)}\n")
                
                f.write("- **Sources**:\n")
                for source in target.source_files:
                    f.write(f"  - `{source}`\n")
                
                if target.common_functions:
                    f.write("- **Common functions**: ")
                    f.write(", ".join(f"`{func}`" for func in target.common_functions[:10]))
                    if len(target.common_functions) > 10:
                        f.write(f" ... and {len(target.common_functions) - 10} more")
                    f.write("\n")
                
                f.write("\n")
            
            f.write("## Implementation Steps\n\n")
            f.write("1. **Run synthesis script**: Execute the generated PowerShell script\n")
            f.write("2. **Review unified files**: Check synthesized implementations for correctness\n")
            f.write("3. **Update build system**: Modify Makefile/meson.build to use unified files\n")
            f.write("4. **Test compilation**: Ensure unified files compile without errors\n")
            f.write("5. **Run tests**: Execute test suite to verify functionality\n")
            f.write("6. **Commit changes**: Git commit the synthesized unified files\n\n")
            
            f.write("## Next Phase\n\n")
            f.write("After synthesis completion:\n")
            f.write("- Modernize synthesized code to C23 standards\n")
            f.write("- Add comprehensive error handling\n")
            f.write("- Implement capability-based security\n")
            f.write("- Create unit tests for all unified functions\n")
            f.write("- Optimize performance and reduce complexity\n")
        
        print(f"Synthesis plan saved:")
        print(f"  - Report: {output_file}")
        print(f"  - Data: {json_file}")

def main():
    parser = argparse.ArgumentParser(description='Generate MINIX4 synthesis plan')
    parser.add_argument('--workspace', default='.', help='Workspace root directory')
    parser.add_argument('--output', default='docs/analysis/Synthesis_Plan_Report.md', 
                       help='Output report file')
    parser.add_argument('--script', default='scripts/automation/Execute_Synthesis.ps1',
                       help='Output PowerShell synthesis script')
    
    args = parser.parse_args()
    
    generator = SynthesisPlanGenerator(args.workspace)
    
    # Analyze duplicates
    duplicates = generator.analyze_duplicates()
    
    # Create synthesis targets
    targets = generator.create_synthesis_targets(duplicates)
    
    # Generate reports
    generator.generate_synthesis_report(targets, args.output)
    
    # Generate synthesis script
    script_content = generator.generate_synthesis_script(targets)
    script_path = Path(args.script)
    script_path.parent.mkdir(parents=True, exist_ok=True)
    
    with open(script_path, 'w') as f:
        f.write(script_content)
    
    print(f"Synthesis script saved: {script_path}")
    
    print(f"\n{'='*60}")
    print("SYNTHESIS PLAN GENERATED")
    print(f"{'='*60}")
    print(f"Found {len(duplicates)} duplicate filename groups")
    print(f"Created {len(targets)} synthesis targets")
    print(f"High priority targets: {len([t for t in targets if t.priority <= 10])}")
    print(f"Low effort targets: {len([t for t in targets if t.estimated_effort == 'Low'])}")
    
    print("\nTop 5 synthesis targets:")
    for i, target in enumerate(targets[:5], 1):
        print(f"  {i}. {target.name} -> {target.unified_path} ({target.estimated_effort} effort)")
    
    print(f"\nRun synthesis with: powershell -File {script_path}")

if __name__ == "__main__":
    main()
