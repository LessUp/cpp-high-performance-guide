# SIMD Vectorization

## ADDED Requirements

### Requirement: Canonical SIMD Module

THE Example_Module SHALL expose a single canonical SIMD module for reusable runtime detection, alignment selection, wrapper types, and vector kernels.

#### Scenario: Callers use one SIMD seam

- **WHEN** tests, examples, or benchmarks need reusable SIMD behavior
- **THEN** they include `include/hpc/simd.hpp` instead of reconstructing the same logic across multiple example-local modules

---

### Requirement: Runtime capability reporting matches kernel selection

THE Example_Module SHALL report runtime SIMD capability in a way that is consistent with the runtime-dispatched kernel path on supported x86 GCC/Clang builds.

#### Scenario: Runtime level reflects runtime CPU support

- **WHEN** `detect_simd_level()` runs on a supported x86 GCC/Clang build
- **THEN** it reports the highest supported runtime level among AVX-512, AVX2, AVX, SSE2, or Scalar

#### Scenario: Runtime alignment follows runtime level

- **WHEN** `get_simd_alignment()` is called
- **THEN** it returns the alignment implied by the reported runtime SIMD level

---

### Requirement: Shared SIMD kernels

THE Example_Module SHALL provide reusable vector kernels through the canonical SIMD module rather than duplicating them across demos and benchmarks.

#### Scenario: Shared add and dot kernels stay correct

- **WHEN** callers execute shared SIMD kernels such as array add or dot product
- **THEN** the results match the scalar reference within floating-point tolerance

#### Scenario: Shared scalar-transform kernels stay correct

- **WHEN** callers execute shared SIMD kernels such as multiply, scale, or clamp
- **THEN** the results match the scalar reference for aligned and tail-length inputs
