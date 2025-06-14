#!/usr/bin/env python3
"""
MINIX4 Deep Scan Analysis and Cleanup Recommendations
Analyzes the deep scan report and provides actionable cleanup recommendations
"""

import json
import os
from pathlib import Path
from collections import defaultdict, Counter

def analyze_deep_scan_report(report_file):
    """Analyze the deep scan report and generate cleanup recommendations."""
    
    with open(report_file, 'r') as f:
        report = json.load(f)
    
    print("="*80)
    print("MINIX4 DEEP SCAN ANALYSIS & CLEANUP RECOMMENDATIONS")
    print("="*80)
    
    summary = report['scan_summary']
    print(f"\n📊 SCAN OVERVIEW:")
    print(f"   • Total files: {summary['total_files']:,}")
    print(f"   • Total size: {summary['total_size_mb']:,} MB") 
    print(f"   • Directories: {summary['directories_scanned']:,}")
    print(f"   • Exact duplicates: {summary['exact_duplicates']:,}")
    print(f"   • Similar files: {summary['similar_file_pairs']:,}")
    print(f"   • Misplaced files: {summary['misplaced_files']:,}")
    print(f"   • Suspicious files: {summary['suspicious_files']:,}")
    
    # Analyze exact duplicates
    print(f"\n🔍 DUPLICATE FILE ANALYSIS:")
    exact_dupes = report['exact_duplicates']
    
    # Calculate potential space savings
    total_duplicate_size = 0
    largest_groups = []
    
    for hash_val, files in exact_dupes.items():
        if len(files) > 1:
            # Estimate file size (we'd need to check actual files for exact size)
            group_size = len(files)
            largest_groups.append((hash_val, files, group_size))
    
    largest_groups.sort(key=lambda x: x[2], reverse=True)
    
    print(f"   • {len(exact_dupes):,} duplicate groups found")
    print(f"   • Top 10 largest duplicate groups:")
    
    for i, (hash_val, files, count) in enumerate(largest_groups[:10]):
        print(f"     {i+1:2d}. {count:3d} copies: {Path(files[0]).name}")
        
        # Categorize the files by location
        locations = defaultdict(list)
        for file_path in files:
            if 'archive' in file_path.lower():
                locations['archive'].append(file_path)
            elif 'legacy' in file_path.lower():
                locations['legacy'].append(file_path)
            elif 'minix4' in file_path.lower():
                locations['minix4'].append(file_path)
            elif 'minix' in file_path.lower():
                locations['minix'].append(file_path)
            elif 'docs' in file_path.lower():
                locations['docs'].append(file_path)
            else:
                locations['other'].append(file_path)
        
        print(f"         Locations: {', '.join(f'{k}({len(v)})' for k, v in locations.items())}")
    
    # Analyze misplaced files
    print(f"\n🏗️ ARCHITECTURAL VIOLATIONS:")
    misplaced = report['misplaced_files']
    
    if misplaced:
        violation_types = Counter([item['reason'] for item in misplaced])
        for reason, count in violation_types.most_common():
            print(f"   • {reason}: {count} files")
            
        print(f"\n   Examples of misplaced files:")
        for item in misplaced[:10]:
            print(f"     - {item['relative_path']}")
            print(f"       Reason: {item['reason']}")
    
    # Analyze suspicious files
    print(f"\n🚨 SUSPICIOUS FILES:")
    suspicious = report['suspicious_files']
    
    if suspicious:
        # Group by file extension/pattern
        patterns = defaultdict(list)
        for item in suspicious:
            filename = Path(item['path']).name
            if filename.endswith('.tmp') or filename.endswith('.temp'):
                patterns['Temporary files'].append(item)
            elif filename.endswith('.bak') or filename.endswith('.backup') or filename.endswith('.orig'):
                patterns['Backup files'].append(item)
            elif 'copy' in filename.lower() or 'duplicate' in filename.lower():
                patterns['Copy/Duplicate files'].append(item)
            elif filename.endswith('~'):
                patterns['Editor backup files'].append(item)
            elif '(' in filename and ')' in filename:
                patterns['Numbered copies'].append(item)
            else:
                patterns['Other suspicious'].append(item)
        
        for pattern, files in patterns.items():
            print(f"   • {pattern}: {len(files)} files")
            for file_info in files[:3]:  # Show first 3 examples
                print(f"     - {file_info['relative_path']}")
            if len(files) > 3:
                print(f"     ... and {len(files) - 3} more")
    
    # Directory structure analysis
    print(f"\n📁 DIRECTORY STRUCTURE ANALYSIS:")
    dir_structure = report['directory_structure']
    
    # Find directories with most files
    big_dirs = [(path, info['num_files']) for path, info in dir_structure.items() if info['num_files'] > 100]
    big_dirs.sort(key=lambda x: x[1], reverse=True)
    
    print(f"   Directories with >100 files:")
    for path, file_count in big_dirs[:15]:
        size_mb = dir_structure[path]['total_size'] / (1024 * 1024)
        print(f"     {path}: {file_count} files ({size_mb:.1f} MB)")
    
    # Generate cleanup recommendations
    print(f"\n🔧 CLEANUP RECOMMENDATIONS:")
    print(f"\n1. IMMEDIATE ACTIONS (High Priority):")
    
    if suspicious:
        print(f"   • Remove {len(suspicious)} suspicious/temporary files")
        print(f"     Command: Create cleanup script to remove .tmp, .bak, ~ files")
    
    if exact_dupes:
        top_dupes = [(hash_val, files) for hash_val, files in exact_dupes.items() if len(files) >= 10]
        print(f"   • Address {len(top_dupes)} major duplicate groups (10+ copies each)")
        print(f"     Focus on: libcxx test files, empty files, Makefiles")
    
    print(f"\n2. MEDIUM PRIORITY:")
    if misplaced:
        print(f"   • Fix {len(misplaced)} architectural violations")
        print(f"     Move build artifacts out of source tree")
    
    print(f"   • Review and merge similar files ({summary['similar_file_pairs']} pairs)")
    
    print(f"\n3. LONG-TERM RESTRUCTURING:")
    print(f"   • Consolidate legacy code in archive/")
    print(f"   • Implement MINIX4 5-layer architecture consistently")
    print(f"   • Review large directories for better organization")
    
    # Generate specific cleanup scripts recommendations
    print(f"\n💡 RECOMMENDED CLEANUP APPROACH:")
    print(f"\nPhase 1: Safe cleanup (no risk)")
    print(f"   1. Remove obvious temporary/backup files")
    print(f"   2. Remove empty duplicate files")
    print(f"   3. Remove build artifacts from source")
    
    print(f"\nPhase 2: Duplicate consolidation") 
    print(f"   1. Keep files in preferred locations (minix4/ > minix/ > archive/)")
    print(f"   2. Remove duplicates from legacy/archive locations")
    print(f"   3. Update any references/includes")
    
    print(f"\nPhase 3: Architecture alignment")
    print(f"   1. Move files to correct layers")
    print(f"   2. Consolidate similar functionality")
    print(f"   3. Update build system")

def main():
    report_file = Path("scripts/analysis/deep_scan_report.json")
    
    if not report_file.exists():
        print(f"Error: Report file not found: {report_file}")
        print("Please run deep_directory_scanner.py first")
        return
    
    analyze_deep_scan_report(report_file)

if __name__ == "__main__":
    main()
