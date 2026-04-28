# Tasks: Performance Teaching Hardening

## 1. SIMD runtime dispatch

- [ ] 1.1 Add `examples/04-simd-vectorization/src/runtime_dispatch.cpp` with `hpc::simd::dispatch_add_arrays` using `__builtin_cpu_supports` for AVX2/SSE2/scalar selection
- [ ] 1.2 Add a `simd_dispatch` STATIC library target in `examples/04-simd-vectorization/CMakeLists.txt` (separate from the INTERFACE `simd_utils` target) that compiles `runtime_dispatch.cpp` and links `simd_utils` for headers
- [ ] 1.3 Add `examples/04-simd-vectorization/src/dispatch_example_main.cpp` and a `dispatch_example` executable target in `examples/04-simd-vectorization/CMakeLists.txt` that links `simd_dispatch` and demonstrates runtime dispatch
- [ ] 1.4 Add `tests/unit/simd/simd_dispatch_test.cpp` (and wire it into `tests/unit/simd/CMakeLists.txt`) that calls `dispatch_add_arrays` and validates results against the scalar reference; note: the test must compile without `-mavx2` so the runtime guard is exercised on machines without AVX2
- [ ] 1.5 Verify `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` passes with the new targets and test

## 2. Vectorization diagnostics documentation

- [ ] 2.1 Add a "Vectorization Diagnostics" section to `examples/04-simd-vectorization/README.md` with GCC (`-fopt-info-vec-optimized`) and Clang (`-Rpass=loop-vectorize`) flag examples and sample output
- [ ] 2.2 Extend `docs/en/guides/learning-path.md` (SIMD section) to surface the vectorization diagnostics workflow for readers, using `-fopt-info-vec-optimized` to stay consistent with the existing content in that file

## 3. Sanitizer workflow documentation

- [ ] 3.1 Add `docs/en/guides/validation.md` documenting the `asan`, `tsan`, and `ubsan` presets with copy-pasteable commands; add entry to VitePress nav
- [ ] 3.2 Cross-link the sanitizer section from the root `README.md` quick-start

## 4. Benchmark regression comparison

- [ ] 4.1 Add `scripts/compare_benchmarks.py`: accepts two Google Benchmark JSON files, prints a regression table (name, baseline, candidate, delta%), exits 1 if any benchmark exceeds the threshold (default 10%, configurable via `--threshold`)
- [ ] 4.2 Add a "Regression Comparison" section to `benchmarks/README.md` showing the capture-and-compare workflow
- [ ] 4.3 Smoke-test the script with two synthesised JSON inputs to confirm it exits 0 on stable and 1 on a regressed run
