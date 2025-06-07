#!/usr/bin/env sh
# map_libc_usage.sh - generate a simple mapping of libc functions to files that reference them.
#
# This script collects function names defined in the libc source tree and searches the
# entire MINIX tree for files that mention those functions. The output is written to
# 'libc_usage_map.txt'. Only the first few matches for each function are recorded to keep
# the file size reasonable.

set -e

# Extract unique function names from the libc sources using ctags.
ctags -R -x --c-kinds=fp minix/lib/libc |
	awk '{print $1}' |
	sort -u \
		>/tmp/libc_functions.txt

# Truncate or create the output file.
: >libc_usage_map.txt

# Search for each function name throughout the source tree.
while IFS= read -r func; do
	# Record the function name as a header.
	printf '%s:\n' "$func" >>libc_usage_map.txt
	# Find up to five locations where the function name appears.
	grep -Rnl --exclude='*.o' --exclude='*.a' --exclude-dir='build' "\\b$func\\b" minix |
		head -n 5 \
			>>libc_usage_map.txt || true
	# Add a blank line between entries.
	printf '\n' >>libc_usage_map.txt
done </tmp/libc_functions.txt

printf 'Mapping written to libc_usage_map.txt\n'
