# Programming Languages in MINIX

This repository contains several languages used for the operating system and associated tools.

## C
- The bulk of MINIX is written in C.
- Code is generally built with GCC 4.8.5 or Clang 3.4.
- Most of the system follows ISO C99. Some toolchain components limit themselves to C89 for portability.

## C++
- A limited amount of C++ exists, primarily in external packages.
- Supported with GCC 4.8.5 (including libstdc++ and g++) and Clang 3.4.
- C++11 features are available.

## Assembly
- Assembly sources use the GNU assembler syntax.
- These files are compiled as part of the normal build through GCC or clang.

## Go
- LLVM bindings include Go code.
- Requires a Go compiler at least version 1.2.

## Python
- Some build scripts and tests use Python 2.7 or newer.
- Python 3 is also supported by parts of the tree.

## AWK
- Several utilities rely on POSIX AWK scripts.

## Shell
- Numerous build scripts use the POSIX shell.

