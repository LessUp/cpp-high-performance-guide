# Proposal: Header-Only Hardening

## Summary

Harden the header-only teaching modules where the current interface promises more than the implementation can safely deliver, focusing on queue payload support, empty-buffer safety, and multi-translation-unit link safety.

## Motivation

The repository is in closure and hardening mode. Several teaching modules already pass today's tests, but still carry design drift that weakens long-term maintainability:

1. `MPMCQueue` documents generic payload support, yet its implementation requires default-constructible element types and does not expose a move-oriented enqueue path.
2. The move-semantics example leaks a hidden caller invariant: `process_by_copy` and `process_by_ref` dereference element zero even when the buffer is empty.
3. Header-defined non-template functions in the modern C++ examples are not explicitly `inline`, which makes the header-only contract fragile across multiple translation units.

## Scope

- Harden the concurrency queue interface so the implementation matches its documented payload story.
- Harden the move-semantics example against empty-buffer use.
- Enforce link-safe header-only behavior in modern C++ example headers.
- Add targeted regression coverage for each fix.

## Out of Scope

- New performance claims or benchmark rewrites.
- Broad API redesign across unrelated modules.
- Reworking the docs site or unrelated repository governance surfaces.
