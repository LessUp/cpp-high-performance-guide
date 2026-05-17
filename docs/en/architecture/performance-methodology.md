# Performance Methodology

The repository uses a conservative methodology for performance work: establish a correct baseline, isolate one variable, measure with the right preset, profile when causality is unclear, and publish only the conclusions that survive repetition.

## Measurement contract

| Step | What to do | Why it matters |
| --- | --- | --- |
| 1. Reproduce | build and run the unmodified workload first | avoids optimizing a rumor |
| 2. Choose the preset | `debug`, `release`, or `relwithdebinfo` depending on the question | keeps results tied to an explicit execution mode |
| 3. Validate correctness | run the relevant tests before trusting numbers | fast code that is wrong is noise |
| 4. Measure | use benchmark executables or counters intentionally | prevents profiler cargo culting |
| 5. Profile | inspect call paths or counters when the cause is not obvious | explains *why* a result changed |
| 6. Re-run and document | confirm the effect and record the boundary conditions | makes the claim auditable later |

## Preset selection matrix

| Preset | Best use | Typical command |
| --- | --- | --- |
| `debug` | baseline behavior and failing tests | `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` |
| `release` | representative benchmark numbers | `cmake --preset=release && cmake --build build/release && ctest --preset=release` |
| `relwithdebinfo` | symbolized profiling with meaningful optimization | `cmake --preset=relwithdebinfo && cmake --build build/relwithdebinfo` |
| `asan` | memory-safety investigation | `cmake --preset=asan && cmake --build build/asan && ctest --preset=asan` |
| `tsan` | concurrency and memory-ordering investigation | `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan` |
| `ubsan` | undefined-behavior checks for low-level code | `cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan` |

## Benchmark protocol

When the repository uses Google Benchmark-based executables, the protocol is straightforward:

1. build an optimized binary, usually under `build/release/examples/<module>/`
2. run the exact benchmark executable for the module you changed
3. keep the dataset and compiler flags stable while you compare variants
4. prefer multiple short, comparable runs over one anecdotal outlier

Representative examples include:

```bash
./build/release/examples/02-memory-cache/aos_vs_soa_bench
./build/release/examples/04-simd-vectorization/simd_bench
```

A benchmark number is strong evidence only when the changed variable is clear. If code layout, compiler, dataset, and synchronization strategy all changed together, the result is descriptive but not diagnostic.

## Profiling protocol

Use profiling when you need to explain a result, not merely display it.

### Counter-oriented checks

```bash
perf stat -d ./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

Use this for quick checks on cache misses, branch behavior, or broad CPU-bound versus memory-bound suspicion.

### Call-path inspection

```bash
perf record -g --call-graph dwarf ./build/relwithdebinfo/examples/04-simd-vectorization/simd_bench
perf report
```

Use this when a benchmark changes but the cause is not obvious from source inspection.

### FlameGraph workflow

```bash
./tools/performance/generate_flamegraph.sh ./build/relwithdebinfo/examples/02-memory-cache/aos_vs_soa_bench
```

Use this when the explanation benefits from a durable visual artifact or when a wide call tree needs to be collapsed into a reviewable picture.

## Interpreting results responsibly

### Cache and memory work

For memory-layout changes, prefer explanations grounded in cache behavior, access locality, or false-sharing reduction. A speedup without that model is incomplete.

### SIMD work

For vectorization, distinguish between:

- compiler auto-vectorization
- explicit intrinsics or wrapper-based SIMD
- runtime dispatch that selects among ISA-specific implementations

Those are different engineering choices with different maintenance costs.

### Concurrency work

For threading changes, never treat throughput alone as sufficient evidence. Pair results with synchronization reasoning and `tsan` where the change touched atomics, queues, or thread interaction.

## External anchors

The repository's methodology is intentionally close to widely used public references:

- [Google Benchmark](https://github.com/google/benchmark) for microbenchmark structure
- [Brendan Gregg's FlameGraph project](https://github.com/brendangregg/FlameGraph) for call-stack visualization
- [perf wiki](https://perf.wiki.kernel.org/) for Linux performance counters
- [Agner Fog's optimization resources](https://www.agner.org/optimize/) for microarchitectural interpretation

Use the [Research References](/en/research/references) page for a broader source shelf.
