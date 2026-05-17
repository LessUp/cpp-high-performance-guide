# Algorithms

This module treats algorithms as executable claims. Every entry below links to runnable code, benchmark evidence, or reproducible experiments in this repository.

## Algorithm Map

<SectionIndex
  :items='[
    {
      title: "Sorting",
      description: "From comparison sorts to SIMD-accelerated radix. Performance is measured, not assumed.",
      links: [
        { href: "/en/algorithms/sorting", label: "Sorting algorithms" },
      ],
    },
    {
      title: "Hashing",
      description: "Hash functions and hash table layouts that survive cache pressure and concurrent access.",
      links: [
        { href: "/en/algorithms/hashing", label: "Hashing & tables" },
      ],
    },
  ]'
/>

## Complexity Quick Reference

| Algorithm | Time | Space | Cache Friendly | Parallel | Key Trade-off |
| --- | --- | --- | --- | --- | --- |
| Quicksort (introspective) | <ComplexityBadge value="O(n log n)" /> avg | <ComplexityBadge value="O(log n)" /> | Partial | Hard | Branch misprediction on pivot |
| Merge sort | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n)" /> | Yes | Easy | Extra memory for merging |
| Radix sort (LSD) | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(n + r)" /> | Yes | Moderate | Key width and digit size |
| Robin Hood hashing | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(n)" /> | Yes | Hard | Robin Hood shift on insert |
| Swiss Tables (F14) | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(n)" /> | Yes | Hard | SIMD probing, flat layout |

## What makes an algorithm "high-performance" here

1. **Measurable first.** A claim about complexity or throughput must be backed by a benchmark that compiles with the repository's CMake presets.
2. **Cache-aware, not just big-O aware.** Constant factors and memory access patterns often dominate asymptotic bounds on real hardware.
3. **ISA-conscious.** When vectorization changes the algorithmic approach (e.g., bitonic sort on SIMD, SIMD probing in F14), the implementation is inspected with Compiler Explorer and validated with `perf stat`.
4. **Maintainability boundary.** The repository stops before becoming a production library. Each algorithm is a teaching-sized implementation with explicit trade-off notes, not an exhaustive optimization catalog.

## Reading order

1. Start with [Sorting](/en/algorithms/sorting) if you want to see how classic algorithms are adapted for modern CPU pipelines.
2. Continue with [Hashing](/en/algorithms/hashing) to understand memory layout decisions that affect every hash-heavy workload.
3. Cross-reference the [Performance Methodology](/en/architecture/performance-methodology) page whenever a benchmark claim needs calibration.
