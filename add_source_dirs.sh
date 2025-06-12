#!/bin/bash

echo "Adding source directories to git..."

echo "Checking limine_src status..."
if [ -d "minix/limine_src/.git" ]; then
    echo "! limine_src contains a git repository"
    echo "  Options:"
    echo "  1. Add as submodule: git submodule add <url> minix/limine_src"
    echo "  2. Remove .git and add files: rm -rf minix/limine_src/.git && git add minix/limine_src/"
    echo "  3. Add .gitkeep file: echo '' > minix/limine_src/.gitkeep && git add minix/limine_src/.gitkeep"
    
    read -p "Remove .git directory and add files? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Removing nested .git directory..."
        rm -rf minix/limine_src/.git
        echo "Adding limine_src files..."
        git add minix/limine_src/
    else
        echo "Skipping limine_src"
    fi
else
    echo "Adding limine_src..."
    git add minix/limine_src/
fi

echo ""
echo "Checking microwindows_src status..."
if [ -d "minix/microwindows_src/.git" ]; then
    echo "! microwindows_src contains a git repository"
    read -p "Remove .git directory and add files? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Removing nested .git directory..."
        rm -rf minix/microwindows_src/.git
        echo "Adding microwindows_src files..."
        git add minix/microwindows_src/
    else
        echo "Skipping microwindows_src"
    fi
else
    echo "Adding microwindows_src..."
    git add minix/microwindows_src/
fi

echo ""
echo "Checking what was added..."
git status --short | grep -E "(limine_src|microwindows_src)" | head -10

total_added=$(git status --short | grep -E "(limine_src|microwindows_src)" | wc -l)
if [ $total_added -gt 10 ]; then
    echo "... and $(( $total_added - 10 )) more files"
fi

echo ""
echo "Ready to commit!"
