#!/bin/bash

echo "=== MINIX 3.1.8 with LLVM-20 Magic Framework Test Suite ==="
echo "Running comprehensive tests for Magic instrumentation system"
echo ""

# Build all tests
echo "Building test suite..."
make clean && make all

if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed"
    exit 1
fi

echo "[SUCCESS] Build completed"
echo ""

# Run all tests
echo "Running test suite..."
make test

if [ $? -eq 0 ]; then
    echo ""
    echo "=== ALL TESTS PASSED ==="
    echo "Magic LLVM-20 framework is working correctly!"
    exit 0
else
    echo ""
    echo "=== SOME TESTS FAILED ==="
    echo "Please review the test output above"
    exit 1
fi
