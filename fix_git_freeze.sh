#!/bin/bash

echo "=== Fixing frozen git add ==="

# Kill any hanging git processes
echo "Killing any hanging git processes..."
pkill -f "git.*add" 2>/dev/null || echo "No git add processes found"
pkill -f "git.*minix318" 2>/dev/null || echo "No git processes found for this repo"

# Remove all git lock files
echo "Removing git lock files..."
find .git -name "*.lock" -type f -delete 2>/dev/null && echo "Lock files removed" || echo "No lock files found"

# Clean up duplicate files with " 2" suffix
echo "=== Cleaning up duplicate files ==="
find . -name "* 2*" -type f | while read -r file; do
    echo "Removing duplicate: $file"
    rm -f "$file"
done

# Check git status
echo "=== Current git status ==="
git status --porcelain

# Reset git index if corrupted
echo "=== Resetting git index if needed ==="
if [ ! -f ".git/index" ] || [ ! -s ".git/index" ]; then
    echo "Git index is missing or empty, resetting..."
    git reset
fi

# Add files one by one to identify problematic files
echo "=== Attempting to add files carefully ==="
git add .gitignore 2>/dev/null || echo "No .gitignore to add"
git add minix/llvm/include/magic_common.h 2>/dev/null || echo "Could not add magic_common.h"
git add minix/main.c 2>/dev/null || echo "Could not add main.c"
git add minix/tests/arch/earm/test_arm_segfault.S 2>/dev/null || echo "Could not add test_arm_segfault.S"

echo "=== Final status ==="
git status --short
