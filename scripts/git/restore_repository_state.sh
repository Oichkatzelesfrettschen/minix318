#!/bin/bash

echo "=== Restoring MINIX 3.1.8 Repository State ==="

# Fix git rebase state
echo "Step 1: Fixing git rebase state..."
./fix_git_rebase_state.sh

# Check if magic_common.h is empty or missing
echo ""
echo "Step 2: Checking Magic framework files..."
if [ ! -s "minix/llvm/include/magic_common.h" ]; then
    echo "magic_common.h is empty or missing - needs restoration"
else
    echo "magic_common.h appears to be present"
fi

# Add and commit any restored files
echo ""
echo "Step 3: Committing restored files..."
git add .
git status --short

if git diff --cached --quiet; then
    echo "No changes to commit"
else
    git commit -m "Restore Magic framework and fix git rebase state"
    echo "Files restored and committed"
fi

echo ""
echo "=== Repository Restoration Complete ==="
echo "Current status:"
git status --short

echo ""
echo "Recent commit history:"
git log --oneline -3

echo ""
echo "All branches:"
git branch -a

echo ""
echo "Repository is now in a clean state!"
