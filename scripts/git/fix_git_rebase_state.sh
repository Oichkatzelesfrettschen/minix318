#!/bin/bash

echo "=== Fixing corrupted git rebase state ==="

# Remove corrupted rebase state
echo "Removing corrupted rebase metadata..."
rm -rf .git/rebase-merge 2>/dev/null || echo "No rebase-merge directory found"
rm -rf .git/rebase-apply 2>/dev/null || echo "No rebase-apply directory found"

# Remove other rebase-related files
echo "Cleaning up rebase state files..."
rm -f .git/REBASE_HEAD 2>/dev/null || echo "No REBASE_HEAD file"
rm -f .git/ORIG_HEAD 2>/dev/null || echo "No ORIG_HEAD file"

# Check current git status
echo ""
echo "=== Current git status ==="
git status --porcelain

echo ""
echo "=== Current branch ==="
git branch --show-current

echo ""
echo "=== Recent commits ==="
git log --oneline -5

echo ""
echo "Git rebase state has been cleaned up!"
echo "You should now be able to use git normally."
