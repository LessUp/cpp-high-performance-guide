# Tasks: Performance Teaching Hardening

## 1. SIMD runtime dispatch

- [ ] 1.1 Add `examples/04-simd-vectorization/src/runtime_dispatch.cpp` with `hpc::simd::dispatch_add_arrays` using `__builtin_cpu_supports` for AVX2/SSE2/scalar selection
- [ ] 1.2 Register `runtime_dispatch` target in `examples/04-simd-vectorization/CMakeLists.txt` via `hpc_add_example`
- [ ] 1.3 Add a correctness test under `tests/` that calls `dispatch_add_arrays` and validates results against the scalar reference
- [ ] 1.4 Verify `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` passes with the new target and test

## 2. Vectorization diagnostics documentation

- [ ] 2.1 Add a "Vectorization Diagnostics" section to `examples/04-simd-vectorization/README.md` with GCC (`-fopt-info-vec`) and Clang (`-Rpass=loop-vectorize`) flag examples and sample output
- [ ] 2.2 Add or extend a docs site page for the SIMD module to surface the vectorization diagnostics workflow for readers

## 3. Sanitizer workflow documentation

- [ ] 3.1 Add a "Validation and Safety" section to the VitePress docs site documenting the `asan`, `tsan`, and `ubsan` presets with copy-pasteable commands
- [ ] 3.2 Cross-link the sanitizer section from the root `README.md` quick-start

## 4. Benchmark regression comparison

- [ ] 4.1 Add `scripts/compare_benchmarks.py`: accepts two Google Benchmark JSON files, prints a regression table (name, baseline, candidate, delta%), exits 1 if any benchmark exceeds the threshold (default 10%, configurable via `--threshold`)
- [ ] 4.2 Add a "Regression Comparison" section to the benchmarks docs entry or `benchmarks/` README showing the capture-and-compare workflow
- [ ] 4.3 Smoke-test the script with two synthesised JSON inputs to confirm it exits 0 on stable and 1 on a regressed run
