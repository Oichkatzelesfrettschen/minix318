#!/usr/bin/env python3
"""Locate all repository files mentioning 'ACPI'.

This utility helps developers collect paths of ACPI-related files so the
subsystem can be analyzed and unified. Paths are printed relative to the
repository root.

The script uses standard :mod:`pathlib` traversal and a case-insensitive
regular expression to select matching files.
"""
from __future__ import annotations

import re
from pathlib import Path
import sys


def find_acpi_files(root: Path) -> list[Path]:
    """Return files with *acpi* in their path.

    Parameters
    ----------
    root:
        Repository directory to scan.

    Returns
    -------
    list[Path]
        All files whose path contains the substring ``acpi`` ignoring case.
    """
    pattern = re.compile("acpi", re.IGNORECASE)
    return [p for p in root.rglob("*") if p.is_file() and pattern.search(str(p))]


def main() -> None:
    """CLI entry point.

    The function prints one path per line and gracefully handles broken pipe
    errors so that callers can pipe the output to other utilities.
    """
    repo_root = Path(__file__).resolve().parents[1]
    try:
        for path in sorted(find_acpi_files(repo_root)):
            print(path.relative_to(repo_root))
    except BrokenPipeError:
        sys.exit(0)


if __name__ == "__main__":
    main()
