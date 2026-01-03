#!/bin/bash
#
# generate_flamegraph.sh - Generate FlameGraph from perf data
#
# Usage: ./generate_flamegraph.sh <executable> [output.svg] [options]
#
# Options:
#   --duration <seconds>  Recording duration (default: run until completion)
#   --frequency <hz>      Sampling frequency (default: 99)
#   --title <title>       FlameGraph title
#   --width <pixels>      SVG width (default: 1200)
#   --colors <scheme>     Color scheme: hot, mem, io, wakeup, java, js, perl
#
# Requirements:
# - perf (Linux performance tools)
# - FlameGraph scripts (https://github.com/brendangregg/FlameGraph)
#
# Example:
#   ./generate_flamegraph.sh ./build/release/aos_vs_soa flamegraph.svg
#   ./generate_flamegraph.sh ./build/release/simd_bench simd.svg --title "SIMD Performance"

set -e

# Default values
FREQUENCY=99
WIDTH=1200
COLORS="hot"
TITLE=""
DURATION=""

# Parse arguments
EXECUTABLE=""
OUTPUT=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --frequency)
            FREQUENCY="$2"
            shift 2
            ;;
        --title)
            TITLE="$2"
            shift 2
            ;;
        --width)
            WIDTH="$2"
            shift 2
            ;;
        --colors)
            COLORS="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 <executable> [output.svg] [options]"
            echo ""
            echo "Options:"
            echo "  --duration <seconds>  Recording duration"
            echo "  --frequency <hz>      Sampling frequency (default: 99)"
            echo "  --title <title>       FlameGraph title"
            echo "  --width <pixels>      SVG width (default: 1200)"
            echo "  --colors <scheme>     Color scheme: hot, mem, io, wakeup"
            echo ""
            echo "Example:"
            echo "  $0 ./build/release/aos_vs_soa flamegraph.svg"
            exit 0
            ;;
        *)
            if [ -z "$EXECUTABLE" ]; then
                EXECUTABLE="$1"
            elif [ -z "$OUTPUT" ]; then
                OUTPUT="$1"
            fi
            shift
            ;;
    esac
done

# Set default output
OUTPUT="${OUTPUT:-flamegraph.svg}"
PERF_DATA="perf.data.$$"

# Check arguments
if [ -z "$EXECUTABLE" ]; then
    echo "Error: No executable specified"
    echo "Usage: $0 <executable> [output.svg] [options]"
    exit 1
fi

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable not found: $EXECUTABLE"
    exit 1
fi

# Check if perf is available
if ! command -v perf &> /dev/null; then
    echo "Error: perf not found."
    echo ""
    echo "Install on Ubuntu/Debian:"
    echo "  sudo apt-get install linux-tools-generic linux-tools-$(uname -r)"
    echo ""
    echo "Install on Fedora/RHEL:"
    echo "  sudo dnf install perf"
    exit 1
fi

# Check if FlameGraph scripts are available
FLAMEGRAPH_DIR="${FLAMEGRAPH_DIR:-$HOME/FlameGraph}"
if [ ! -d "$FLAMEGRAPH_DIR" ]; then
    echo "FlameGraph not found at $FLAMEGRAPH_DIR"
    echo ""
    echo "Install with:"
    echo "  git clone https://github.com/brendangregg/FlameGraph.git $FLAMEGRAPH_DIR"
    echo ""
    echo "Or set FLAMEGRAPH_DIR environment variable to your FlameGraph location."
    exit 1
fi

# Build perf record command
PERF_CMD="perf record -g -F $FREQUENCY -o $PERF_DATA"
if [ -n "$DURATION" ]; then
    PERF_CMD="$PERF_CMD -- timeout $DURATION"
fi

# Build flamegraph.pl options
FG_OPTS="--width $WIDTH --colors $COLORS"
if [ -n "$TITLE" ]; then
    FG_OPTS="$FG_OPTS --title \"$TITLE\""
fi

echo "=== FlameGraph Generator ==="
echo "Executable: $EXECUTABLE"
echo "Output: $OUTPUT"
echo "Frequency: $FREQUENCY Hz"
echo ""

echo "Recording performance data..."
$PERF_CMD "$EXECUTABLE" 2>/dev/null || true

if [ ! -f "$PERF_DATA" ]; then
    echo "Error: Failed to record performance data"
    exit 1
fi

echo "Generating FlameGraph..."
perf script -i "$PERF_DATA" 2>/dev/null | \
    "$FLAMEGRAPH_DIR/stackcollapse-perf.pl" 2>/dev/null | \
    eval "$FLAMEGRAPH_DIR/flamegraph.pl" $FG_OPTS > "$OUTPUT" 2>/dev/null

if [ -f "$OUTPUT" ] && [ -s "$OUTPUT" ]; then
    echo ""
    echo "FlameGraph saved to: $OUTPUT"
    echo "Open in a web browser to view."
else
    echo "Error: Failed to generate FlameGraph"
    rm -f "$PERF_DATA"
    exit 1
fi

# Cleanup
rm -f "$PERF_DATA"

echo ""
echo "Tips:"
echo "  - Wider boxes = more time spent in that function"
echo "  - Click on a box to zoom in"
echo "  - Search with Ctrl+F"
