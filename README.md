[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/Oichkatzelesfrettschen/minix318)

## Building with Meson and Ninja

The project uses the Meson build system. A minimal configuration relies on the
LLVM toolchain and `ninja` for compilation:

```bash
export CC=clang-18
export CFLAGS="-O3 -pthread -fopenmp -Werror"
meson setup build
ninja -C build
```

This configuration ensures all kernel sources are compiled with modern LLVM
optimizations and strict warning enforcement.

## Development Tools Setup

Install the LLVM toolchain and supporting utilities. The `setup.sh` script
automates this configuration:
```bash
sudo apt-get update
sudo apt-get install -y clang-18 clang-tools-18 llvm-18-dev \
  libpolly-18-dev lld-18 lldb-18 bolt-18 build-essential binutils \
  meson valgrind lcov
```
Verify versions with `apt-cache policy` before adjusting the major release.

## Recommended Tools

Install additional development utilities:
```bash
sudo apt-get install -y cscope universal-ctags graphviz cloc build-essential git python3-pip libgraphviz-dev
pip3 install pygraphviz matplotlib pandas
```

Run `scripts/acpi_files.py` to list all ACPI related files. Use
`scripts/gen_acpi_tags.sh` to generate ctags and cscope databases for deeper
analysis.

