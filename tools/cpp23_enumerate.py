#!/usr/bin/env python3
"""Enumerate repository files and guess which are C++23 upgraded.

The script walks the directory tree rooted at the repository and prints a
summary of file types. It also inspects C++ source files and uses simple
heuristics to determine whether they likely use C++23 features.
"""

from __future__ import annotations

import argparse
import os
import mimetypes
from dataclasses import dataclass
from typing import Iterator, List


CXX_EXTENSIONS = {".cpp", ".cc", ".cxx", ".C", ".hpp", ".hh", ".hxx"}

CXX23_MARKERS = [
    "consteval",  # keyword introduced in C++20, relevant for 23
    "std::expected",  # library feature from C++23
    "std::print",  # print facility in C++23
    "std::format",  # improved in C++23
    "std::ranges",  # ranges library
    "co_await",  # coroutine keyword
    "#if __cplusplus >= 202302L",  # direct version check
]


@dataclass
class FileInfo:
    """Simple container for file information."""

    path: str
    file_type: str
    upgraded_cpp23: bool = False


def guess_file_type(path: str) -> str:
    """Return a best-effort string describing the file type."""
    mime, _ = mimetypes.guess_type(path)
    return mime or "unknown"


def walk_files(root: str) -> Iterator[str]:
    """Yield all file paths under *root*."""
    for current, _, files in os.walk(root):
        for name in files:
            yield os.path.join(current, name)


def inspect_file(path: str) -> FileInfo:
    """Inspect *path* and return collected information."""
    ext = os.path.splitext(path)[1]
    file_type = guess_file_type(path)
    upgraded = False
    if ext in CXX_EXTENSIONS:
        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as source:
                data = source.read()
        except OSError:
            data = ""
        upgraded = any(marker in data for marker in CXX23_MARKERS)
    return FileInfo(path, file_type, upgraded)


def analyze(root: str) -> tuple[list[FileInfo], list[str], list[str]]:
    """Analyze the directory tree starting at *root*."""
    tree: List[FileInfo] = []
    upgraded: List[str] = []
    not_upgraded: List[str] = []
    for path in walk_files(root):
        info = inspect_file(path)
        tree.append(info)
        if os.path.splitext(path)[1] in CXX_EXTENSIONS:
            (upgraded if info.upgraded_cpp23 else not_upgraded).append(path)
    return tree, upgraded, not_upgraded


def print_summary(
    tree: list[FileInfo], upgraded: list[str], not_upgraded: list[str]
) -> None:
    """Print the collected summary to stdout."""
    print("File tree and types:\n")
    for info in tree:
        print(f"{info.path}: {info.file_type}")
    print("\nC++23 upgraded files:")
    for path in upgraded:
        print(path)
    print("\nFiles not upgraded to C++23:")
    for path in not_upgraded:
        print(path)


def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "root", nargs="?", default=".", help="Root directory to scan"
    )
    return parser.parse_args()


def main() -> None:
    """Entry point for command line execution."""
    args = parse_args()
    tree, upgraded, not_upgraded = analyze(args.root)
    print_summary(tree, upgraded, not_upgraded)


if __name__ == "__main__":
    main()
