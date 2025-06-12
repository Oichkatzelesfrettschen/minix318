#!/usr/bin/env sh
## gen_acpi_tags.sh - generate ctags and cscope databases for ACPI sources.
#
# Collect all files whose paths contain ``acpi`` and build tag databases.
# The script relies on ``universal-ctags`` and ``cscope`` which are installed
# via :file:`setup.sh`.
#
# Usage:
#   ``scripts/gen_acpi_tags.sh``

set -e

script_dir="$(dirname "$0")"
repo_root="$(realpath "$script_dir/..")"

# Locate ACPI-related files.
file_list="$(mktemp)"
python3 "$script_dir/acpi_files.py" >"$file_list"

# Generate ctags database.
ctags -L "$file_list" -f "$repo_root/acpi.tags"

# Generate cscope database.
cscope -b -q -k -i "$file_list" -f "$repo_root/acpi.out"

rm "$file_list"
