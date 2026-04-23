#!/usr/bin/env bash
set -euo pipefail

preset="${1:-release}"

echo "Configuring preset: $preset"
cmake --preset="$preset"

echo "Building preset: $preset"
cmake --build --preset "$preset"

echo "Build completed successfully."
