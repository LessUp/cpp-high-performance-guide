# Design: Header-Only Hardening

## Overview

This change deepens three fragile modules by moving hidden invariants behind their own interfaces instead of forcing callers and future maintainers to remember them.

## Design Decisions

### 1. `MPMCQueue` owns payload lifetime explicitly

The queue cell will store `std::optional<T>` instead of a permanently live `T`. This removes the accidental default-constructible requirement and gives the module a real interface for generic payloads. The queue will also expose an rvalue enqueue path so move-only and expensive-to-copy types can cross the seam safely.

### 2. `Buffer` processing absorbs the empty-buffer invariant

The move-semantics example currently requires callers to know that `buf.data()[0]` is only valid when `size() > 0`. That is a shallow interface. The hardening change moves the empty-buffer check into the processing helpers so the module remains safe for default-constructed and zero-sized buffers.

### 3. Modern C++ example headers become explicitly link-safe

The repository treats these example headers as header-only modules. Non-template free functions defined in headers will be marked `inline`, and a dedicated multi-translation-unit smoke target will prove the contract during normal builds.

## Testing Strategy

1. Add a failing queue test that uses non-default-constructible and move-only payloads.
2. Add a failing move-semantics test that exercises empty buffers.
3. Add a failing multi-translation-unit smoke target for the modern C++ headers.
4. Apply the minimal production changes to make each loop pass.
5. Re-run the full preset-driven debug validation path.
