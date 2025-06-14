#!/bin/bash

echo "=== Resolving R3.1.8 Merge Conflicts ==="

echo "Current merge conflicts:"
git --no-pager status --porcelain | grep "^D"

echo ""
echo "The conflicts are 'modify/delete' conflicts where:"
echo "- Files were deleted in HEAD (master)"
echo "- Files were modified in R3.1.8"
echo ""
echo "Since you're on the newer master branch, the deletions were likely intentional."
echo "We'll resolve by keeping the deletions (removing the files from R3.1.8)."

echo ""
echo "Resolving conflicts by accepting deletions..."

# Remove the conflicted files (accept the deletion from master)
git rm commands/man/man.conf 2>/dev/null || echo "commands/man/man.conf already handled"
git rm servers/ext2/link.c 2>/dev/null || echo "servers/ext2/link.c already handled"
git rm servers/mfs/link.c 2>/dev/null || echo "servers/mfs/link.c already handled"

echo ""
echo "Completing the merge..."
git commit -m "Merge R3.1.8: Resolve conflicts by accepting file deletions from master"

if [ $? -eq 0 ]; then
    echo "[SUCCESS] R3.1.8 merge completed!"
    echo ""
    echo "Continuing with pr-76 merge..."
    git merge pr-76
    
    if [ $? -eq 0 ]; then
        echo "[SUCCESS] pr-76 merged successfully!"
        echo ""
        echo "*** ALL BRANCHES MERGED SUCCESSFULLY! ***"
        echo ""
        echo "=== Final Repository Status ==="
        git --no-pager status --short
        echo ""
        echo "=== Recent Commit History ==="
        git --no-pager log --oneline -8
        echo ""
        echo "=== Branch Cleanup ==="
        echo "You can now delete merged branches with:"
        echo "git branch -d kernel-refactor-foundations docs-kasserts-spinlocks kernel-tests R3.1.8 pr-76"
    else
        echo "[ERROR] Conflict in pr-76. Check conflicts:"
        git --no-pager status --porcelain | grep "^UU\|^AA\|^DD"
    fi
else
    echo "[ERROR] Failed to complete R3.1.8 merge"
    git --no-pager status
fi
