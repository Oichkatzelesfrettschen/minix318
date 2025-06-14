#!/usr/bin/env python3
"""
Smart Duplicate Merger for MINIX4
Merges duplicate files based on content similarity and logic analysis.
Uses multiprocessing for efficient parallel processing.
"""

import os
import json
import hashlib
import difflib
from pathlib import Path
from collections import defaultdict
import re
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, as_completed
import time

def calculate_file_hash_worker(file_path):
    """Worker function to calculate file hash - can be used in multiprocessing."""
    try:
        with open(file_path, 'rb') as f:
            content = f.read()
            if len(content) == 0:
                return str(file_path), None, 0  # Skip empty files
            return str(file_path), hashlib.sha256(content).hexdigest(), len(content)
    except Exception as e:
        return str(file_path), None, 0

def analyze_code_quality_worker(file_path):
    """Worker function to analyze code quality - can be used in multiprocessing."""
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Simple quality metrics
        score = 0
        
        # Prefer files with more comments
        comment_lines = len(re.findall(r'^\s*[/*#]', content, re.MULTILINE))
        score += comment_lines * 2
        
        # Prefer files with function documentation
        if re.search(r'/\*\*.*@brief', content, re.DOTALL):
            score += 20
        
        # Prefer files with includes/headers
        if re.search(r'#include\s*[<"]', content):
            score += 10
        
        # Prefer files with proper error handling
        if re.search(r'(errno|EINVAL|ENOMEM)', content):
            score += 10
        
        # Prefer C23 features
        if re.search(r'(_Static_assert|typeof|_BitInt)', content):
            score += 15
        
        # Penalty for copy in filename
        if 'copy' in str(file_path).lower():
            score -= 50
            
        return str(file_path), score
    except:
        return str(file_path), 0

def calculate_similarity_worker(args):
    """Worker function to calculate file similarity - can be used in multiprocessing."""
    file1, file2 = args
    try:
        with open(file1, 'r', encoding='utf-8', errors='ignore') as f1:
            content1 = f1.readlines()
        with open(file2, 'r', encoding='utf-8', errors='ignore') as f2:
            content2 = f2.readlines()
        
        matcher = difflib.SequenceMatcher(None, content1, content2)
        return str(file1), str(file2), matcher.ratio()
    except:
        return str(file1), str(file2), 0.0

