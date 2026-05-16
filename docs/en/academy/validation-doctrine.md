# Validation Doctrine

Performance claims in this repository should be backed by preset-driven builds, tests, and—when appropriate—sanitizer or benchmark evidence.

## Default path

```bash
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug
```

## Escalation path

- Use `release` when you need representative benchmark binaries.
- Use `asan`, `tsan`, and `ubsan` when investigating memory, race, or undefined-behavior failures.
- Treat profiling and benchmarks as supporting evidence, not as substitutes for functional correctness.

## Companion pages

- [Validation & Sanitizers](/en/guides/validation)
- [Performance Methodology](/en/architecture/performance-methodology)
- [Profiling Guide](/en/guides/profiling-guide)
