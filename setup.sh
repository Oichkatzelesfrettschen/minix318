#!/bin/bash
set -e

echo "Updating package lists..."
sudo apt-get update

echo "Installing Doxygen, Graphviz, clang-format, cppcheck..."
sudo apt-get install -y doxygen graphviz clang-format cppcheck

echo "Development tools setup complete."
