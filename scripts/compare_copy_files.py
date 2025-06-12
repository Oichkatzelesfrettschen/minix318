#!/usr/bin/env python3
"""Compare ``* copy.*`` files with their originals.

The utility scans the repository for files whose names contain
``' copy.'`` and compares each to the file with the same path but
without that substring.  A brief diff preview is printed when files
are not identical.  The script helps developers locate historical
duplicates that might be consolidated.
"""

from __future__ import annotations

import argparse
import difflib
from pathlib import Path


def find_copy_files(root: Path) -> list[Path]:
    """Return a list of all files whose name contains ' copy.'."""
    return list(root.rglob("* copy.*"))


def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Compare '* copy.*' files with their originals"
    )
    parser.add_argument(
        "--diff-lines",
        type=int,
        default=10,
        help="Number of diff lines to show when files differ",
    )
    return parser.parse_args()


def compare_files(copy_path: Path, original_path: Path, diff_lines: int) -> None:
    """Print comparison results for a pair of files."""
    print(f"\nComparing {copy_path} -> {original_path}")
    if not original_path.exists():
        print("Original file missing")
        return

    copy_content = copy_path.read_text(errors="ignore").splitlines()
    orig_content = original_path.read_text(errors="ignore").splitlines()

    if copy_content == orig_content:
        print("IDENTICAL")
    else:
        print("DIFFERENT")
        diff = difflib.unified_diff(
            orig_content,
            copy_content,
            fromfile=str(original_path),
            tofile=str(copy_path),
            lineterm="",
        )
        for line in list(diff)[:diff_lines]:
            print(line)
        if len(copy_content) != len(orig_content):
            print(f"Line count original={len(orig_content)} copy={len(copy_content)}")


def main() -> None:
    """Execute the comparison across the repository."""
    args = parse_args()
    root = Path(__file__).resolve().parents[1]
    for copy_file in find_copy_files(root):
        original_file = Path(str(copy_file).replace(" copy", ""))
        compare_files(copy_file, original_file, args.diff_lines)


if __name__ == "__main__":
    main()
