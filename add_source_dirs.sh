#!/bin/bash

echo "Adding source directories to git..."

echo "Adding limine_src..."
git add minix/limine_src/

echo "Adding microwindows_src..."
git add minix/microwindows_src/

echo "Checking what was added..."
git status --short | grep -E "(limine_src|microwindows_src)" | head -10

echo ""
echo "Ready to commit these directories!"
echo "Use: git commit -m \"Add limine and microwindows source directories\""
