#!/usr/bin/env python3
"""Generate Mermaid include graphs for given source directory.

This script searches C source and header files for include statements and
produces a Mermaid graph showing which files include which headers.
An optional SVG is produced using mermaid-cli if available.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
from pathlib import Path


INCLUDE_RE = re.compile(r"^\s*#\s*include\s+[<\"]([^\">]+)[\">]")


def gather_includes(root: Path) -> list[tuple[str, str]]:
    """Collect (source, header) pairs from all files under *root*."""
    edges: list[tuple[str, str]] = []
    for path in root.rglob("*.[ch]"):
        try:
            with path.open("r", errors="ignore") as f:
                for line in f:
                    m = INCLUDE_RE.match(line)
                    if m:
                        header = m.group(1)
                        edges.append((str(path.relative_to(root)), header))
        except OSError:
            pass
    return edges


def write_mermaid(edges: list[tuple[str, str]], out_file: Path) -> None:
    """Write Mermaid graph from *edges* to *out_file*."""
    with out_file.open("w") as f:
        f.write("graph TD\n")
        for src, header in edges:
            src_node = src.replace(" ", "_")
            header_node = header.replace(" ", "_")
            f.write(f"    {src_node}--> {header_node}\n")


def render_svg(mmd_file: Path, svg_file: Path) -> None:
    """Render Mermaid file to SVG if mermaid-cli is available."""
    cmd = ["mmdc", "-i", str(mmd_file), "-o", str(svg_file)]
    if Path("puppeteer.json").exists():
        cmd += ["-p", "puppeteer.json"]
    if Path("mermaid_config.json").exists():
        cmd += ["-c", "mermaid_config.json"]
    try:
        subprocess.run(cmd, check=True)
    except (FileNotFoundError, subprocess.CalledProcessError):
        print("Warning: mermaid-cli not installed or failed to render SVG")


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate Mermaid include graph")
    parser.add_argument("source", type=Path, help="Root directory to analyze")
    parser.add_argument(
        "output", type=Path, help="Output file prefix (without extension)"
    )
    args = parser.parse_args()

    edges = gather_includes(args.source.resolve())
    mmd_file = args.output.with_suffix(".mmd")
    svg_file = args.output.with_suffix(".svg")

    write_mermaid(edges, mmd_file)
    render_svg(mmd_file, svg_file)
    print(f"Mermaid graph written to {mmd_file}")
    if svg_file.exists():
        print(f"SVG output written to {svg_file}")


if __name__ == "__main__":
    main()
