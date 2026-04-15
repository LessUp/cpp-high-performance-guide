#!/bin/bash
set -e

# Test script for C++ High Performance Guide

PRESET=${1:-release}

echo "Running tests with preset: $PRESET"
ctest --preset=$PRESET

echo "All tests passed!"
