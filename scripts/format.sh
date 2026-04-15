#!/bin/bash

# Format script using clang-format

echo "Formatting C++ source files..."
find examples tests benchmarks -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | \
    xargs -I {} clang-format -i {}

echo "Formatting completed!"
