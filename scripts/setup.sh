#!/usr/bin/env bash
set -euo pipefail

echo "Setting up development environment..."

check_tool() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "Warning: $1 not found"
        return 1
    fi

    echo "✓ $1 found"
    return 0
}

check_tool g++ || check_tool clang++
check_tool cmake
check_tool ninja || check_tool make

check_tool clang-format || echo "  Install for code formatting"
check_tool clangd || echo "  Install for editor/LSP support"
check_tool perf || echo "  Install for profiling"
check_tool valgrind || echo "  Install for memory analysis"

if git rev-parse --show-toplevel >/dev/null 2>&1; then
    ./scripts/setup-hooks.sh
fi

echo ""
echo "Setup check complete!"
