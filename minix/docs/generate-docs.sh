#!/bin/bash
#
# Script to generate Doxygen documentation for the MINIX C23 Kernel project.
# This script should be run from the directory where it is located (minix/docs/).

set -e # Exit immediately if a command exits with a non-zero status.

echo "Generating MINIX Kernel Documentation..."
echo "Current working directory: $(pwd)"
echo "Using Doxyfile: Doxyfile.kernel"

# Ensure Doxyfile.kernel exists in the current directory
if [ ! -f "Doxyfile.kernel" ]; then
    echo "Error: Doxyfile.kernel not found in the current directory!"
    echo "Please run this script from the 'minix/docs/' directory."
    exit 1
fi

# Run Doxygen
doxygen Doxyfile.kernel

echo ""
echo "Doxygen documentation generation process initiated."
echo "Check Doxygen output for warnings or errors."
echo "HTML output (if successful and not aborted by environment limits) would be in './generated-docs/html/'"
