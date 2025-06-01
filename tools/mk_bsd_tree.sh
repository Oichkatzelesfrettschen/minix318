#!/usr/bin/env bash
# Generate a tree view of the repository marking paths containing BSD references.
# The output is saved as bsd_tree.txt in the repository root.

set -euo pipefail

# Determine repository root based on the script location.
repo_root="$(git -C "$(dirname "$0")/.." rev-parse --show-toplevel)"
cd "$repo_root"

# Check if a path or any of its descendant files mention BSD or NetBSD.
has_bsd() {
  local path="$1"
  grep -RIl --exclude-dir=.git --exclude=bsd_tree.txt \
    -e 'BSD' -e 'NetBSD' "$path" >/dev/null 2>&1
}

# Recursively print the tree. Arguments: path indent is_last
print_tree() {
  local path="$1"
  local indent="$2"
  local is_last="$3"

  local name="$(basename "$path")"
  local branch
  local child_indent
  if [ "$is_last" -eq 1 ]; then
    branch="└──"
    child_indent="${indent}    "
  else
    branch="├──"
    child_indent="${indent}│   "
  fi

  local mark="[no BSD]"
  if has_bsd "$path"; then
    mark="[BSD]"
  fi

  echo "${indent}${branch} ${name} ${mark}"

  if [ -d "$path" ]; then
    local entries
    mapfile -t entries < <(ls -A "$path" | \
      grep -v '^\.git$' | grep -v '^bsd_tree\.txt$' | sort)
    local total=${#entries[@]}
    local index=0
    for entry in "${entries[@]}"; do
      index=$((index + 1))
      local last=0
      if [ $index -eq $total ]; then
        last=1
      fi
      print_tree "$path/$entry" "$child_indent" "$last"
    done
  fi
}

# Generate the tree starting at repository root.
mark_root="[no BSD]"
if has_bsd .; then
  mark_root="[BSD]"
fi

echo ". ${mark_root}" > bsd_tree.txt

entries=( $(ls -A | grep -v '^\.git$' | grep -v '^bsd_tree\.txt$' | sort) )
count=${#entries[@]}
idx=0
for entry in "${entries[@]}"; do
  idx=$((idx + 1))
  last=0
  if [ $idx -eq $count ]; then
    last=1
  fi
  print_tree "$entry" "" "$last" >> bsd_tree.txt
done
