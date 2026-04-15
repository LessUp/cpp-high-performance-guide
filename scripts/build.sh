#!/bin/bash
set -e

# Build script for C++ High Performance Guide

PRESET=${1:-release}

echo "Building with preset: $PRESET"
cmake --preset=$PRESET
cmake --build build/$PRESET

echo "Build completed successfully!"
