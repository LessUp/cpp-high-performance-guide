# Tasks: Deepen Runtime SIMD Module

## 1. Spec and red tests

- [x] 1.1 Add the OpenSpec proposal, design, tasks, and SIMD spec delta for the runtime SIMD module deepening change
- [x] 1.2 Add failing tests for the new public SIMD seam in `tests/unit/simd/`
- [x] 1.3 Run the focused SIMD test target and confirm the new tests fail for the expected reason

## 2. Public module implementation

- [x] 2.1 Add `include/hpc/simd.hpp` with runtime detection, alignment helpers, wrapper types, and reusable kernels
- [x] 2.2 Refactor the SIMD tests to include the new public header
- [x] 2.3 Refactor the SIMD examples and benchmark to use the new public seam and delete duplicated kernel logic
- [x] 2.4 Minimize or remove obsolete example-local SIMD forwarding headers

## 3. Coupled platform hardening

- [x] 3.1 Fix the Windows `page_size()` compile path in `include/hpc/core.hpp`
- [x] 3.2 Re-run the focused SIMD and core tests

## 4. Full validation

- [x] 4.1 Run `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug`
- [x] 4.2 Run sanitizer validation for the touched surface
- [x] 4.3 Summarize the architectural deepening and any deferred opportunities
