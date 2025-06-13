#!/bin/bash

echo "=== Complete ASCII Tree of Files with 'alloc' in filename ==="
echo ""

# Function to create tree structure
create_tree() {
    find . -type f -iname "*alloc*" | sort | while read file; do
        # Remove leading ./
        clean_path=${file#./}
        
        # Split into directory and filename
        dir=$(dirname "$clean_path")
        filename=$(basename "$clean_path")
        
        # If in root directory
        if [ "$dir" = "." ]; then
            echo "├── 📄 $filename"
        else
            # Show directory structure
            echo "$dir" | tr '/' '\n' | awk -v filename="$filename" '
            {
                for(i=1; i<NR; i++) print "│   " 
                if(NR==1) print "📁 " $0 "/"
                else print "│   📁 " $0 "/"
            }
            END {
                for(i=1; i<NR; i++) printf "│   "
                print "│   ├── 📄 " filename
            }'
        fi
    done
}

# Create the tree
echo "Repository Root (.)"
create_tree | sort -u

echo ""
echo "=== Files by Directory ==="
echo ""

# Group files by directory
find . -type f -iname "*alloc*" | while read file; do
    dir=$(dirname "$file")
    filename=$(basename "$file")
    echo "$dir|$filename"
done | sort | awk -F'|' '
BEGIN { prev_dir = "" }
{
    if ($1 != prev_dir) {
        if (prev_dir != "") print ""
        print "📁 " $1 "/"
        prev_dir = $1
    }
    print "  ├── 📄 " $2
}'

echo ""
echo "=== File Contents Preview ==="
echo ""

# Show content previews
find . -type f -iname "*alloc*" | sort | while read file; do
    echo "┌── 📄 $file"
    echo "│"
    if [ -f "$file" ]; then
        lines=$(wc -l < "$file")
        bytes=$(wc -c < "$file")
        echo "│   📊 Stats: $lines lines, $bytes bytes"
        
        if file "$file" | grep -q "text"; then
            echo "│   📝 Preview:"
            head -5 "$file" 2>/dev/null | sed 's/^/│     /'
        else
            echo "│   🔒 Binary file"
        fi
    else
        echo "│   ❌ File not accessible"
    fi
    echo "└──"
    echo ""
done
