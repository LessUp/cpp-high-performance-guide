# Proposal: Deepen Runtime SIMD Module

## Summary

Create one canonical SIMD module in `include/hpc/simd.hpp` that owns runtime capability detection, alignment policy, wrapper types, and reusable vector kernels. Refactor the example, benchmark, and test surfaces to use that seam, and fix the coupled platform bug in `include/hpc/core.hpp`.

## Why

The current SIMD code is architecturally shallow:

1. Runtime feature detection is split between `simd_utils.hpp` and ad-hoc dispatch helpers, so the interface a caller learns is not the interface the implementation actually uses.
2. Reusable kernels (`add`, `dot`, `scale`, `clamp`, `multiply`) are duplicated across `simd_utils.hpp`, `simd_wrapper.hpp`, `intrinsics_intro.cpp`, and `simd_bench.cpp`, which destroys locality.
3. The public `hpc` surface has no canonical SIMD module even though SIMD is a first-class capability in the repository.
4. `include/hpc/core.hpp` has a Windows compile-path bug (`SYSTEM_INFO` / `GetSystemInfo` without the required include), which is tightly coupled to the refactor because the new public module depends on platform utilities being trustworthy.

## In scope

- Add `include/hpc/simd.hpp` as the public SIMD module
- Concentrate runtime SIMD level detection and alignment selection there
- Centralize reusable SIMD wrapper types and high-level kernels there
- Refactor example, benchmark, and test code to use the new seam
- Keep or reduce total code duplication in the SIMD area
- Fix the coupled `core.hpp` Windows include bug

## Out of scope

- Repository-wide allocator unification across memory/concurrency/SIMD
- New docs-site information architecture
- New benchmark dashboards or CI jobs
- Reworking the memory or concurrency teaching modules in this change

## Success criteria

- `include/hpc/simd.hpp` becomes the single caller-facing SIMD module
- Runtime SIMD level reporting and alignment selection are consistent on supported x86 GCC/Clang builds
- SIMD tests validate the new public seam instead of example-internal headers
- Existing SIMD examples and benchmarks still build and run through the refactored module
- `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` passes
- Sanitizer validation for the touched surface passes without new failures
