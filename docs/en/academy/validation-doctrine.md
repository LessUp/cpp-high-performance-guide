# Validation Doctrine

Performance claims in this repository are accepted only when they survive the appropriate preset, test, and measurement path. The default posture is simple: prove correctness first, then measure, then explain why the number is believable.

## Default contract

The baseline path is the same one maintainers are expected to use for ordinary changes:

```bash
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug
```

This path answers one narrow but essential question: **does the repository still build and pass its standard checks in a debuggable configuration?**

## Evidence ladder

| Question | Minimum evidence | Typical escalation |
| --- | --- | --- |
| Does the change preserve behavior? | `debug` build and `ctest --preset=debug` | add a focused unit or property test when the contract was previously implicit |
| Does the optimized path stay representative? | `release` or `relwithdebinfo` build and a benchmark run | collect profiler output if the speedup is not obvious or stable |
| Does the low-level code remain memory-safe? | `asan` build and test run | inspect the smallest failing workload before changing multiple variables |
| Does concurrent code obey its synchronization contract? | unit or property tests plus `tsan` | reduce the workload and confirm the race with one reproducible path |
| Are arithmetic, layout, or cast assumptions sound? | `ubsan` | document the assumption if it is intentional and architecture-specific |

## Preset ladder

The repository uses presets as the public vocabulary for validation. That keeps commands stable across docs, CI, and local maintenance.

| Preset | Use it when | Why it exists |
| --- | --- | --- |
| `debug` | establishing a baseline | fast feedback, readable failures, full test coverage |
| `release` | comparing performance-sensitive variants | optimized binaries expose real code generation |
| `relwithdebinfo` | profiling a workload you expect to keep | optimization plus symbols is the right default for `perf` |
| `asan` | investigating memory lifetime or bounds issues | catches classes of bugs that may not fail functionally |
| `tsan` | touching atomics, queues, or OpenMP code | race detection is a separate concern from raw throughput |
| `ubsan` | validating low-level arithmetic, shifts, or conversions | undefined behavior often invalidates benchmark conclusions |

## Benchmark doctrine

Benchmarks are part of the evidence ladder, not a substitute for it. In this repository, a benchmark result is persuasive only when the surrounding conditions are explicit:

1. **The executable is identified.** Prefer the exact benchmark path, such as `build/release/examples/02-memory-cache/bench/aos_soa_bench`.
2. **The build preset is named.** `release` and `relwithdebinfo` mean different things for diagnosis.
3. **The comparison isolates one variable.** Algorithm, data layout, or synchronization strategy should change one at a time.
4. **The hardware context is understood.** Cache, ISA, and core count determine whether a result generalizes.
5. **The number is reproducible.** If you cannot rerun it locally, it is an observation, not yet a doctrine.

## Profiling doctrine

Use a profiler when you need causality, not merely a before-and-after number.

- Use `perf stat` for quick counter-level checks.
- Use `perf record -g --call-graph dwarf` or FlameGraph tooling when you need a call-path explanation.
- Use `relwithdebinfo` when symbolized profiling matters more than peak benchmark purity.
- Treat profiler output as a guide to the next experiment, not as a proof by itself.

## When a claim is ready to publish

A performance claim is ready for docs, comments, or review only when you can answer all of these:

- Which preset produced the binary?
- Which executable or test target was run?
- Which variable changed?
- Which validation surface would falsify the claim if it were wrong?
- Which page or source file should a skeptical reader inspect next?

## Companion pages

- [Performance Methodology](/en/architecture/performance-methodology)
- [Validation & Sanitizers](/en/guides/validation)
- [Profiling Guide](/en/guides/profiling-guide)
- [Research References](/en/research/references)
