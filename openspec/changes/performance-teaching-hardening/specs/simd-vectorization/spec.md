# SIMD Vectorization

## ADDED Requirements

### Requirement: Runtime CPU Dispatch

> **Scope note**: This requirement adds *function-level runtime dispatch* for a single binary (selecting AVX2, SSE2, or scalar at call time via `__builtin_cpu_supports`). It extends the existing compile-time CPU capability / build-system requirement already covered by this module; it does not replace it. The compile-time path (e.g., `-mavx2` flag, CMake feature detection) remains in effect for all other targets.

THE Example_Module SHALL provide a runtime CPU dispatch example that selects the highest-available SIMD instruction set at runtime.

#### Scenario: Runtime dispatch selects correct path

- **WHEN** `dispatch_add_arrays` is called on a system with AVX2
- **THEN** the AVX2 code path is selected and results match the scalar reference within floating-point tolerance

#### Scenario: Runtime dispatch falls back gracefully

- **WHEN** `dispatch_add_arrays` is called on a system without AVX2 or SSE2
- **THEN** the scalar fallback path is used and results are correct

---

### Requirement: Vectorization Diagnostics Workflow

THE Documentation SHALL document how to obtain compiler vectorization reports for the SIMD examples.

#### Scenario: Reader enables vectorization diagnostics

- **WHEN** a reader builds the SIMD examples with GCC (`-fopt-info-vec-optimized`) or Clang (`-Rpass=loop-vectorize`)
- **THEN** the module README provides the exact command and an explanation of the output
