#!/usr/bin/env python3
"""
MINIX4 Comprehensive Cleanup Script
Based on deep directory scan findings, this script performs:
1. Safe removal of temporary/suspicious files
2. Removal of build artifacts from source tree
3. Consolidation of exact duplicates (prefer minix4/ over legacy/archive)
4. Cleanup of empty files and test artifacts
"""

import os
import json
import shutil
from pathlib import Path
from collections import defaultdict
import hashlib

class MINIX4Cleaner:
    def __init__(self, root_path, dry_run=True):
        self.root_path = Path(root_path)
        self.dry_run = dry_run
        self.actions_taken = []
        self.space_saved = 0
        self.files_removed = 0
        
        # Load the deep scan report
        self.report_file = self.root_path / "scripts" / "analysis" / "deep_scan_report.json"
        if not self.report_file.exists():
            raise FileNotFoundError(f"Deep scan report not found: {self.report_file}")
        
        with open(self.report_file, 'r') as f:
            self.report = json.load(f)

    def log_action(self, action, file_path, reason="", size=0):
        """Log an action taken during cleanup."""
        self.actions_taken.append({
            'action': action,
            'file': str(file_path),
            'reason': reason,
            'size': size
        })
        if action == 'DELETE':
            self.files_removed += 1
            self.space_saved += size

    def is_safe_to_remove(self, file_path):
        """Check if a file is safe to remove (not critical system files)."""
        path_str = str(file_path).lower()
        
        # Never remove critical files
        critical_patterns = [
            'makefile',  # Main Makefiles
            'readme',
            'license', 
            'dockerfile',
            'gitignore',
            '.github',
            'package.json',
            'meson.build'
        ]
        
        filename = file_path.name.lower()
        for pattern in critical_patterns:
            if pattern in filename and not any(x in filename for x in ['copy', 'backup', '.bak', '.tmp']):
                return False
        
        return True

    def clean_suspicious_files(self):
        """Remove suspicious/temporary files."""
        print("🧹 Cleaning suspicious files...")
        
        suspicious_files = self.report.get('suspicious_files', [])
        
        for file_info in suspicious_files:
            file_path = Path(file_info['path'])
            
            if not file_path.exists():
                continue
            
            # Skip if not safe to remove
            if not self.is_safe_to_remove(file_path):
                continue
            
            size = file_info.get('size', 0)
            
            if self.dry_run:
                print(f"  [DRY RUN] Would remove: {file_path.relative_to(self.root_path)}")
            else:
                try:
                    file_path.unlink()
                    print(f"  Removed: {file_path.relative_to(self.root_path)}")
                except Exception as e:
                    print(f"  Error removing {file_path}: {e}")
                    continue
            
            self.log_action('DELETE', file_path, 'Suspicious file', size)

    def clean_build_artifacts(self):
        """Remove build artifacts from source tree."""
        print("🔨 Cleaning build artifacts...")
        
        misplaced_files = self.report.get('misplaced_files', [])
        
        for file_info in misplaced_files:
            if 'Build artifact' not in file_info['reason']:
                continue
                
            file_path = Path(file_info['path'])
            
            if not file_path.exists():
                continue
            
            size = file_info.get('size', 0)
            
            if self.dry_run:
                print(f"  [DRY RUN] Would remove: {file_path.relative_to(self.root_path)}")
            else:
                try:
                    file_path.unlink()
                    print(f"  Removed: {file_path.relative_to(self.root_path)}")
                except Exception as e:
                    print(f"  Error removing {file_path}: {e}")
                    continue
            
            self.log_action('DELETE', file_path, 'Build artifact', size)

    def consolidate_duplicates(self, min_copies=10):
        """Consolidate exact duplicate files, preferring modern locations."""
        print(f"📋 Consolidating duplicates (groups with {min_copies}+ copies)...")
        
        exact_duplicates = self.report.get('exact_duplicates', {})
        
        # Priority order for keeping files (higher number = higher priority)
        location_priority = {
            'minix4/': 100,
            'minix/': 80,
            'userspace/': 70,
            'docs/': 60,
            'scripts/': 50,
            'tests/': 40,
            'tools/': 30,
            'archive/': 10,
            'legacy/': 5
        }
        
        for hash_val, file_list in exact_duplicates.items():
            if len(file_list) < min_copies:
                continue
            
            # Determine which file to keep based on location priority
            files_with_priority = []
            for file_path_str in file_list:
                file_path = Path(file_path_str)
                
                if not file_path.exists():
                    continue
                
                # Calculate priority based on path
                priority = 0
                for pattern, prio in location_priority.items():
                    if pattern in str(file_path):
                        priority = max(priority, prio)
                
                # Prefer non-legacy, non-test files
                if 'legacy' in str(file_path).lower():
                    priority -= 20
                if 'test' in str(file_path).lower() and file_path.suffix in ['.cpp', '.c']:
                    priority -= 10
                
                files_with_priority.append((file_path, priority))
            
            if len(files_with_priority) <= 1:
                continue
            
            # Sort by priority (highest first)
            files_with_priority.sort(key=lambda x: x[1], reverse=True)
            
            # Keep the highest priority file, remove the rest
            keep_file = files_with_priority[0][0]
            remove_files = [f[0] for f in files_with_priority[1:]]
            
            # Special handling for test files - remove most if they're just placeholders
            if 'nothing_to_do.pass.cpp' in keep_file.name or 'tested_elsewhere.pass.cpp' in keep_file.name:
                # Keep only one in a reasonable location, remove all others
                reasonable_locations = [f for f in files_with_priority if 'minix4' in str(f[0]) and 'test' in str(f[0])]
                if reasonable_locations:
                    keep_file = reasonable_locations[0][0]
                    remove_files = [f[0] for f in files_with_priority if f[0] != keep_file]
            
            print(f"\n  Hash {hash_val[:12]}... ({len(file_list)} copies)")
            print(f"    KEEP: {keep_file.relative_to(self.root_path)}")
            
            for remove_file in remove_files:
                if not self.is_safe_to_remove(remove_file):
                    print(f"    SKIP: {remove_file.relative_to(self.root_path)} (not safe)")
                    continue
                
                try:
                    size = remove_file.stat().st_size if remove_file.exists() else 0
                except:
                    size = 0
                
                if self.dry_run:
                    print(f"    [DRY RUN] Would remove: {remove_file.relative_to(self.root_path)}")
                else:
                    try:
                        remove_file.unlink()
                        print(f"    REMOVED: {remove_file.relative_to(self.root_path)}")
                    except Exception as e:
                        print(f"    ERROR removing {remove_file}: {e}")
                        continue
                
                self.log_action('DELETE', remove_file, f'Duplicate of {keep_file.name}', size)

    def clean_empty_files(self):
        """Remove empty files (0 bytes)."""
        print("📄 Cleaning empty files...")
        
        # Find the hash for empty files (common hash for 0-byte files)
        empty_hash = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
        
        exact_duplicates = self.report.get('exact_duplicates', {})
        
        if empty_hash in exact_duplicates:
            empty_files = exact_duplicates[empty_hash]
            print(f"  Found {len(empty_files)} empty files")
            
            # Keep at most 2-3 empty files in reasonable locations, remove the rest
            keep_count = 0
            max_keep = 3
            
            for file_path_str in empty_files:
                file_path = Path(file_path_str)
                
                if not file_path.exists():
                    continue
                
                # Keep some empty files that might be placeholders
                should_keep = (
                    keep_count < max_keep and
                    ('minix4' in str(file_path) or 'docs' in str(file_path)) and
                    'legacy' not in str(file_path).lower() and
                    self.is_safe_to_remove(file_path)
                )
                
                if should_keep:
                    keep_count += 1
                    print(f"    KEEP: {file_path.relative_to(self.root_path)}")
                    continue
                
                if not self.is_safe_to_remove(file_path):
                    continue
                
                if self.dry_run:
                    print(f"    [DRY RUN] Would remove: {file_path.relative_to(self.root_path)}")
                else:
                    try:
                        file_path.unlink()
                        print(f"    REMOVED: {file_path.relative_to(self.root_path)}")
                    except Exception as e:
                        print(f"    ERROR removing {file_path}: {e}")
                        continue
                
                self.log_action('DELETE', file_path, 'Empty file', 0)

    def cleanup_test_placeholders(self):
        """Remove excessive test placeholder files."""
        print("🧪 Cleaning test placeholder files...")
        
        exact_duplicates = self.report.get('exact_duplicates', {})
        
        for hash_val, file_list in exact_duplicates.items():
            # Look for test files
            test_files = [f for f in file_list if any(
                pattern in Path(f).name.lower() 
                for pattern in ['nothing_to_do', 'tested_elsewhere', 'placeholder']
            )]
            
            if len(test_files) < 5:  # Only clean if there are many copies
                continue
            
            print(f"\n  Test placeholder group ({len(test_files)} copies)")
            
            # Keep 1-2 files in reasonable test directories
            kept_count = 0
            max_keep = 2
            
            for file_path_str in sorted(test_files):
                file_path = Path(file_path_str)
                
                if not file_path.exists():
                    continue
                
                # Keep a few in proper test locations
                should_keep = (
                    kept_count < max_keep and
                    'test' in str(file_path) and
                    'minix4' in str(file_path) and
                    'legacy' not in str(file_path).lower()
                )
                
                if should_keep:
                    kept_count += 1
                    print(f"    KEEP: {file_path.relative_to(self.root_path)}")
                    continue
                
                try:
                    size = file_path.stat().st_size if file_path.exists() else 0
                except:
                    size = 0
                
                if self.dry_run:
                    print(f"    [DRY RUN] Would remove: {file_path.relative_to(self.root_path)}")
                else:
                    try:
                        file_path.unlink()
                        print(f"    REMOVED: {file_path.relative_to(self.root_path)}")
                    except Exception as e:
                        print(f"    ERROR removing {file_path}: {e}")
                        continue
                
                self.log_action('DELETE', file_path, 'Test placeholder', size)

    def generate_summary(self):
        """Generate cleanup summary report."""
        print("\n" + "="*80)
        print("CLEANUP SUMMARY")
        print("="*80)
        
        if self.dry_run:
            print("🔍 DRY RUN MODE - No files were actually modified")
        else:
            print("✅ LIVE RUN - Files were actually removed")
        
        print(f"\n📊 ACTIONS TAKEN:")
        print(f"   Files removed: {self.files_removed:,}")
        print(f"   Space saved: {self.space_saved / (1024*1024):.1f} MB")
        
        # Group actions by type
        action_summary = defaultdict(int)
        for action in self.actions_taken:
            action_summary[action['reason']] += 1
        
        print(f"\n📋 BREAKDOWN BY TYPE:")
        for reason, count in sorted(action_summary.items(), key=lambda x: x[1], reverse=True):
            print(f"   {reason}: {count} files")
        
        # Save detailed log
        log_file = self.root_path / "scripts" / "analysis" / f"cleanup_log_{'dry' if self.dry_run else 'live'}.json"
        with open(log_file, 'w') as f:
            json.dump({
                'summary': {
                    'dry_run': self.dry_run,
                    'files_removed': self.files_removed,
                    'space_saved_mb': self.space_saved / (1024*1024),
                    'action_summary': dict(action_summary)
                },
                'detailed_actions': self.actions_taken
            }, f, indent=2)
        
        print(f"\n💾 Detailed log saved: {log_file}")

    def run_cleanup(self):
        """Run the complete cleanup process."""
        print("🚀 Starting MINIX4 comprehensive cleanup...")
        print(f"   Mode: {'DRY RUN' if self.dry_run else 'LIVE RUN'}")
        print(f"   Root: {self.root_path}")
        
        # Phase 1: Safe cleanup
        print(f"\n{'='*60}")
        print("PHASE 1: SAFE CLEANUP")
        print(f"{'='*60}")
        
        self.clean_suspicious_files()
        self.clean_build_artifacts()
        self.clean_empty_files()
          # Phase 2: Duplicate consolidation  
        print(f"\n{'='*60}")
        print("PHASE 2: DUPLICATE CONSOLIDATION")
        print(f"{'='*60}")
        
        self.cleanup_test_placeholders()
        self.consolidate_duplicates(min_copies=10)
        
        # Generate summary
        self.generate_summary()

def main():
    import sys
    
    # Parse command line arguments
    dry_run = True
    if len(sys.argv) > 1 and sys.argv[1] == '--live':
        dry_run = False
        print("⚠️  LIVE MODE ENABLED - Files will be permanently removed!")
        response = input("Are you sure you want to continue? (yes/no): ")
        if response.lower() != 'yes':
            print("Cancelled.")
            return
    
    root_path = "."
    if len(sys.argv) > 2:
        root_path = sys.argv[2]
    
    try:
        cleaner = MINIX4Cleaner(root_path, dry_run=dry_run)
        cleaner.run_cleanup()
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    main()
