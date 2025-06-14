#!/usr/bin/env sh
# map_libc_usage.sh - generate a comprehensive mapping of libc functions to files referencing them.
#
# This script enumerates functions defined in the libc source tree using ctags
# and searches the entire MINIX tree for occurrences of each function name.
# The resulting list is written to 'libc_usage_map.txt'.
#
# The optional '--limit N' argument restricts the number of result paths stored
# per function. When omitted all matches are captured.

set -eu

limit=""

# parse command line arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --limit)
            [ $# -ge 2 ] || { echo "Missing value for --limit" >&2; exit 1; }
            limit="$2"; shift 2 ;;
        *)
            echo "Usage: $0 [--limit N]" >&2
            exit 1 ;;
    esac
done

# gather unique function names from libc using ctags
generate_function_list() {
    ctags -R -x --c-kinds=fp minix/lib/libc \
        | awk '{print $1}' \
        | sort -u > /tmp/libc_functions.txt
}

# map each function to the files that reference it
write_mapping() {
    : > libc_usage_map.txt
    while IFS= read -r func; do
        printf '%s:\n' "$func" >> libc_usage_map.txt
        if [ -n "$limit" ]; then
            grep -Rnl --exclude='*.o' --exclude='*.a' --exclude-dir='build' \
                --no-messages "\\b$func\\b" minix \
                | head -n "$limit" >> libc_usage_map.txt || true
        else
            grep -Rnl --exclude='*.o' --exclude='*.a' --exclude-dir='build' \
                --no-messages "\\b$func\\b" minix \
                >> libc_usage_map.txt || true
        fi
        printf '\n' >> libc_usage_map.txt
    done < /tmp/libc_functions.txt
}

generate_function_list
write_mapping

printf 'Mapping written to libc_usage_map.txt\n'
