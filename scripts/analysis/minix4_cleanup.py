#!/usr/bin/env python3
"""
MINIX4 Cleanup and Reorganization Script
Based on comprehensive duplicate analysis results

This script will:
1. Remove exact duplicate files (keeping one copy)
2. Consolidate similar files
3. Reorganize files according to MINIX4 5-layer architecture
4. Create a cleanup report
"""

import os
import sys
import shutil
import json
from pathlib import Path
from comprehensive_duplicate_scanner import FileAnalyzer

class MinixCleanup:
    def __init__(self, root_path: str, dry_run: bool = True):
        self.root_path = Path(root_path)
        self.dry_run = dry_run
        self.actions_log = []
        
        # Priority order for keeping files (prefer these locations)
        self.preferred_locations = [
            'minix4/',
            'archive/',
            'userspace/',
            'minix/',
            'docs/',
            'scripts/',
            'tools/'
        ]
        
        # Files to definitely remove (obvious duplicates)
        self.removal_patterns = [
            r'.*copy\.c$',
            r'.*copy\.h$', 
            r'.*_backup\.',
            r'.*\.bak$',
            r'.*_old\.',
            r'.*~$',
            r'.*\.orig$'
        ]
        
    def log_action(self, action_type: str, source: str, target: str = "", reason: str = ""):
        """Log cleanup actions"""
        self.actions_log.append({
            'type': action_type,
            'source': source,
            'target': target,
            'reason': reason,
            'dry_run': self.dry_run
        })
        
        if self.dry_run:
            print(f"[DRY RUN] {action_type}: {source} -> {target} ({reason})")
        else:
            print(f"{action_type}: {source} -> {target} ({reason})")
    
    def choose_file_to_keep(self, files: list) -> Path:
        """Choose which file to keep based on location preferences"""
        # Convert to Path objects
        file_paths = [Path(f) for f in files]
        
        # Prefer files in better locations
        for preferred in self.preferred_locations:
            for file_path in file_paths:
                if preferred in str(file_path):
                    return file_path
        
        # If no preference matches, keep the first one
        return file_paths[0]
    
    def clean_exact_duplicates(self, analyzer: FileAnalyzer):
        """Remove exact duplicate files"""
        print("\n🧹 Cleaning exact duplicates...")
        
        exact_duplicates = analyzer.find_exact_duplicates()
        removed_count = 0
        
        for group in exact_duplicates:
            if len(group) < 2:
                continue
                
            # Choose which file to keep
            keeper = self.choose_file_to_keep(group)
            
            # Remove the others
            for file_path in group:
                if file_path != keeper:
                    self.log_action(
                        "REMOVE_DUPLICATE", 
                        str(file_path),
                        reason=f"Duplicate of {keeper}"
                    )
                    
                    if not self.dry_run:
                        try:
                            file_path.unlink()
                            removed_count += 1
                        except OSError as e:
                            print(f"Error removing {file_path}: {e}")
        
        print(f"✅ Removed {removed_count} duplicate files")
        return removed_count
    
    def reorganize_architecture_violations(self, analyzer: FileAnalyzer):
        """Reorganize files according to MINIX4 architecture"""
        print("\n🏗️ Reorganizing architectural violations...")
        
        moved_count = 0
        
        for violation in analyzer.architecture_violations:
            if violation['type'] == 'legacy_location':
                old_path = violation['file']
                suggestion = violation.get('suggestion', 'archive/')
                
                # Determine new location
                if suggestion.startswith('minix4/'):
                    new_path = self.root_path / suggestion / old_path.name
                elif suggestion == 'userspace/ or archive/':
                    # Move to archive by default for safety
                    new_path = self.root_path / 'archive' / 'minix_legacy' / old_path.name
                else:
                    new_path = self.root_path / 'archive' / 'reorganized' / old_path.name
                
                # Ensure target directory exists
                new_path.parent.mkdir(parents=True, exist_ok=True)
                
                # Only move if target doesn't exist
                if not new_path.exists():
                    self.log_action(
                        "MOVE_FILE",
                        str(old_path),
                        str(new_path),
                        f"Architecture violation: {violation['issue']}"
                    )
                    
                    if not self.dry_run:
                        try:
                            shutil.move(str(old_path), str(new_path))
                            moved_count += 1
                        except OSError as e:
                            print(f"Error moving {old_path}: {e}")
        
        print(f"✅ Moved {moved_count} files for architectural compliance")
        return moved_count
    
    def clean_suspicious_files(self, analyzer: FileAnalyzer):
        """Clean up obviously suspicious/temporary files"""
        print("\n🗑️ Cleaning suspicious files...")
        
        import re
        removed_count = 0
        
        for item in analyzer.misplaced_files:
            file_path = item['file']
            filename = file_path.name.lower()
            
            # Check if it's an obvious temporary/backup file
            should_remove = False
            for pattern in self.removal_patterns:
                if re.match(pattern, filename):
                    should_remove = True
                    break
            
            # Also remove obvious versioned copies
            if ' copy' in filename or filename.endswith(' 3.c') or filename.endswith(' 4.c'):
                should_remove = True
            
            if should_remove:
                self.log_action(
                    "REMOVE_SUSPICIOUS",
                    str(file_path),
                    reason=f"Suspicious file: {item['issue']}"
                )
                
                if not self.dry_run:
                    try:
                        file_path.unlink()
                        removed_count += 1
                    except OSError as e:
                        print(f"Error removing {file_path}: {e}")
        
        print(f"✅ Removed {removed_count} suspicious files")
        return removed_count
    
    def create_minix4_structure(self):
        """Create proper MINIX4 directory structure"""
        print("\n🏗️ Creating MINIX4 directory structure...")
        
        minix4_structure = {
            'minix4': {
                'exokernel': ['arch/x86_64', 'arch/i386', 'arch/arm32', 'arch/arm64', 
                              'arch/common', 'core/capability', 'core/memory', 'core/security',
                              'core/init', 'drivers/storage', 'drivers/network', 'drivers/input',
                              'drivers/display', 'drivers/platform', 'include'],
                'microkernel': ['core/ipc', 'core/process', 'core/memory', 'core/signal',
                                'core/security', 'core/clock', 'core/debug', 'servers/pm',
                                'servers/vm', 'servers/vfs', 'servers/net', 'servers/ds',
                                'servers/sm', 'servers/ios', 'servers/cs', 'include'],
                'rump': ['fs', 'net', 'crypto', 'dev', 'include'],
                'libos': ['posix/libc/stdio', 'posix/libc/stdlib', 'posix/libc/string',
                          'posix/libc/math', 'posix/libc/time', 'posix/libc/locale',
                          'posix/libc/pthread', 'posix/syscalls', 'posix/posix/signals',
                          'posix/posix/ipc', 'posix/posix/process', 'posix/posix/io',
                          'posix/compat', 'unix', 'realtime', 'embedded', 'include'],
                'include': ['minix', 'sys', 'arch']
            }
        }
        
        created_count = 0
        for top_dir, subdirs in minix4_structure.items():
            for main_dir, sub_paths in subdirs.items():
                for sub_path in sub_paths:
                    full_path = self.root_path / top_dir / main_dir / sub_path
                    
                    if not full_path.exists():
                        self.log_action(
                            "CREATE_DIR",
                            str(full_path),
                            reason="MINIX4 architecture structure"
                        )
                        
                        if not self.dry_run:
                            full_path.mkdir(parents=True, exist_ok=True)
                            created_count += 1
        
        print(f"✅ Created {created_count} directories")
        return created_count
    
    def generate_cleanup_report(self):
        """Generate detailed cleanup report"""
        print("\n📊 Generating cleanup report...")
        
        report = {
            'summary': {
                'total_actions': len(self.actions_log),
                'by_type': {}
            },
            'actions': self.actions_log
        }
        
        # Count actions by type
        for action in self.actions_log:
            action_type = action['type']
            report['summary']['by_type'][action_type] = report['summary']['by_type'].get(action_type, 0) + 1
        
        # Save report
        report_path = self.root_path / 'cleanup_report.json'
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        print(f"📄 Cleanup report saved to: {report_path}")
        
        # Print summary
        print(f"\n📈 CLEANUP SUMMARY:")
        for action_type, count in report['summary']['by_type'].items():
            print(f"   {action_type}: {count}")
        
        return report

