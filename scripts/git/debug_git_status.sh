#!/bin/bash

echo "=== Git Process Diagnostic ==="
echo "Checking for running git processes..."
ps aux | grep git | grep -v grep

echo -e "\n=== Git Lock File Status ==="
if [ -f ".git/index.lock" ]; then
    echo "Found .git/index.lock file:"
    ls -la .git/index.lock
    echo "Lock file age:"
    stat -f "%Sm" .git/index.lock
    echo "Removing stale lock file..."
    rm -f .git/index.lock
    echo "Lock file removed."
else
    echo "No .git/index.lock file found."
fi

echo -e "\n=== Checking for other git lock files ==="
find .git -name "*.lock" -type f 2>/dev/null

echo -e "\n=== Git Status ==="
git status --porcelain

echo -e "\n=== Git Repository State ==="
if [ -f ".git/MERGE_HEAD" ]; then
    echo "Repository is in MERGE state"
fi
if [ -f ".git/CHERRY_PICK_HEAD" ]; then
    echo "Repository is in CHERRY-PICK state"
fi
if [ -f ".git/REVERT_HEAD" ]; then
    echo "Repository is in REVERT state"
fi
if [ -f ".git/REBASE_HEAD" ]; then
    echo "Repository is in REBASE state"
fi

echo -e "\n=== Attempting to reset git state ==="
# Kill any hanging git processes
pkill -f "git.*minix318" 2>/dev/null || echo "No git processes to kill"

# Remove any remaining lock files
find .git -name "*.lock" -type f -delete 2>/dev/null

# Try to add files again
echo "Attempting git add..."
git add -A .

echo -e "\n=== Final git status ==="
git status --short
