#!/usr/bin/env python3
"""Locate all repository files mentioning 'ACPI'.

This utility helps developers collect paths of ACPI-related files so the
subsystem can be analyzed and unified. Paths are printed relative to the
repository root.
"""
from __future__ import annotations

import re
from pathlib import Path
import sys


def find_acpi_files(root: Path) -> list[Path]:
    """Return a list of files whose path contains 'acpi' case-insensitively."""
    pattern = re.compile("acpi", re.IGNORECASE)
    return [p for p in root.rglob("*") if p.is_file() and pattern.search(str(p))]


def main() -> None:
    """Print ACPI-related file paths."""
    repo_root = Path(__file__).resolve().parents[1]
    try:
        for path in sorted(find_acpi_files(repo_root)):
            print(path.relative_to(repo_root))
    except BrokenPipeError:
        sys.exit(0)


if __name__ == "__main__":
    main()
