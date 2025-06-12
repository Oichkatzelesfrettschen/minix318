#!/bin/bash

# Simple, quiet commit script
if [ $# -eq 0 ]; then
    echo "Usage: ./quiet_commit.sh \"commit message\""
    exit 1
fi

echo "Committing changes..."
git add . > /dev/null 2>&1
git commit -m "$1" > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "✓ Committed: $1"
    echo "Latest: $(git log --oneline -1)"
else
    echo "✗ Commit failed or no changes to commit"
fi
