#!/bin/bash

echo "=== Files containing 'alloc' in filename ==="
echo "Searching recursively through the entire repository..."
echo ""

# Find all files with 'alloc' in filename (case insensitive)
alloc_files=$(find . -type f -iname "*alloc*" | sort)

if [ -z "$alloc_files" ]; then
    echo "No files found with 'alloc' in filename."
    exit 0
fi

echo "Found $(echo "$alloc_files" | wc -l) files:"
echo ""

# Create ASCII tree structure
echo "$alloc_files" | while read file; do
    # Remove leading ./
    clean_path=${file#./}
    
    # Split path into components
    IFS='/' read -ra PATH_PARTS <<< "$clean_path"
    
    # Calculate depth
    depth=${#PATH_PARTS[@]}
    
    # Create indentation
    indent=""
    for ((i=1; i<depth; i++)); do
        indent+="│   "
    done
    
    # Get filename
    filename=${PATH_PARTS[-1]}
    
    # Display with tree structure
    if [ $depth -eq 1 ]; then
        echo "├── $filename"
    else
        echo "${indent}├── $filename"
    fi
    
    # Show file details
    if [ -f "$file" ]; then
        size=$(wc -l < "$file" 2>/dev/null || echo "0")
        echo "${indent}│   └── ($size lines)"
    fi
done

echo ""
echo "=== Detailed File Information ==="
echo ""

# Show detailed information for each file
echo "$alloc_files" | while read file; do
    if [ -f "$file" ]; then
        echo "File: $file"
        echo "  Size: $(wc -l < "$file") lines, $(wc -c < "$file") bytes"
        echo "  Type: $(file "$file" | cut -d: -f2-)"
        
        # Show first few lines if it's a text file
        if file "$file" | grep -q "text"; then
            echo "  Preview:"
            head -3 "$file" 2>/dev/null | sed 's/^/    /'
        fi
        echo ""
    fi
done

echo "=== Summary ==="
echo "Total files with 'alloc' in name: $(echo "$alloc_files" | wc -l)"
echo "File types found:"
echo "$alloc_files" | xargs file | cut -d: -f2- | sort | uniq -c | sed 's/^/  /'
