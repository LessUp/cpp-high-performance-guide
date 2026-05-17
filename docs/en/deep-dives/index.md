---
type: section-index
description: Deep technical explorations of core concepts in high-performance C++ programming.
---

# Deep Dives

These deep dive articles provide comprehensive, technical explorations of fundamental concepts in high-performance C++ programming. Each article assumes familiarity with the basics and dives into implementation details, performance considerations, and practical guidelines.

## Available Deep Dives

<SectionIndex
  :cards="[
    {
      title: 'Memory Layout',
      description: 'AOS vs SOA layouts, cache line alignment, false sharing elimination, and memory prefetching strategies.',
      links: [
        { text: 'Read', href: '/en/deep-dives/memory-layout' }
      ]
    },
    {
      title: 'Lock-Free Queues',
      description: 'SPSC and MPMC queue implementations, memory ordering semantics, and lock-free algorithm design.',
      links: [
        { text: 'Read', href: '/en/deep-dives/lock-free-queue' }
      ]
    },
    {
      title: 'SIMD Internals',
      description: 'Auto-vectorization conditions, intrinsic wrappers, runtime dispatch, and AVX-512 masked operations.',
      links: [
        { text: 'Read', href: '/en/deep-dives/simd-internals' }
      ]
    }
  ]"
/>

## Prerequisites

Before diving into these advanced topics, you should be familiar with:

- **Academy**: Module Atlas and Validation Doctrine
- **Architecture**: Repository Topology and Performance Methodology
- **C++ Fundamentals**: Move semantics, templates, and the STL

## How to Read

These articles are designed for **deep understanding**, not quick reference. We recommend:

1. **Read sequentially** - Each article builds concepts progressively
2. **Experiment with code** - All examples are runnable from the examples/ directory
3. **Benchmark yourself** - Use the provided benchmarks to validate performance claims
4. **Consult references** - Each article cites primary sources for deeper study
