#!/bin/bash
# Script for Initial Codebase Scan & Metric Generation

echo "Finding all C files potentially involved in memory allocation..."
find . -path ./build -prune -o -path ./dist -prune -o -path ./third_party -prune -o -type f \( -name "*alloc.c" -o -name "*malloc.c" -o -name "*alloc*.h" -o -name "*malloc*.h" \) -print > data/all_allocators.txt
echo "Found $(wc -l < data/all_allocators.txt) potential allocator files."

echo "Generating Lines of Code (LOC) metrics..."
if [ -s data/all_allocators.txt ]; then
    xargs cloc --by-file --csv < data/all_allocators.txt > data/loc_metrics.csv
    echo "LOC metrics generated in data/loc_metrics.csv"
else
    echo "No files found to analyze for LOC metrics, skipping cloc."
    echo "language,filename,blank,comment,code" > data/loc_metrics.csv # Create empty csv with header
fi

echo "Generating allocator call counts..."
echo "file,alloc_calls,malloc_calls,calloc_calls,realloc_calls,free_calls,total_calls" > data/allocator_call_counts.csv
echo "" > data/raw_allocation_sites.txt
if [ -s data/all_allocators.txt ]; then
    while IFS= read -r file_path; do
        if [ -f "$file_path" ]; then
            alloc_calls=$(grep -Ewci "\b(alloc|xalloc|kalloc|valloc|palloc)\b" "$file_path")
            malloc_calls=$(grep -Ewci "\b(malloc|xmalloc|kmalloc|vmalloc)\b" "$file_path")
            calloc_calls=$(grep -Ewci "\b(calloc|xcalloc|kcalloc)\b" "$file_path")
            realloc_calls=$(grep -Ewci "\b(realloc|xrealloc|krealloc)\b" "$file_path")
            free_calls=$(grep -Ewci "\b(free|xfree|kfree|vfree)\b" "$file_path")
            total_calls=$((alloc_calls + malloc_calls + calloc_calls + realloc_calls + free_calls))
            echo "$file_path,$alloc_calls,$malloc_calls,$calloc_calls,$realloc_calls,$free_calls,$total_calls" >> data/allocator_call_counts.csv
            grep -Enh "\b(alloc|malloc|calloc|realloc|free|xalloc|kalloc|valloc|palloc|xmalloc|kmalloc|vmalloc|xcalloc|kcalloc|xrealloc|krealloc|xfree|kfree|vfree)\b" "$file_path" >> data/raw_allocation_sites.txt
        fi
    done < data/all_allocators.txt
    echo "Allocator call counts generated in data/allocator_call_counts.csv"
    echo "Raw allocation sites extracted to data/raw_allocation_sites.txt"
else
    echo "No files found to analyze for call counts."
fi
