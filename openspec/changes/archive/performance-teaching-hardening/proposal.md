# Proposal: Performance Teaching Hardening

## Summary

Harden the first wave of existing teaching surfaces: close the gap between the SIMD module's compile-time wrapper and runtime dispatch, surface vectorization and sanitizer workflows in reader-facing documentation, and establish a reproducible benchmark regression path.

## Why

The repository has solid example code and CI scaffolding but three gaps remain that reduce teaching value and maintainability:

1. The SIMD module has a compile-time `FloatVec` alias that selects one instruction set at compile time. There is no runtime dispatch example, so readers who want portable SIMD code for heterogeneous deployments have no guide.
2. Vectorization diagnostics (`-fopt-info-vec`, `-Rpass=loop-vectorize`) and sanitizer workflows (ASan/TSan/UBSan) are reachable via CMake presets but are not surfaced in reader-facing documentation.
3. The benchmark suite produces JSON output but there is no documented or scripted path for comparing runs across commits, making regression detection manual and fragile.

## Scope

### In scope

- Runtime CPU dispatch closure for the SIMD module (example + test)
- Vectorization diagnostics workflow documented for readers
- Sanitizer workflow surfaced in reader-facing docs and the docs site
- Benchmark regression comparison: documented workflow and script

### Out of scope

- New teaching modules (concurrency, memory, modern-cpp extensions)
- CI benchmark publishing to external dashboards
- AVX-512 masking or gather/scatter intrinsics
- Windows or macOS port validation

## Success criteria

- A `runtime_dispatch.cpp` example under `examples/04-simd-vectorization/src/` compiles and the corresponding test passes in the debug preset.
- The example README and docs site entry for the SIMD module explain how to see compiler vectorization reports.
- The docs site has a visible validation / sanitizer path that a reader can follow without reading CMakeLists.txt.
- A `scripts/compare_benchmarks.py` script accepts two Google Benchmark JSON files and prints a human-readable regression report; the script is referenced from the benchmark module README.
