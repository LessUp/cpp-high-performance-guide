# Benchmarks

This directory holds shared benchmark utilities plus the local workflow for
capturing and comparing Google Benchmark JSON output.

---

## Regression Comparison

1. Build the optimized benchmark targets:

```bash
cmake --preset=release
cmake --build build/release
```

2. Capture a baseline run:

```bash
./build/release/examples/04-simd-vectorization/simd_bench \
  --benchmark_format=json \
  --benchmark_out=simd-baseline.json
```

3. Capture a candidate run after your change:

```bash
./build/release/examples/04-simd-vectorization/simd_bench \
  --benchmark_format=json \
  --benchmark_out=simd-candidate.json
```

4. Compare the two runs:

```bash
python3 tools/performance/benchmark_compare.py simd-baseline.json simd-candidate.json --threshold 10
```

The script prints a per-benchmark table with baseline time, candidate time, and
delta percentage. It exits with code `1` when any benchmark regresses by more
than the threshold, which makes it suitable for local smoke checks or future CI
gating.
