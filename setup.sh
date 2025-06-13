#!/usr/bin/env sh
# setup.sh - environment setup for MINIX libc exploration
# Installs utilities for debugging, reverse engineering, and analysis.
# This script is shell-agnostic and uses POSIX sh features only.

set -e

# Ensure noninteractive apt operations
export DEBIAN_FRONTEND=noninteractive

# Update package index
apt-get update -o Acquire::Retries=3

# Base utilities required for package configuration
apt-get install -y apt-utils ca-certificates

# Install core compilation and debugging utilities
apt-get install -y \
	clang-18 \
	clang-tools-18 \
	llvm-18-dev \
	libpolly-18-dev \
	lld-18 \
	lldb-18 \
	clang-format-18 \
	clang-tidy-18 \
	bolt-18 \
	build-essential \
	binutils \
	valgrind \
	lcov \
	gdb \
	gdb-multiarch \
	strace \
	ltrace \
	radare2 \
	cscope \
	universal-ctags \
	graphviz \
	doxygen \
	shfmt \
	meson \
	ninja-build \
	pkg-config \
	jq \
	tree \
	cloc

# Install virtualization and cross-development tools
apt-get install -y \
	qemu-system-x86 \
	qemu-system-arm \
	gcc-arm-linux-gnueabihf

# Install auxiliary utilities for scripting and performance analysis
apt-get install -y \
	python3 \
	python3-pip \
	python3-venv \
	python3-setuptools \
	python3-dev \
	cmake \
	lsof

# Install Node.js and npm for JavaScript-based tooling
apt-get install -y nodejs npm

# Install Node-based developer dependencies (e.g., jscpd)
npm ci

# Globally install CLI tools for duplication detection (jscpd)
# and line-of-code metrics (cloc)
npm install -g jscpd cloc

# Clean up apt cache to reduce image size
apt-get clean
