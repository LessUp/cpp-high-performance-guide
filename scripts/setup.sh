#!/bin/bash

echo "Setting up development environment..."

# Check for required tools
check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo "Warning: $1 not found"
        return 1
    fi
    echo "✓ $1 found"
    return 0
}

# Check compilers
check_tool g++ || check_tool clang++
check_tool cmake
check_tool ninja || check_tool make

# Optional tools
check_tool clang-format || echo "  Install for code formatting"
check_tool perf || echo "  Install for profiling"
check_tool valgrind || echo "  Install for memory analysis"

echo ""
echo "Setup check complete!"
