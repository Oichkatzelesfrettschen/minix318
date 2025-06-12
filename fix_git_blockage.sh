#!/bin/bash

echo "=== Fixing Git Blockages ==="

# Remove lock files
echo "Removing git lock files..."
rm -f .git/index.lock
rm -f .git/refs/heads/*.lock
rm -f .git/config.lock
rm -f .git/HEAD.lock

# Clean rebase state
echo "Cleaning rebase state..."
rm -rf .git/rebase-merge
rm -rf .git/rebase-apply
rm -f .git/REBASE_HEAD
rm -f .git/ORIG_HEAD

# Clean merge state
echo "Cleaning merge state..."
rm -f .git/MERGE_HEAD
rm -f .git/MERGE_MSG
rm -f .git/MERGE_MODE

# Clean cherry-pick state
echo "Cleaning cherry-pick state..."
rm -f .git/CHERRY_PICK_HEAD
rm -f .git/sequencer

echo ""
echo "Testing git status..."
git status --short

echo ""
echo "Git should now work normally!"
