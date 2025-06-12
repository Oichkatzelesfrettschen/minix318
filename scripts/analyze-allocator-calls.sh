#!/bin/bash
echo "Analyzing allocator call patterns..."
OUTPUT_CSV="allocator-call-counts.csv"
INPUT_FILE="all-allocators.txt"

# Check if input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: $INPUT_FILE not found. Please run Task 1.2 first."
    exit 1
fi

# Check if input file is empty
if [ ! -s "$INPUT_FILE" ]; then
    echo "Warning: $INPUT_FILE is empty. No files to analyze."
    echo "file,malloc_calls,calloc_calls,realloc_calls,free_calls,kalloc_calls,kfree_calls,other_alloc_calls,other_free_calls" > "$OUTPUT_CSV"
    exit 0
fi

echo "file,malloc_calls,calloc_calls,realloc_calls,free_calls,kalloc_calls,kfree_calls,other_alloc_calls,other_free_calls" > "$OUTPUT_CSV"

while IFS= read -r file_path; do
    if [ -f "$file_path" ]; then
        # More specific grep patterns for function calls.
        # \b for word boundaries, \( to match the parenthesis.
        malloc_calls=$(grep -c -E '\bmalloc\s*\(' "$file_path")
        calloc_calls=$(grep -c -E '\bcalloc\s*\(' "$file_path")
        realloc_calls=$(grep -c -E '\brealloc\s*\(' "$file_path")
        free_calls=$(grep -c -E '\bfree\s*\(' "$file_path")

        # Kernel specific or common alternatives (extend as needed)
        kalloc_calls=$(grep -c -E '\bkalloc\s*\(' "$file_path")
        kfree_calls=$(grep -c -E '\bkfree\s*\(' "$file_path")

        # Broader "alloc" and "free" for other cases, excluding the specific ones already counted.
        # This is tricky with grep -c. A more complex awk script might be better for non-overlapping counts.
        # For simplicity, this grep will still count lines, so if a line has "malloc" and "other_alloc", it's counted by both.
        # This is a known limitation of this simple script.
        other_alloc_calls=$(grep -E '\balloc' "$file_path" | grep -v -E '\bmalloc\s*\(|\bcalloc\s*\(|\brealloc\s*\(|\bkalloc\s*\(' | wc -l)
        other_free_calls=$(grep -E '\bfree' "$file_path" | grep -v -E '\bfree\s*\(|\bkfree\s*\(' | wc -l)

        echo ""$file_path",$malloc_calls,$calloc_calls,$realloc_calls,$free_calls,$kalloc_calls,$kfree_calls,$other_alloc_calls,$other_free_calls" >> "$OUTPUT_CSV"
    else
        echo "Warning: File not found during analysis: $file_path"
    fi
done < "$INPUT_FILE"

echo "Analysis complete. Output in $OUTPUT_CSV"
