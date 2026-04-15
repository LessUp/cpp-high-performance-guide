# Tools

This directory contains various tools for development, profiling, and analysis.

## Structure

- `performance/` - Performance analysis and profiling tools
  - `generate_flamegraph.sh` - Generate FlameGraph visualizations from perf data
  - `benchmark_compare.py` - Compare benchmark results
- `scripts/` - Development utility scripts

## Usage

### FlameGraph Generation

```bash
./tools/performance/generate_flamegraph.sh ./build/release/your_benchmark
```

### Benchmark Comparison

```bash
python3 tools/performance/benchmark_compare.py baseline.json current.json
```
