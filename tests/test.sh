#!/bin/bash

# Ensure we are in the project root
cd "$(dirname "$0")/.."

# Build project
mkdir -p build && cd build && cmake .. && make && cd ..

# 1. Test CLI functionality
mkdir -p build/test_output
./build/lexy tests/myScanner.lexy -o ./build/test_output -g
if [ -f "./build/test_output/scanners/myScanner.cpp" ] && [ -f "./build/test_output/images/nfa.png" ]; then
    echo "CLI Test Passed"
else
    echo "CLI Test Failed"
    exit 1
fi

# 2. Test Generated Scanner
# Copy generated scanner to the location expected by the test
mkdir -p build/scanners
cp build/test_output/scanners/myScanner.cpp build/scanners/

# Compile and run test
g++ -std=c++20 tests/test_myScanner.cpp -o build/test_scanner
./build/test_scanner
if [ $? -eq 0 ]; then
    echo "Scanner Test Passed"
else
    echo "Scanner Test Failed"
    exit 1
fi
