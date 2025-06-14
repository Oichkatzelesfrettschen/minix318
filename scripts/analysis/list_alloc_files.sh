#!/bin/bash

echo "Files containing 'alloc' in filename:"
echo ""

find . -type f -iname "*alloc*" | sort | sed 's|^\./||'
