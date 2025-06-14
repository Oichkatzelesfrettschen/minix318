#!/usr/bin/env python3
"""
Deep Directory Scanner for MINIX4
Scans the entire directory tree recursively to find:
- Exact duplicate files (by hash)
- Similar files (by content similarity)
- Misplaced files according to MINIX4 5-layer architecture
- Nested files in wrong locations
- Suspicious/temporary files
- Files that should be reorganized
"""

import os
import hashlib
import json
import difflib
from collections import defaultdict
from pathlib import Path
import re
import sys

class DeepDirectoryScanner:
    def __init__(self, root_path):
        self.root_path = Path(root_path)
        self.file_hashes = defaultdict(list)
        self.file_sizes = defaultdict(list)
        self.similar_files = []
        self.misplaced_files = []
        self.suspicious_files = []
        self.total_files = 0
        self.total_size = 0
        self.directories_scanned = 0
        
        # Define the MINIX4 5-layer architecture structure
        self.expected_structure = {
            'minix4/exokernel/': ['Layer 1: Exokernel Foundation'],
            'minix4/microkernel/': ['Layer 2: Microkernel Core'],
            'minix4/libos/': ['Layer 5: LibOS Environments'],
            'minix/core/': ['Layer 2: Microkernel Core'],
            'minix/process/': ['Layer 2-3: Process Management'],
            'minix/memory/': ['Layer 2: Memory Management'],
            'minix/ipc/': ['Layer 2: IPC'],
            'minix/filesystem/': ['Layer 3: Filesystem Services'],
            'minix/device/': ['Layer 3: Device Services'],
            'minix/headers/': ['Headers and Interfaces'],
            'userspace/': ['Layer 5: User Applications'],
            'archive/': ['Legacy/Archive - should be minimal'],
            'legacy/': ['Legacy/Archive - should be minimal'],
            'docs/': ['Documentation'],
            'scripts/': ['Build and Development Scripts'],
            'tests/': ['Test Suites'],
            'tools/': ['Development Tools']
        }
        
        # Patterns for files that should NOT be in certain locations
        self.misplacement_patterns = [
            # Legacy files in modern directories
            (re.compile(r'.*/(minix4|minix)/.*/.*legacy.*', re.IGNORECASE), 'Legacy file in modern directory'),
            (re.compile(r'.*/(minix4|minix)/.*/.*old.*', re.IGNORECASE), 'Old file in modern directory'),
            (re.compile(r'.*/(minix4|minix)/.*/.*backup.*', re.IGNORECASE), 'Backup file in source directory'),
            
            # Duplicate directory structures
            (re.compile(r'.*/minix/.*/(minix|kernel)/', re.IGNORECASE), 'Nested minix/kernel directory'),
            (re.compile(r'.*/minix4/.*/(minix4|minix)/', re.IGNORECASE), 'Nested minix4/minix directory'),
            
            # Wrong file types in wrong places
            (re.compile(r'.*/docs/.+\.(c|h|S|asm)$', re.IGNORECASE), 'Source code in docs directory'),
            (re.compile(r'.*/minix/.*\.(md|txt|pdf|doc)$', re.IGNORECASE), 'Documentation in source directory'),
            
            # User/application files in kernel space
            (re.compile(r'.*/minix/core/.*/usr/', re.IGNORECASE), 'User files in kernel core'),
            (re.compile(r'.*/minix/memory/.*/app/', re.IGNORECASE), 'Application files in memory management'),
            
            # Test files mixed with source
            (re.compile(r'.*/minix/(?!tests).*test.*\.(c|h)$', re.IGNORECASE), 'Test file mixed with source'),
            (re.compile(r'.*/minix4/(?!tests).*test.*\.(c|h)$', re.IGNORECASE), 'Test file mixed with source'),
            
            # Build artifacts in source
            (re.compile(r'.*\.(o|obj|exe|dll|so|a|lib)$', re.IGNORECASE), 'Build artifact in source tree'),
            (re.compile(r'.*/(Debug|Release|build|Build)/', re.IGNORECASE), 'Build directory in source tree'),
        ]
        
        # Suspicious file patterns
        self.suspicious_patterns = [
            re.compile(r'.*\.tmp$', re.IGNORECASE),
            re.compile(r'.*\.temp$', re.IGNORECASE),
            re.compile(r'.*\.bak$', re.IGNORECASE),
            re.compile(r'.*\.backup$', re.IGNORECASE),
            re.compile(r'.*~$'),
            re.compile(r'.*\.orig$', re.IGNORECASE),
            re.compile(r'.*\.old$', re.IGNORECASE),
            re.compile(r'.*copy.*', re.IGNORECASE),
            re.compile(r'.*duplicate.*', re.IGNORECASE),
            re.compile(r'.*\(\d+\)\..*', re.IGNORECASE),  # File (1).ext, File (2).ext
            re.compile(r'.*_copy.*', re.IGNORECASE),
            re.compile(r'.*\.conflict.*', re.IGNORECASE),
            re.compile(r'thumbs\.db$', re.IGNORECASE),
            re.compile(r'desktop\.ini$', re.IGNORECASE),
            re.compile(r'\.ds_store$', re.IGNORECASE),
        ]

    def calculate_file_hash(self, filepath):
        """Calculate SHA-256 hash of a file."""
        try:
            hash_sha256 = hashlib.sha256()
            with open(filepath, "rb") as f:
                for chunk in iter(lambda: f.read(4096), b""):
                    hash_sha256.update(chunk)
            return hash_sha256.hexdigest()
        except (IOError, PermissionError, OSError) as e:
            print(f"Warning: Could not hash {filepath}: {e}")
            return None

    def get_file_content_sample(self, filepath, max_size=8192):
        """Get a sample of file content for similarity comparison."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                return f.read(max_size)
        except:
            try:
                with open(filepath, 'rb') as f:
                    return f.read(max_size).decode('utf-8', errors='ignore')
            except:
                return ""

    def files_are_similar(self, file1, file2, threshold=0.8):
        """Check if two files are similar based on content."""
        try:
            content1 = self.get_file_content_sample(file1)
            content2 = self.get_file_content_sample(file2)
            
            if not content1 or not content2:
                return False
                
            similarity = difflib.SequenceMatcher(None, content1, content2).ratio()
            return similarity >= threshold
        except:
            return False

    def is_misplaced_file(self, filepath):
        """Check if a file is misplaced according to architecture rules."""
        str_path = str(filepath)
        relative_path = str(filepath.relative_to(self.root_path))
        
        for pattern, reason in self.misplacement_patterns:
            if pattern.match(str_path):
                return reason
        return None

    def is_suspicious_file(self, filepath):
        """Check if a file is suspicious (temporary, backup, etc.)."""
        filename = filepath.name
        for pattern in self.suspicious_patterns:
            if pattern.match(filename):
                return True
        return False

    def scan_directory_deep(self):
        """Recursively scan all directories and files."""
        print(f"Starting deep directory scan from: {self.root_path}")
        print("This will scan EVERY file and directory recursively...")
        
        # Walk through all directories recursively
        for root, dirs, files in os.walk(self.root_path):
            root_path = Path(root)
            self.directories_scanned += 1
            
            if self.directories_scanned % 100 == 0:
                print(f"Scanned {self.directories_scanned} directories, {self.total_files} files...")
            
            for file in files:
                filepath = root_path / file
                
                try:
                    # Skip if file doesn't exist or is inaccessible
                    if not filepath.exists() or filepath.is_symlink():
                        continue
                    
                    file_size = filepath.stat().st_size
                    self.total_files += 1
                    self.total_size += file_size
                    
                    # Calculate hash for exact duplicate detection
                    file_hash = self.calculate_file_hash(filepath)
                    if file_hash:
                        self.file_hashes[file_hash].append(str(filepath))
                    
                    # Group by size for potential similarity checking
                    self.file_sizes[file_size].append(str(filepath))
                    
                    # Check if file is misplaced
                    misplacement_reason = self.is_misplaced_file(filepath)
                    if misplacement_reason:
                        self.misplaced_files.append({
                            'path': str(filepath),
                            'relative_path': str(filepath.relative_to(self.root_path)),
                            'reason': misplacement_reason,
                            'size': file_size
                        })
                    
                    # Check if file is suspicious
                    if self.is_suspicious_file(filepath):
                        self.suspicious_files.append({
                            'path': str(filepath),
                            'relative_path': str(filepath.relative_to(self.root_path)),
                            'size': file_size
                        })
                        
                except (PermissionError, OSError) as e:
                    print(f"Warning: Could not access {filepath}: {e}")
                    continue

    def find_similar_files(self, max_pairs=1000):
        """Find similar files by comparing content."""
        print("Finding similar files...")
        similar_count = 0
        
        # Only check files of similar sizes to avoid unnecessary comparisons
        for size, files in self.file_sizes.items():
            if len(files) > 1 and similar_count < max_pairs:
                for i in range(len(files)):
                    for j in range(i + 1, len(files)):
                        if similar_count >= max_pairs:
                            break
                        
                        file1, file2 = files[i], files[j]
                        
                        # Skip if they're exact duplicates (already found)
                        hash1 = self.calculate_file_hash(file1)
                        hash2 = self.calculate_file_hash(file2)
                        if hash1 and hash2 and hash1 == hash2:
                            continue
                        
                        if self.files_are_similar(file1, file2):
                            self.similar_files.append({
                                'file1': file1,
                                'file2': file2,
                                'size': size
                            })
                            similar_count += 1

    def analyze_directory_structure(self):
        """Analyze the directory structure for architectural compliance."""
        structure_analysis = {}
        
        for root, dirs, files in os.walk(self.root_path):
            root_path = Path(root)
            relative_path = str(root_path.relative_to(self.root_path))
            
            # Count files and subdirectories
            structure_analysis[relative_path] = {
                'num_files': len(files),
                'num_subdirs': len(dirs),
                'total_size': sum(
                    (root_path / f).stat().st_size 
                    for f in files 
                    if (root_path / f).exists()
                ),
                'subdirs': dirs
            }
        
        return structure_analysis

    def generate_report(self):
        """Generate comprehensive analysis report."""
        # Find exact duplicates
        exact_duplicates = {h: files for h, files in self.file_hashes.items() if len(files) > 1}
        
        # Find similar files
        self.find_similar_files()
        
        # Analyze directory structure
        structure_analysis = self.analyze_directory_structure()
        
        report = {
            'scan_summary': {
                'total_files': self.total_files,
                'total_size_mb': round(self.total_size / (1024 * 1024), 2),
                'directories_scanned': self.directories_scanned,
                'exact_duplicates': len(exact_duplicates),
                'duplicate_groups': len([group for group in exact_duplicates.values() if len(group) > 1]),
                'similar_file_pairs': len(self.similar_files),
                'misplaced_files': len(self.misplaced_files),
                'suspicious_files': len(self.suspicious_files)
            },
            'exact_duplicates': exact_duplicates,
            'similar_files': self.similar_files,
            'misplaced_files': self.misplaced_files,
            'suspicious_files': self.suspicious_files,
            'directory_structure': structure_analysis
        }
        
        return report

    def print_summary(self, report):
        """Print a human-readable summary of the analysis."""
        summary = report['scan_summary']
        
        print("\n" + "="*80)
        print("MINIX4 DEEP DIRECTORY SCAN RESULTS")
        print("="*80)
        
        print(f"\n📊 SCAN STATISTICS:")
        print(f"   Total files scanned: {summary['total_files']:,}")
        print(f"   Total size: {summary['total_size_mb']:,} MB")
        print(f"   Directories scanned: {summary['directories_scanned']:,}")
        
        print(f"\n🔍 DUPLICATE ANALYSIS:")
        print(f"   Exact duplicate files: {summary['exact_duplicates']:,}")
        print(f"   Duplicate groups: {summary['duplicate_groups']:,}")
        print(f"   Similar file pairs: {summary['similar_file_pairs']:,}")
        
        print(f"\n🏗️ ARCHITECTURAL ANALYSIS:")
        print(f"   Misplaced files: {summary['misplaced_files']:,}")
        print(f"   Suspicious files: {summary['suspicious_files']:,}")
        
        # Show top misplacement reasons
        if report['misplaced_files']:
            print(f"\n📍 TOP MISPLACEMENT REASONS:")
            reasons = defaultdict(int)
            for file_info in report['misplaced_files']:
                reasons[file_info['reason']] += 1
            
            for reason, count in sorted(reasons.items(), key=lambda x: x[1], reverse=True)[:10]:
                print(f"   {reason}: {count} files")
        
        # Show largest duplicate groups
        if report['exact_duplicates']:
            print(f"\n📂 LARGEST DUPLICATE GROUPS:")
            largest_groups = sorted(
                report['exact_duplicates'].items(),
                key=lambda x: len(x[1]),
                reverse=True
            )[:10]
            
            for hash_val, files in largest_groups:
                print(f"   {len(files)} copies of file (hash: {hash_val[:12]}...):")
                for file_path in files[:3]:  # Show first 3 files
                    rel_path = str(Path(file_path).relative_to(self.root_path))
                    print(f"     - {rel_path}")
                if len(files) > 3:
                    print(f"     ... and {len(files) - 3} more")
                print()
        
        # Show directory size analysis
        print(f"\n📁 LARGEST DIRECTORIES BY FILE COUNT:")
        dir_sizes = [(path, info['num_files']) for path, info in report['directory_structure'].items()]
        dir_sizes.sort(key=lambda x: x[1], reverse=True)
        
        for path, file_count in dir_sizes[:15]:
            if file_count > 0:
                size_mb = report['directory_structure'][path]['total_size'] / (1024 * 1024)
                print(f"   {path}: {file_count} files ({size_mb:.1f} MB)")

def main():
    if len(sys.argv) > 1:
        root_path = sys.argv[1]
    else:
        root_path = "."
    
    scanner = DeepDirectoryScanner(root_path)
    
    print("Starting DEEP directory scan...")
    print("This will analyze EVERY file in the entire directory tree.")
    print("This may take several minutes for large codebases.")
    
    # Perform the deep scan
    scanner.scan_directory_deep()
    
    # Generate comprehensive report
    print("\nGenerating analysis report...")
    report = scanner.generate_report()
    
    # Print summary
    scanner.print_summary(report)
    
    # Save detailed report to file
    report_file = Path(root_path) / "scripts" / "analysis" / "deep_scan_report.json"
    report_file.parent.mkdir(parents=True, exist_ok=True)
    
    with open(report_file, 'w') as f:
        json.dump(report, f, indent=2)
    
    print(f"\n💾 Detailed report saved to: {report_file}")
    print(f"\n✅ Deep directory scan complete!")

if __name__ == "__main__":
    main()
