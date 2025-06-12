#!/bin/bash

echo "=== MINIX 3.1.8 Status Check ==="

# Simple git status
echo "Current branch: $(git branch --show-current)"
echo ""

# Show what's actually changed (cleaner view)
echo "Repository status:"
changed_files=$(git status --porcelain | wc -l)
if [ $changed_files -eq 0 ]; then
    echo "✓ Clean - no changes"
else
    echo "Changed files ($changed_files total):"
    git status --porcelain | head -5
    if [ $changed_files -gt 5 ]; then
        echo "... and $(( $changed_files - 5 )) more"
    fi
fi

echo ""
echo "Key files status:"
if [ -f "minix/llvm/include/magic_common.h" ]; then
    lines=$(wc -l < minix/llvm/include/magic_common.h)
    echo "✓ Magic framework: $lines lines"
else
    echo "✗ Magic framework missing"
fi

if [ -f "minix/main.c" ]; then
    echo "✓ Main kernel file present"
else
    echo "? Main kernel file status unknown"
fi

echo ""
echo "Recent activity:"
git log --oneline -3 | sed 's/^/  /'

echo ""
echo "Available branches:"
git branch | sed 's/^/  /'

echo ""
echo "=== Summary ==="
echo "Repository looks $([ $changed_files -eq 0 ] && echo 'clean' || echo 'ready for commit')"