def main():
    if len(sys.argv) > 1:
        root_path = sys.argv[1]
    else:
        root_path = os.getcwd()
    
    # Check if dry run
    dry_run = '--dry-run' in sys.argv or '-n' in sys.argv
    if dry_run:
        print("🚀 MINIX4 Cleanup Script (DRY RUN MODE)")
    else:
        print("🚀 MINIX4 Cleanup Script (LIVE MODE)")
        response = input("This will modify files. Continue? (y/N): ")
        if response.lower() != 'y':
            print("Cleanup cancelled.")
            return
    
    print(f"Working on: {root_path}")
    
    # First, analyze the current state
    print("Phase 1: Analyzing current state...")
    analyzer = FileAnalyzer(root_path)
    analyzer.scan_directory()
    
    # Create cleanup instance
    cleanup = MinixCleanup(root_path, dry_run=dry_run)
    
    # Phase 2: Create proper structure
    print("\nPhase 2: Creating MINIX4 structure...")
    cleanup.create_minix4_structure()
    
    # Phase 3: Clean duplicates
    print("\nPhase 3: Removing duplicates...")
    cleanup.clean_exact_duplicates(analyzer)
    
    # Phase 4: Clean suspicious files
    print("\nPhase 4: Cleaning suspicious files...")
    cleanup.clean_suspicious_files(analyzer)
    
    # Phase 5: Reorganize architecture violations
    print("\nPhase 5: Reorganizing architectural violations...")
    cleanup.reorganize_architecture_violations(analyzer)
    
    # Phase 6: Generate report
    print("\nPhase 6: Generating report...")
    cleanup.generate_cleanup_report()
    
    if dry_run:
        print("\n✅ Dry run completed. Review the actions above and run without --dry-run to execute.")
    else:
        print("\n✅ Cleanup completed!")

if __name__ == "__main__":
    main()
