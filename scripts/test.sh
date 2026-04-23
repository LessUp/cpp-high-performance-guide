#!/usr/bin/env bash
set -euo pipefail

preset="${1:-release}"

echo "Running tests with preset: $preset"
ctest --preset="$preset"

echo "All tests passed."
