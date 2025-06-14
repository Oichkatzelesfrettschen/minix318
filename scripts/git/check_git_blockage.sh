#!/bin/bash

echo "=== Git Blockage Diagnostic ==="

echo "1. Checking git repository state..."
if [ -d ".git" ]; then
    echo "✓ Git repository detected"
else
    echo "✗ No .git directory found"
    exit 1
fi

echo ""
echo "2. Checking for problematic git state files..."
if [ -d ".git/rebase-merge" ]; then
    echo "! Found .git/rebase-merge directory"
    ls -la .git/rebase-merge/
fi

if [ -d ".git/rebase-apply" ]; then
    echo "! Found .git/rebase-apply directory"
    ls -la .git/rebase-apply/
fi

if [ -f ".git/MERGE_HEAD" ]; then
    echo "! Found .git/MERGE_HEAD - repository in merge state"
fi

if [ -f ".git/REBASE_HEAD" ]; then
    echo "! Found .git/REBASE_HEAD - repository in rebase state"
fi

if [ -f ".git/CHERRY_PICK_HEAD" ]; then
    echo "! Found .git/CHERRY_PICK_HEAD - repository in cherry-pick state"
fi

echo ""
echo "3. Checking git index status..."
if [ -f ".git/index" ]; then
    echo "✓ Git index exists"
else
    echo "✗ Git index missing"
fi

if [ -f ".git/index.lock" ]; then
    echo "! Git index is locked (.git/index.lock exists)"
    echo "  This will block all git operations"
fi

echo ""
echo "4. Testing basic git commands..."
echo "Git status test:"
git status --porcelain 2>&1 | head -3

echo ""
echo "Git branch test:"
git branch --show-current 2>&1

echo ""
echo "=== Quick Fix Recommendations ==="
if [ -f ".git/index.lock" ]; then
    echo "Remove lock file: rm -f .git/index.lock"
fi

if [ -d ".git/rebase-merge" ] || [ -d ".git/rebase-apply" ]; then
    echo "Clean rebase state: rm -rf .git/rebase-merge .git/rebase-apply"
fi

if [ -f ".git/REBASE_HEAD" ]; then
    echo "Remove rebase head: rm -f .git/REBASE_HEAD"
fi
