# Repository Contribution Guidelines

This project requires that all modernization efforts preserve historical
information. Legacy comments that predate Doxygen must **not** be removed.
Instead, convert them to Doxygen style so their content remains available in
modern documentation.

* Never rewrite existing commits. Add new commits only.
* During refactoring or migration to C23, retain existing comments and include
  them in new Doxygen blocks.
* Modernize Meson build files but keep previous documentation intact.
* Use Doxygen together with Sphinx, Graphviz and other supported tools to
  generate diagrams and metrics automatically.
* All code must be formatted according to project conventions. Comments alone
  should not trigger reformatting.
* Every function and structure requires comments; use Doxygen whenever
  applicable.
* Documentation build steps should generate images and diagrams automatically
  when possible.

These rules apply to the entire repository.

## Development Tools Setup

The project expects a modern LLVM toolchain. Verify package availability and
install the following tools using `apt`:

```bash
sudo apt-get update
sudo apt-get install -y clang-18 clang-tools-18 llvm-18-dev \
  libpolly-18-dev lld-18 lldb-18 bolt-18 build-essential binutils \
  meson valgrind lcov clov
```

Update this list as new major versions (e.g., `clang-19`) become available.
Check with `apt-cache policy` before changing versions.

The installed LLVM toolchain, `lcov`, `clov`, and `valgrind` form the
official toolkit used throughout development and testing.
