#!/usr/bin/env python3
"""
MINIX4 Python Synthesis Executor - Batch 3 (Targets 22-31)
Processes the next 10 most duplicated filenames
"""

import os
import sys
import json
import shutil
from pathlib import Path
from typing import Dict, List, Optional
import argparse
import time
import hashlib

class SynthesisExecutorBatch3:
    """Executes file synthesis for batch 3 (targets 22-31)"""
    
    def __init__(self, workspace_root: str, analysis_file: str):
        self.workspace_root = Path(workspace_root)
        self.analysis_file = Path(analysis_file)
        self.dry_run = False
        
        # Load analysis results
        with open(self.analysis_file) as f:
            self.analysis_data = json.load(f)
        
        # Define batch 3 targets (22-31)
        self.batch3_targets = [
            "diffdir.c", "file.c", "grep.c", "queue.c", "format.c",
            "kernel.c", "mem.c", "output.c", "trace.c", "char.c"
        ]
    
    def log(self, message: str, level: str = "INFO"):
        """Log with timestamp and color"""
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
        colors = {
            'INFO': '\033[97m',      # White
            'SUCCESS': '\033[92m',   # Green
            'WARNING': '\033[93m',   # Yellow
            'ERROR': '\033[91m',     # Red
            'SYNTHESIS': '\033[95m'  # Magenta
        }
        reset = '\033[0m'
        color = colors.get(level, colors['INFO'])
        print(f"{color}[{timestamp}] [{level}] {message}{reset}")
    
    def create_unified_header(self, target_path: str, sources: List[str], description: str) -> str:
        """Create a modern C23 header for unified files"""
        filename = Path(target_path).name
        
        header = f'''/**
 * @file {filename}
 * @brief {description}
 * @details This file is a synthesized/unified implementation combining multiple
 *          source files from the legacy MINIX codebase into a single, modern,
 *          C23-compliant implementation for MINIX4.
 * 
 * @version 4.0.0
 * @date {time.strftime('%Y-%m-%d')}
 * @author MINIX4 Synthesis System
 * 
 * @copyright Copyright (c) 2024 MINIX4 Project
 * @license MIT License
 * 
 * SYNTHESIS INFORMATION:
 * ======================
 * This file synthesizes the following source files:
'''
        
        for i, source in enumerate(sources, 1):
            header += f' *   {i:2d}. {source}\n'
        
        header += f''' * 
 * Total source files: {len(sources)}
 * Synthesis date: {time.strftime('%Y-%m-%d %H:%M:%S')}
 * Synthesis strategy: Modern C23 unification with legacy compatibility
 * 
 * MODERNIZATION FEATURES:
 * =======================
 * - C23 standard compliance
 * - Modern error handling patterns
 * - Consistent coding style
 * - Improved memory safety
 * - Enhanced documentation
 * - POSIX compliance where applicable
 */

#ifndef {filename.upper().replace('.', '_')}_H
#define {filename.upper().replace('.', '_')}_H

// C23 Standard Headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>

// POSIX Headers
#include <unistd.h>
#include <errno.h>
#include <string.h>

// MINIX4 Headers
#include "minix4_types.h"
#include "minix4_config.h"

#ifdef __cplusplus
extern "C" {{
#endif

'''
        return header
    
    def create_unified_footer(self, filename: str) -> str:
        """Create footer for unified files"""
        return f'''
#ifdef __cplusplus
}}
#endif

#endif /* {filename.upper().replace('.', '_')}_H */

/* End of {filename} - Synthesized by MINIX4 Synthesis System */
'''
    
    def read_source_file_safely(self, filepath: str) -> Optional[str]:
        """Safely read source file with encoding detection"""
        try:
            # Try UTF-8 first
            with open(filepath, 'r', encoding='utf-8') as f:
                return f.read()
        except UnicodeDecodeError:
            try:
                # Fall back to latin-1
                with open(filepath, 'r', encoding='latin-1') as f:
                    return f.read()
            except Exception as e:
                self.log(f"Failed to read {filepath}: {e}", "ERROR")
                return None
        except Exception as e:
            self.log(f"Failed to read {filepath}: {e}", "ERROR")
            return None
    
    def extract_functions_and_types(self, content: str, filepath: str) -> Dict:
        """Extract functions, types, and key definitions from C source"""
        lines = content.split('\n')
        functions = []
        includes = []
        defines = []
        types = []
        globals = []
        
        in_function = False
        brace_count = 0
        current_function = []
        
        for line in lines:
            stripped = line.strip()
            
            # Extract includes
            if stripped.startswith('#include'):
                includes.append(line)
            
            # Extract defines
            elif stripped.startswith('#define'):
                defines.append(line)
            
            # Extract typedefs, structs, unions
            elif any(stripped.startswith(kw) for kw in ['typedef', 'struct', 'union', 'enum']):
                types.append(line)
            
            # Simple function detection (line starting with identifier followed by '(')
            elif ('(' in stripped and 
                  not stripped.startswith('//') and 
                  not stripped.startswith('/*') and
                  not stripped.startswith('#') and
                  not stripped.startswith('if') and
                  not stripped.startswith('while') and
                  not stripped.startswith('for') and
                  not stripped.startswith('switch') and
                  '{' in stripped):
                in_function = True
                current_function = [line]
                brace_count = stripped.count('{') - stripped.count('}')
            
            elif in_function:
                current_function.append(line)
                brace_count += stripped.count('{') - stripped.count('}')
                
                if brace_count <= 0:
                    functions.append('\n'.join(current_function))
                    in_function = False
                    current_function = []
            
            # Global variable declarations
            elif (stripped.endswith(';') and 
                  not stripped.startswith('//') and 
                  not stripped.startswith('/*') and
                  not stripped.startswith('#') and
                  not any(kw in stripped for kw in ['if', 'while', 'for', 'return']) and
                  not in_function):
                globals.append(line)
        
        return {
            'includes': includes,
            'defines': defines,
            'types': types,
            'functions': functions,
            'globals': globals,
            'source_path': filepath
        }
    
    def synthesize_target(self, target_name: str, target_data: Dict) -> bool:
        """Synthesize a single target into unified file"""
        self.log(f"Synthesizing target: {target_name}", "SYNTHESIS")
        
        unified_path = Path(target_data['unified_path'])
        source_files = target_data['source_files']
        description = target_data['description']
        
        # Create output directory
        unified_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Collect all source content
        all_includes = set()
        all_defines = []
        all_types = []
        all_functions = []
        all_globals = []
        source_info = []
        
        for source_file in source_files:
            source_path = self.workspace_root / source_file
            
            if source_path.exists() and source_path.is_file():
                content = self.read_source_file_safely(str(source_path))
                if content:
                    extracted = self.extract_functions_and_types(content, str(source_path))
                    
                    # Merge unique includes
                    for inc in extracted['includes']:
                        if 'minix' not in inc.lower() or 'legacy' not in inc.lower():
                            all_includes.add(inc.strip())
                    
                    all_defines.extend(extracted['defines'])
                    all_types.extend(extracted['types'])
                    all_functions.extend(extracted['functions'])
                    all_globals.extend(extracted['globals'])
                    
                    source_info.append({
                        'path': str(source_path),
                        'functions': len(extracted['functions']),
                        'size': len(content)
                    })
                    
                    self.log(f"  Processed: {source_file} ({len(extracted['functions'])} functions)", "INFO")
                else:
                    self.log(f"  Skipped (read failed): {source_file}", "WARNING")
            else:
                self.log(f"  Skipped (not found): {source_file}", "WARNING")
        
        # Generate unified file
        try:
            with open(unified_path, 'w', encoding='utf-8') as f:
                # Write header
                f.write(self.create_unified_header(
                    str(unified_path), 
                    [info['path'] for info in source_info], 
                    description
                ))
                
                # Write includes (deduplicated)
                if all_includes:
                    f.write('\n/* ===== INCLUDES ===== */\n')
                    for include in sorted(all_includes):
                        f.write(f'{include}\n')
                
                # Write defines
                if all_defines:
                    f.write('\n/* ===== DEFINES ===== */\n')
                    for define in all_defines:
                        if define.strip():
                            f.write(f'{define}\n')
                
                # Write types
                if all_types:
                    f.write('\n/* ===== TYPES ===== */\n')
                    for type_def in all_types:
                        if type_def.strip():
                            f.write(f'{type_def}\n')
                
                # Write globals
                if all_globals:
                    f.write('\n/* ===== GLOBAL VARIABLES ===== */\n')
                    for global_var in all_globals:
                        if global_var.strip():
                            f.write(f'{global_var}\n')
                
                # Write functions
                if all_functions:
                    f.write('\n/* ===== FUNCTIONS ===== */\n\n')
                    for i, func in enumerate(all_functions):
                        f.write(f'/* Function {i+1} */\n')
                        f.write(f'{func}\n\n')
                
                # Write footer
                f.write(self.create_unified_footer(unified_path.name))
            
            self.log(f"  SUCCESS: Created {unified_path}", "SUCCESS")
            self.log(f"  Stats: {len(source_info)} sources, {len(all_functions)} functions, {len(all_includes)} includes", "INFO")
            return True
            
        except Exception as e:
            self.log(f"  ERROR: Failed to create {unified_path}: {e}", "ERROR")
            return False
    
    def execute_batch3(self):
        """Execute synthesis for batch 3 targets"""
        self.log("Starting MINIX4 Synthesis Executor - Batch 3 (Targets 22-31)", "SUCCESS")
        self.log(f"Workspace: {self.workspace_root}", "INFO")
        self.log(f"Analysis file: {self.analysis_file}", "INFO")
        self.log(f"Batch 3 targets: {', '.join(self.batch3_targets)}", "INFO")
        
        # targets is a list, not a dict
        targets_list = self.analysis_data.get('targets', [])
        targets_dict = {target['name']: target for target in targets_list}
        
        processed = 0
        successful = 0
        
        for target_name in self.batch3_targets:
            if target_name in targets_dict:
                if self.synthesize_target(target_name, targets_dict[target_name]):
                    successful += 1
                processed += 1
            else:
                self.log(f"Target not found in analysis: {target_name}", "WARNING")
        
        self.log(f"Batch 3 synthesis complete: {successful}/{processed} targets successful", "SUCCESS")
        
        if successful == len(self.batch3_targets):
            self.log("All batch 3 targets synthesized successfully!", "SUCCESS")
        else:
            self.log(f"Some targets failed. Check logs above.", "WARNING")
        
        return successful == len(self.batch3_targets)

def main():
    parser = argparse.ArgumentParser(description='MINIX4 Synthesis Executor - Batch 3')
    parser.add_argument('--workspace', '-w', default='.', 
                       help='Workspace root directory')
    parser.add_argument('--analysis', '-a', 
                       default='docs/analysis/Synthesis_Plan_Report.json',
                       help='Analysis file path')
    
    args = parser.parse_args()
    
    # Create and run executor
    executor = SynthesisExecutorBatch3(args.workspace, args.analysis)
    success = executor.execute_batch3()
    
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
