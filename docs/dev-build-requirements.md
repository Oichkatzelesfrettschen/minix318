# Developer Build Requirements

This project relies on a number of tools and libraries to build and analyze the MINIX sources. The `setup.sh` script installs everything automatically. The list below documents each package so a developer can replicate the environment manually.

## System packages

The following Debian/Ubuntu packages are installed:

- build-essential
- clang
- clang-tools
- clang-format
- clang-tidy
- binutils
- gdb
- gdb-multiarch
- lldb
- strace
- ltrace
- valgrind
- radare2
- afl
- cscope
- universal-ctags
- graphviz
- doxygen
- shfmt
- jq
- tree
- qemu-system-x86
- qemu-system-arm
- gcc-arm-linux-gnueabihf
- python3
- python3-pip
- python3-venv
- cmake
- perf
- lsof
- nodejs
- npm

## Node dependencies

After the system packages are installed, `npm ci` installs the Node-based developer tools defined in `package.json`. Currently this includes the `jscpd` copy/paste detector.
