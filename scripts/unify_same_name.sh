#!/bin/bash
# unify_same_name.sh - unify duplicate files that share the same basename.
#
# This script identifies duplicate basenames within the current git
# repository tree. It groups files with the same name and checks if they
# are byte-for-byte identical using fdupes. Identical copies are listed
# for deletion. Divergent copies launch meld for manual merging.
#
# Dependencies: fd (or fdfind), ripgrep, fdupes, diffutils, meld, jq,
# and moreutils.

set -euo pipefail
shopt -s globstar nullglob

# Step 0: ensure 'fd' is available under the expected name
command -v fd >/dev/null 2>&1 || sudo ln -sf "$(command -v fdfind)" /usr/local/bin/fd

# Step 1: gather basenames appearing more than once
mapfile -d '' repeated_basenames < <(
	fd --type f -0 | xargs -0 -n1 basename -a |
		sort -z | uniq -dz
)

# Step 2: process each basename
for base in "${repeated_basenames[@]}"; do
	printf "\n\e[1;36m» Group: %s\e[0m\n" "$base"

	# Step 2a: collect all paths with this basename
	mapfile -d '' paths < <(
		fd --type f -0 -c never -a -t f --exact-depth 0 -x echo -n {} -0 |
			grep -z "/${base}$"
	)

	# Step 2b: if all files are identical, suggest deletion
	if fdupes -r1 -q "${paths[@]}" >/dev/null; then
		echo "  ✔ All duplicates are binary-identical – you may safely delete extras:"
		printf "    %s\n" "${paths[@]:1}"
		continue
	fi

	# Step 2c: launch meld for divergent copies
	echo "  ⚡ Divergent copies detected – launching meld for synthesis…"
	if ((${#paths[@]} <= 3)); then
		meld "${paths[@]}"
	else
		for ((i = 0; i < ${#paths[@]}; i += 3)); do
			meld "${paths[@]:i:3}" &
		done
	fi
done
