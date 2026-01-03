# Profiling Guide

This guide covers essential profiling tools and techniques for analyzing C++ performance.

## Overview

Performance optimization follows a simple cycle:
1. **Measure** - Profile to find bottlenecks
2. **Analyze** - Understand the root cause
3. **Optimize** - Apply targeted improvements
4. **Verify** - Measure again to confirm improvement

## Tools

### perf (Linux)

`perf` is the standard Linux profiling tool.

#### Installation

```bash
# Ubuntu/Debian
sudo apt-get install linux-tools-common linux-tools-generic

# Fedora
sudo dnf install perf
```

#### Basic Usage

```bash
# Record CPU samples
perf record -g ./your_benchmark

# View report
perf report

# Show annotated source
perf annotate
```

#### Useful Commands

```bash
# CPU cycles breakdown
perf stat ./your_benchmark

# Cache miss analysis
perf stat -e cache-references,cache-misses,L1-dcache-load-misses ./your_benchmark

# Branch prediction
perf stat -e branches,branch-misses ./your_benchmark

# Record with call graph (dwarf for C++)
perf record -g --call-graph dwarf ./your_benchmark
```

### FlameGraph

FlameGraphs provide intuitive visualization of where time is spent.

#### Using the Project Script

```bash
# Generate FlameGraph for a benchmark
./tools/flamegraph/generate_flamegraph.sh ./build/release/examples/02-memory-cache/bench/aos_soa_bench

# View the result
firefox flamegraph.svg
```

#### Manual Generation

```bash
# Clone FlameGraph tools (if not already done)
git clone https://github.com/brendangregg/FlameGraph.git

# Record with perf
perf record -F 99 -g ./your_benchmark

# Generate FlameGraph
perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > flamegraph.svg
```

#### Reading FlameGraphs

- **Width** = Time spent (wider = more time)
- **Height** = Call stack depth
- **Color** = Random (no meaning)
- **Top** = Currently executing function
- **Bottom** = Entry point (main)

Look for:
- Wide plateaus (hot functions)
- Deep stacks (excessive call depth)
- Unexpected functions taking time

### Valgrind

Valgrind provides detailed memory and cache analysis.

#### Cachegrind (Cache Simulation)

```bash
# Run cache simulation
valgrind --tool=cachegrind ./your_benchmark

# View results
cg_annotate cachegrind.out.*
```

Output shows:
- I1 cache misses (instruction cache)
- D1 cache misses (L1 data cache)
- LL cache misses (last-level cache)

#### Callgrind (Call Graph Profiling)

```bash
# Run call graph profiling
valgrind --tool=callgrind ./your_benchmark

# View with KCachegrind (GUI)
kcachegrind callgrind.out.*
```

### Intel VTune (Advanced)

VTune provides the most detailed analysis on Intel CPUs.

#### Installation

Download from [Intel oneAPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html).

#### Basic Usage

```bash
# Hotspots analysis
vtune -collect hotspots ./your_benchmark

# Memory access analysis
vtune -collect memory-access ./your_benchmark

# Microarchitecture analysis
vtune -collect uarch-exploration ./your_benchmark

# View results
vtune-gui
```

## Profiling Strategies

### CPU-Bound Code

1. Start with `perf stat` for overview
2. Use `perf record` + FlameGraph to find hot functions
3. Use `perf annotate` to see hot instructions
4. Check vectorization with compiler reports

```bash
# Check if code is vectorized
g++ -O3 -march=native -fopt-info-vec-optimized your_code.cpp
```

### Memory-Bound Code

1. Check cache misses with `perf stat`
2. Use Cachegrind for detailed cache analysis
3. Look for:
   - High L1 miss rate (> 5%)
   - High LLC miss rate (> 1%)
   - Poor spatial locality

```bash
# Quick cache check
perf stat -e L1-dcache-load-misses,L1-dcache-loads ./your_benchmark
```

### Multi-threaded Code

1. Check for false sharing
2. Analyze lock contention
3. Verify thread scaling

```bash
# Check for cache line bouncing (false sharing indicator)
perf stat -e cache-misses ./your_benchmark

# Run with different thread counts
OMP_NUM_THREADS=1 ./your_benchmark
OMP_NUM_THREADS=2 ./your_benchmark
OMP_NUM_THREADS=4 ./your_benchmark
```

## Common Performance Issues

### 1. Cache Misses

**Symptoms:**
- High L1/L2/L3 miss rates
- Memory bandwidth saturation

**Solutions:**
- Improve data locality (SOA layout)
- Use prefetching
- Reduce working set size

### 2. Branch Mispredictions

**Symptoms:**
- High branch-misses count
- Unpredictable control flow

**Solutions:**
- Use branchless code
- Sort data to improve prediction
- Use CMOV instructions

### 3. False Sharing

**Symptoms:**
- Poor multi-threaded scaling
- High cache-to-cache transfers

**Solutions:**
- Pad data to cache line boundaries
- Use thread-local storage
- Reduce shared state

### 4. Vectorization Failures

**Symptoms:**
- Scalar code in hot loops
- No SIMD instructions in assembly

**Solutions:**
- Align data
- Use `restrict` pointers
- Simplify loop structure
- Use explicit SIMD intrinsics

## Benchmark Best Practices

### Avoid Measurement Errors

```cpp
// Prevent dead code elimination
benchmark::DoNotOptimize(result);

// Force memory writes to be visible
benchmark::ClobberMemory();
```

### Warm Up Caches

```cpp
// Run a few iterations before measuring
for (int i = 0; i < warmup_iterations; ++i) {
    do_work();
}
```

### Control Environment

```bash
# Disable CPU frequency scaling
sudo cpupower frequency-set --governor performance

# Pin to specific CPU
taskset -c 0 ./your_benchmark

# Disable ASLR for reproducibility
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

### Statistical Significance

- Run multiple iterations
- Report mean, median, and standard deviation
- Use Google Benchmark's built-in statistics

```bash
# Run with statistics
./your_benchmark --benchmark_repetitions=10 --benchmark_report_aggregates_only=true
```

## Quick Reference

| Task | Tool | Command |
|------|------|---------|
| CPU hotspots | perf | `perf record -g ./bench && perf report` |
| Cache misses | perf | `perf stat -e cache-misses ./bench` |
| Visual profile | FlameGraph | `./tools/flamegraph/generate_flamegraph.sh ./bench` |
| Detailed cache | Valgrind | `valgrind --tool=cachegrind ./bench` |
| Call graph | Valgrind | `valgrind --tool=callgrind ./bench` |
| Vectorization | GCC | `-fopt-info-vec-optimized` |
| Vectorization | Clang | `-Rpass=loop-vectorize` |
