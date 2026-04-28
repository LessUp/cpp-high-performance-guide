# SIMD Vectorization

## ADDED Requirements

### Requirement: Runtime CPU Dispatch

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

- **WHEN** a reader builds the SIMD examples with GCC (`-fopt-info-vec`) or Clang (`-Rpass=loop-vectorize`)
- **THEN** the module README provides the exact command and an explanation of the output
