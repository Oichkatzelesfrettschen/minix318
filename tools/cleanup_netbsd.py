#!/usr/bin/env python3
"""Remove lines containing 'NetBSD' from source files.

This script recursively scans a directory, edits text files in place to
strip any lines that mention 'NetBSD'. Binary files are skipped.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path
from typing import Iterable


def iter_source_files(start: Path) -> Iterable[Path]:
    """Yield all regular files under *start* that appear to be text."""
    for path in start.rglob("*"):
        # Skip anything that is not a regular file.
        if not path.is_file():
            continue

        try:
            # Read a small chunk to detect binary data.
            chunk = path.open("rb").read(1024)
            if b"\0" in chunk:
                # Binary files are ignored.
                continue
        except OSError:
            # Ignore files we cannot access.
            continue

        # Yield the text-like file path back to the caller.
        yield path


def strip_netbsd_lines(path: Path) -> None:
    """Remove any line containing 'NetBSD' from *path*."""
    try:
        # Attempt to read the file as UTF-8 text.
        content = path.read_text(encoding="utf-8")
    except (UnicodeDecodeError, OSError):
        # Skip files that cannot be decoded or accessed.
        return

    # Filter out lines mentioning NetBSD and write the result back.
    new_lines = [line for line in content.splitlines() if "NetBSD" not in line]
    path.write_text("\n".join(new_lines) + "\n", encoding="utf-8")


def main() -> None:
    """Parse arguments and clean files."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "root",
        nargs="?",
        default=".",
        help="Root directory to scan",
    )
    args = parser.parse_args()

    # Walk the tree and clean each text file.
    for path in iter_source_files(Path(args.root)):
        strip_netbsd_lines(path)


if __name__ == "__main__":
    main()
