# Design: Deepen Runtime SIMD Module

## Overview

This change creates a deeper SIMD module by moving the reusable caller-facing interface into `include/hpc/simd.hpp`. The module owns:

- runtime SIMD capability detection
- runtime alignment selection
- aligned SIMD buffer aliases
- vector wrapper types (`SimdVec`, `FloatVec`)
- reusable vector kernels (`add`, `multiply`, `dot`, `scale`, `clamp`)

The examples remain teaching entry points, but they stop being the seam that tests and benchmarks must cross to reach reusable behavior.

## Architectural problem

Today the SIMD area makes maintainers bounce across multiple shallow modules:

- `examples/04-simd-vectorization/include/simd_utils.hpp`
- `examples/04-simd-vectorization/include/simd_wrapper.hpp`
- `examples/04-simd-vectorization/src/intrinsics_intro.cpp`
- `examples/04-simd-vectorization/bench/simd_bench.cpp`

The interface is scattered almost as widely as the implementation. That yields low depth, low leverage, and poor locality. The deletion test fails: deleting any one file would mostly move the same logic to another caller rather than remove complexity.

## New module shape

### Public module

`include/hpc/simd.hpp`

Responsibilities:

1. expose a single SIMD interface for callers and tests
2. detect runtime SIMD capability on supported compilers/architectures
3. derive the alignment policy from that capability
4. provide the wrapper types and high-level kernels
5. hide the target-attribute dispatch details behind the module interface

### Adapters and seams

- The module seam is `hpc::simd`
- Runtime dispatch is internal to the module implementation
- Example executables and benchmarks become adapters that demonstrate or measure the module
- Tests validate behavior through the same public seam callers use

## Runtime capability design

### Detection

`detect_simd_level()` will use `__builtin_cpu_init()` / `__builtin_cpu_supports()` on GCC/Clang x86 builds to report the highest runtime-supported level among:

- `AVX512`
- `AVX2`
- `AVX`
- `SSE2`
- `Scalar`

On unsupported compilers or architectures, the function falls back to a conservative compile-time answer.

### Alignment

`get_simd_alignment()` will derive the alignment from `detect_simd_level()` rather than from compile-time macros. That keeps the interface consistent with the actual runtime-dispatched implementation path.

## Kernel design

The public module will own these reusable kernels:

- `add_arrays`
- `multiply_arrays`
- `dot_product`
- `scale_array`
- `clamp_array`

Compatibility alias:

- `dispatch_add_arrays` forwards to `add_arrays`

This retains the existing teaching name where it is already useful while collapsing the actual implementation behind one deeper module.

## Wrapper design

`SimdVec`, `SimdVecScalar`, `FloatVec`, and `FLOAT_VEC_WIDTH` move into `include/hpc/simd.hpp`. The wrapper remains part of the teaching surface because it provides leverage for examples and benchmarks, but it is no longer isolated in a separate shallow header.

## Migration plan

1. Add tests for the new public seam first.
2. Add `include/hpc/simd.hpp`.
3. Refactor SIMD tests to include the public module.
4. Refactor examples and benchmarks to include the public module and delete duplicated scalar/SIMD kernels where the shared module now owns them.
5. Keep `examples/04-simd-vectorization/include/simd_utils.hpp` as a thin forwarding header only if needed during transition. Delete or minimize `simd_wrapper.hpp`.
6. Fix `include/hpc/core.hpp` Windows include bug because the new module depends on platform utilities remaining portable.

## Validation strategy

- Focused red/green runs for the SIMD unit tests while iterating
- Full debug preset validation before completion
- Sanitizer preset validation on the touched surface after the refactor is green
