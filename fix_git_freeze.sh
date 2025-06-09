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
git --no-pager status --porcelain

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
git --no-pager status --short

echo ""
echo "=== Branch Management Analysis ==="
echo "Current branch:"
git --no-pager branch --show-current

echo ""
echo "Local branches:"
git --no-pager branch

echo ""
echo "Remote branches:"
git --no-pager branch -r

echo ""
echo "Detailed branch information:"
echo "Local branches with commit info:"
for branch in $(git --no-pager branch | sed 's/\*//g' | xargs); do
    if [ "$branch" != "master" ]; then
        echo "  $branch:"
        echo "    Last commit: $(git --no-pager log --oneline -1 $branch)"
        echo "    Commits ahead of master: $(git rev-list --count master..$branch)"
        echo "    Commits behind master: $(git rev-list --count $branch..master)"
        echo "    Files changed: $(git --no-pager diff --name-only master..$branch | wc -l | xargs)"
        echo ""
    fi
done

echo "=== STARTING MERGE PROCESS ==="
echo "Step 1: Committing current changes first..."

# First, make sure current changes are committed
git add . 
git commit -m "Pre-merge: Clean up duplicates, fix git issues, and LLVM-20 enhancements"

echo ""
echo "Step 2: Starting with foundation branch merge..."
echo "Merging kernel-refactor-foundations..."

git merge kernel-refactor-foundations

if [ $? -eq 0 ]; then
    echo "[SUCCESS] kernel-refactor-foundations merged successfully!"
    echo ""
    echo "Step 3: Continuing with documentation and testing branches..."
    
    echo "Merging docs-kasserts-spinlocks..."
    git merge docs-kasserts-spinlocks
    
    if [ $? -eq 0 ]; then
        echo "[SUCCESS] docs-kasserts-spinlocks merged successfully!"
        
        echo "Merging kernel-tests..."
        git merge kernel-tests
        
        if [ $? -eq 0 ]; then
            echo "[SUCCESS] kernel-tests merged successfully!"
            
            echo ""
            echo "Step 4: Merging release/PR branches..."
            
            echo "Merging R3.1.8..."
            git merge R3.1.8
            
            if [ $? -eq 0 ]; then
                echo "[SUCCESS] R3.1.8 merged successfully!"
                
                echo "Merging pr-76..."
                git merge pr-76
                
                if [ $? -eq 0 ]; then
                    echo "[SUCCESS] pr-76 merged successfully!"
                    echo ""
                    echo "*** ALL LOCAL BRANCHES MERGED SUCCESSFULLY! ***"
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
                    echo "[ERROR] Conflict in pr-76. Please resolve manually:"
                    git --no-pager status --porcelain | grep "^UU\|^AA\|^DD"
                fi
            else
                echo "[ERROR] Conflict in R3.1.8. Please resolve manually:"
                git --no-pager status --porcelain | grep "^UU\|^AA\|^DD"
            fi
        else
            echo "[ERROR] Conflict in kernel-tests. Please resolve manually:"
            git --no-pager status --porcelain | grep "^UU\|^AA\|^DD"
        fi
    else
        echo "[ERROR] Conflict in docs-kasserts-spinlocks. Please resolve manually:"
        git --no-pager status --porcelain | grep "^UU\|^AA\|^DD"
    fi
else
    echo "[ERROR] Merge conflict detected in kernel-refactor-foundations"
    echo ""
    echo "=== Conflict Resolution Steps ==="
    echo "1. Check conflicted files:"
    echo "   git status"
    echo ""
    echo "2. Edit conflicted files to resolve conflicts"
    echo "3. Stage resolved files:"
    echo "   git add <resolved-files>"
    echo ""
    echo "4. Complete the merge:"
    echo "   git commit"
    echo ""
    echo "5. Then re-run this script or continue with next merges"
    echo ""
    echo "=== Files with conflicts ==="
    git --no-pager status --porcelain | grep "^UU\|^AA\|^DD"
fi