class SmartDuplicateMerger:
    def __init__(self, root_path, dry_run=True):
        self.root_path = Path(root_path)
        self.dry_run = dry_run
        self.actions_taken = []
        
        # Preference order for directories (prefer newer/cleaner structure)
        self.directory_preference = [
            'minix/',
            'minix4/microkernel/',
            'minix4/libos/',
            'minix4/exokernel/',
            'minix4/include_legacy/',
            'minix4/libos_legacy/',
            'archive/',
            'docs/susv4-2018-main/',
            'tar/'
        ]
        
        # Files to never merge/delete
        self.protected_files = {
            'Makefile', 'meson.build', 'CMakeLists.txt',
            'README.md', 'LICENSE', 'COPYING',
            'main.c', 'kernel.c', 'init.c'
        }

    def calculate_file_hash(self, file_path):
        """Calculate hash of a single file."""
        try:
            with open(file_path, 'rb') as f:
                content = f.read()
                if len(content) == 0:
                    return None
                return hashlib.sha256(content).hexdigest()
        except Exception:
            return None

    def calculate_similarity(self, file1, file2):
        """Calculate similarity between two text files."""
        try:
            with open(file1, 'r', encoding='utf-8', errors='ignore') as f1:
                content1 = f1.readlines()
            with open(file2, 'r', encoding='utf-8', errors='ignore') as f2:
                content2 = f2.readlines()
            
            matcher = difflib.SequenceMatcher(None, content1, content2)
            return matcher.ratio()
        except:
            return 0.0

    def analyze_code_quality(self, file_path):
        """Analyze code quality metrics for a file."""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Simple quality metrics
            score = 0
            
            # Prefer files with more comments
            comment_lines = len(re.findall(r'^\s*[/*#]', content, re.MULTILINE))
            score += comment_lines * 2
            
            # Prefer files with function documentation
            if re.search(r'/\*\*.*@brief', content, re.DOTALL):
                score += 20
            
            # Prefer files with includes/headers
            if re.search(r'#include\s*[<"]', content):
                score += 10
            
            # Prefer files with proper error handling
            if re.search(r'(errno|EINVAL|ENOMEM)', content):
                score += 10
            
            # Prefer C23 features
            if re.search(r'(_Static_assert|typeof|_BitInt)', content):
                score += 15
            
            # Penalty for copy in filename
            if 'copy' in file_path.name.lower():
                score -= 50
                
            return score
        except:
            return 0

    def get_directory_priority(self, file_path):
        """Get priority score for directory (higher = better)."""
        path_str = str(file_path)
        for i, preferred_dir in enumerate(self.directory_preference):
            if path_str.startswith(preferred_dir):
                return len(self.directory_preference) - i
        return 0    def is_protected_file(self, file_path):
        """Check if file should be protected from merging."""
        name = file_path.name.lower()
        for protected in self.protected_files:
            if protected.lower() in name:
                return True
        return False

    def find_duplicates(self):
        """Find duplicate files and group them using multiprocessing."""
        print("🔍 Scanning for duplicate files with parallel processing...")
        
        # Collect all file paths first
        all_files = []
        for file_path in self.root_path.rglob('*'):
            if file_path.is_file():
                all_files.append(file_path)
        
        print(f"  Found {len(all_files)} files to process...")
        
        file_hashes = defaultdict(list)
        
        # Use multiprocessing to calculate hashes
        cpu_count = mp.cpu_count()
        print(f"  Using {cpu_count} CPU cores for parallel hashing...")
        
        start_time = time.time()
        with ProcessPoolExecutor(max_workers=cpu_count) as executor:
            # Submit all hash calculation tasks
            future_to_file = {executor.submit(calculate_file_hash_worker, file_path): file_path 
                              for file_path in all_files}
            
            processed = 0
            for future in as_completed(future_to_file):
                file_path_str, file_hash, size = future.result()
                processed += 1
                
                if processed % 1000 == 0:
                    elapsed = time.time() - start_time
                    rate = processed / elapsed if elapsed > 0 else 0
                    print(f"    Processed {processed}/{len(all_files)} files ({rate:.1f} files/sec)")
                
                if file_hash:
                    file_hashes[file_hash].append(Path(file_path_str))
        
        elapsed = time.time() - start_time
        print(f"  Hashing completed in {elapsed:.1f} seconds")
        
        # Filter to only duplicates
        duplicates = {h: files for h, files in file_hashes.items() if len(files) > 1}
        
        print(f"✅ Found {len(duplicates)} groups of duplicate files from {len(all_files)} total files")
        return duplicates

    def merge_duplicate_group(self, files):
        """Merge a group of duplicate files, keeping the best one."""
        if len(files) < 2:
            return
        
        print(f"\n📁 Analyzing duplicate group ({len(files)} files):")
        for f in files:
            print(f"  - {f}")
        
        # Skip if any file is protected
        protected_files = [f for f in files if self.is_protected_file(f)]
        if protected_files:
            print(f"  ⚠️  SKIP: Contains protected files: {[str(f) for f in protected_files]}")
            return
        
        # Score each file
        file_scores = []
        for file_path in files:
            score = 0
            
            # Directory priority
            score += self.get_directory_priority(file_path) * 100
            
            # Code quality
            score += self.analyze_code_quality(file_path)
            
            # File size (prefer non-empty)
            try:
                size = file_path.stat().st_size
                if size > 0:
                    score += min(size // 100, 50)  # Cap bonus at 50
            except:
                pass
            
            file_scores.append((score, file_path))
        
        # Sort by score (highest first)
        file_scores.sort(reverse=True)
        
        # Keep the best file, remove others
        keep_file = file_scores[0][1]
        remove_files = [fs[1] for fs in file_scores[1:]]
        
        print(f"  ✅ KEEP: {keep_file} (score: {file_scores[0][0]})")
        
        for score, remove_file in file_scores[1:]:
            print(f"  🗑️  REMOVE: {remove_file} (score: {score})")
            
            if not self.dry_run:
                try:
                    remove_file.unlink()
                    self.actions_taken.append({
                        'action': 'MERGE_REMOVE',
                        'removed_file': str(remove_file),
                        'kept_file': str(keep_file),
                        'reason': 'Lower quality duplicate'
                    })
                except Exception as e:
                    print(f"    ❌ Error removing {remove_file}: {e}")

    def find_similar_files(self, similarity_threshold=0.8):
        """Find files with high content similarity."""
        print(f"\n🔍 Finding similar files (threshold: {similarity_threshold})...")
        
        # Group files by extension
        files_by_ext = defaultdict(list)
        for file_path in self.root_path.rglob('*'):
            if file_path.is_file() and file_path.suffix in ['.c', '.h', '.S']:
                files_by_ext[file_path.suffix].append(file_path)
        
        similar_groups = []
        
        for ext, files in files_by_ext.items():
            print(f"  Checking {len(files)} {ext} files...")
            
            for i, file1 in enumerate(files):
                if self.is_protected_file(file1):
                    continue
                    
                for file2 in files[i+1:]:
                    if self.is_protected_file(file2):
                        continue
                    
                    similarity = self.calculate_similarity(file1, file2)
                    if similarity >= similarity_threshold:
                        similar_groups.append((similarity, file1, file2))
        
        return similar_groups

    def merge_similar_files(self, similarity_threshold=0.8):
        """Merge files with high content similarity."""
        similar_groups = self.find_similar_files(similarity_threshold)
        
        print(f"📋 Found {len(similar_groups)} pairs of similar files")
        
        for similarity, file1, file2 in similar_groups:
            print(f"\n🔗 Similar files ({similarity:.2%} match):")
            print(f"  File 1: {file1}")
            print(f"  File 2: {file2}")
            
            # Score both files
            score1 = (self.get_directory_priority(file1) * 100 + 
                     self.analyze_code_quality(file1))
            score2 = (self.get_directory_priority(file2) * 100 + 
                     self.analyze_code_quality(file2))
            
            if score1 > score2:
                keep_file, remove_file = file1, file2
                keep_score, remove_score = score1, score2
            else:
                keep_file, remove_file = file2, file1
                keep_score, remove_score = score2, score1
            
            print(f"  ✅ KEEP: {keep_file} (score: {keep_score})")
            print(f"  🗑️  REMOVE: {remove_file} (score: {remove_score})")
            
            if not self.dry_run:
                try:
                    remove_file.unlink()
                    self.actions_taken.append({
                        'action': 'SIMILARITY_MERGE',
                        'removed_file': str(remove_file),
                        'kept_file': str(keep_file),
                        'similarity': similarity,
                        'reason': f'Similar content ({similarity:.2%} match)'
                    })
                except Exception as e:
                    print(f"    ❌ Error removing {remove_file}: {e}")

    def run_smart_merge(self):
        """Run the complete smart merge process."""
        print("🚀 Starting MINIX4 Smart Duplicate Merger")
        print(f"   Mode: {'DRY RUN' if self.dry_run else 'LIVE MODE'}")
        print(f"   Root: {self.root_path}")
        print("=" * 80)
        
        # Phase 1: Exact duplicates
        print("\nPHASE 1: EXACT DUPLICATE MERGING")
        print("=" * 50)
        duplicates = self.find_duplicates()
        
        for file_hash, files in duplicates.items():
            self.merge_duplicate_group(files)
        
        # Phase 2: Similar files
        print("\nPHASE 2: SIMILAR FILE MERGING")
        print("=" * 50)
        self.merge_similar_files(similarity_threshold=0.85)
        
        # Summary
        print("\n" + "=" * 80)
        print("SMART MERGE SUMMARY")
        print("=" * 80)
        print(f"✅ Mode: {'DRY RUN' if self.dry_run else 'LIVE MODE'}")
        print(f"📊 Actions taken: {len(self.actions_taken)}")
        
        if self.actions_taken:
            # Save detailed log
            log_file = self.root_path / "scripts" / "analysis" / f"smart_merge_log_{'dry' if self.dry_run else 'live'}.json"
            with open(log_file, 'w') as f:
                json.dump({
                    'summary': {
                        'dry_run': self.dry_run,
                        'actions_count': len(self.actions_taken),
                        'timestamp': str(Path(__file__).stat().st_mtime)
                    },
                    'actions': self.actions_taken
                }, f, indent=2)
            
            print(f"💾 Detailed log saved: {log_file}")

def main():
    import sys
    
    # Parse command line arguments
    dry_run = True
    if len(sys.argv) > 1 and sys.argv[1] == '--live':
        dry_run = False
        print("⚠️  LIVE MODE ENABLED - Files will be permanently merged!")
        response = input("Are you sure you want to continue? (yes/no): ")
        if response.lower() != 'yes':
            print("Cancelled.")
            return
    
    root_path = "."
    if len(sys.argv) > 2:
        root_path = sys.argv[2]
    
    try:
        merger = SmartDuplicateMerger(root_path, dry_run=dry_run)
        merger.run_smart_merge()
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    main()
