# Tasks: Header-Only Hardening

## 1. Queue payload hardening

- [x] Add regression coverage showing `MPMCQueue` handles non-default-constructible and move-only payloads.
- [x] Refactor queue cell storage so element lifetime matches the queue seam.
- [x] Add move enqueue support without weakening existing behavior.

## 2. Empty-buffer safety

- [x] Add regression coverage for processing default-constructed and zero-sized buffers.
- [x] Move the empty-buffer invariant behind the move-semantics helper interface.

## 3. Header-only link safety

- [x] Add a multi-translation-unit smoke target that includes the modern C++ headers from more than one source file.
- [x] Mark header-defined non-template free functions `inline` where needed.

## 4. Validation

- [x] Run the targeted red/green loops for the concurrency and modern C++ test surfaces.
- [x] Run `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug`.
