#!/usr/bin/env python3
"""Generate a report of the repository file tree with file types."""

from __future__ import annotations

import argparse
import os
from typing import Iterable, Tuple


def scan_tree(start: str) -> Iterable[Tuple[str, str]]:
    """Yield each filesystem entry path and a human-readable type."""
    for root, dirs, files in os.walk(start):
        for name in dirs:
            path = os.path.join(root, name)
            yield path, "directory"
        for name in files:
            path = os.path.join(root, name)
            if os.path.islink(path):
                yield path, "symlink"
            else:
                yield path, "file"


def main() -> None:
    """Parse command line arguments and print the file tree report."""
    parser = argparse.ArgumentParser(
        description="Generate a file tree report with file types"
    )
    parser.add_argument(
        "start",
        nargs="?",
        default=".",
        help="directory to start scanning (default: current directory)",
    )
    args = parser.parse_args()

    for path, ftype in scan_tree(args.start):
        print(f"{path}\t{ftype}")


if __name__ == "__main__":
    main()
