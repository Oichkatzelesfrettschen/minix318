#!/usr/bin/env python3
"""Report files that share the same name.

The script scans the repository for files with identical basenames.
When matches occur, it reports each pair along with their sizes and
line counts so duplicates can be investigated.
"""

from __future__ import annotations

import os
from collections import defaultdict
from pathlib import Path


def gather_files(root: Path) -> dict[str, list[Path]]:
    """Return a mapping from base name to all paths with that name."""
    mapping: dict[str, list[Path]] = defaultdict(list)
    for path in root.rglob("*"):
        if path.is_file():
            mapping[path.name].append(path)
    return mapping


def describe(path: Path) -> tuple[int, int]:
    """Return size in bytes and line count for ``path``."""
    size = path.stat().st_size
    try:
        lines = sum(1 for _ in path.open("rb"))
    except OSError:
        lines = 0
    return size, lines


def main() -> None:
    """Print duplicate file names with metrics."""
    root = Path(__file__).resolve().parents[1]
    mapping = gather_files(root)
    for name, paths in sorted(mapping.items()):
        if len(paths) < 2:
            continue
        info = [describe(p) for p in paths]
        print(f"\n{name}")
        for p, (size, lines) in zip(paths, info):
            print(f"  {p} size={size} lines={lines}")


if __name__ == "__main__":
    main()
