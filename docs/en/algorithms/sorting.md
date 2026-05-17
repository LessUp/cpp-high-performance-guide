# Sorting Algorithms

Sorting is the canonical algorithmic problem. In high-performance C++, the interesting question is rarely "which sort is theoretically optimal" and usually "which sort is optimal *for this data shape on this microarchitecture*."

---

## Overview

| Algorithm | Best Time | Average Time | Worst Time | Space | Stable | Notes |
|-----------|-----------|--------------|------------|-------|--------|-------|
| Quicksort (Hoare) | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n²)" /> | <ComplexityBadge value="O(log n)" /> | No | Cache-friendly in practice; pivot choice matters |
| Introsort | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(log n)" /> | No | Quicksort + heapsort fallback; `std::sort` uses this |
| Merge sort | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n)" /> | Yes | Predictable, cache-friendly; good for linked data |
| Timsort | <ComplexityBadge value="O(n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n)" /> | Yes | Exploits runs in real-world data |
| Radix sort (LSD) | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(n + r)" /> | Yes | k = key width, r = radix; dominates for fixed-width keys |
| Bitonic sort | <ComplexityBadge value="O(log² n)" /> | <ComplexityBadge value="O(log² n)" /> | <ComplexityBadge value="O(log² n)" /> | <ComplexityBadge value="O(1)" /> | No | Data-parallel; excellent on SIMD/GPU |

## Quicksort and Its Engineering

### Why quicksort dominates in practice

Despite the <ComplexityBadge value="O(n²)" /> worst case, quicksort's average-case cache behavior and low instruction count make it the default choice for general-purpose sorting in C++ (`std::sort` is typically an introsort variant).

**Key engineering decisions:**

1. **Pivot selection.** Median-of-three or median-of-medians reduces pathological inputs. Some implementations sample more pivots to avoid adversarial cases.
2. **Partition scheme.** Hoare's partition visits each element once; Lomuto's is simpler but performs more swaps. Modern implementations favor Hoare or a hybrid.
3. **Small-array fallback.** When subarrays drop below a threshold (typically 16–32 elements), insertion sort is faster due to lower overhead.
4. **Tail-call optimization.** Recursing on the smaller partition first and looping on the larger keeps stack depth at <ComplexityBadge value="O(log n)" />.

### Cache behavior

Quicksort is *not* inherently cache-optimal, but its divide-and-conquer structure tends to keep working sets within cache for moderate input sizes. The critical metric is the number of cache misses per element:

```cpp
// A cache-conscious quicksort sketch
void quicksort(int* first, int* last) {
  while (last - first > 32) {
    auto* pivot = hoare_partition(first, last);
    if (pivot - first < last - pivot) {
      quicksort(first, pivot);
      first = pivot + 1;
    } else {
      quicksort(pivot + 1, last);
      last = pivot;
    }
  }
  insertion_sort(first, last);
}
```

## Merge Sort and Stability

Merge sort guarantees <ComplexityBadge value="O(n log n)" /> and stability, at the cost of <ComplexityBadge value="O(n)" /> auxiliary memory. In C++, `std::stable_sort` uses a merge-sort variant.

### Memory layout optimization

The naive merge sort allocates a temporary buffer for every merge. A better approach:

- Allocate one temporary array of size `n` at the top level.
- Alternate merging direction (A→B, then B→A) to avoid copies.
- This reduces allocation overhead from <ComplexityBadge value="O(n log n)" /> to <ComplexityBadge value="O(n)" />.

## Radix Sort: When Big-O Loses

For fixed-width integer keys, radix sort can achieve linear time and outperform comparison sorts by 3–10x. The catch: it is not comparison-based and requires extra memory.

### LSD (Least Significant Digit)

```cpp
// Simplified LSD radix sort for 32-bit unsigned integers
void lsd_radix_sort(uint32_t* data, size_t n) {
  constexpr int bits_per_pass = 8;
  constexpr int buckets = 1 << bits_per_pass;
  constexpr int mask = buckets - 1;
  std::vector<uint32_t> temp(n);

  for (int shift = 0; shift < 32; shift += bits_per_pass) {
    size_t count[buckets] = {};
    for (size_t i = 0; i < n; ++i) {
      ++count[(data[i] >> shift) & mask];
    }
    size_t prefix = 0;
    for (int b = 0; b < buckets; ++b) {
      size_t c = count[b];
      count[b] = prefix;
      prefix += c;
    }
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = data[i];
      size_t bucket = (key >> shift) & mask;
      temp[count[bucket]++] = key;
    }
    std::swap(data, temp);
  }
}
```

**Performance characteristics:**
- 4 passes for 32-bit keys (8 bits per pass).
- Each pass is a linear scan: cache-friendly if `count` fits in L1.
- Not stable unless implemented with careful offset tracking (the version above is stable).

### SIMD opportunities

Some radix sort variants use SIMD to accelerate the histogram (counting) phase. AVX2 can process 8 integers at once, reducing the serial dependency of the counting loop. See the repository's `examples/04-simd-vectorization/` for vectorization patterns applicable to histogram computation.

## Bitonic Sort for Parallel Hardware

Bitonic sort is a data-parallel algorithm with <ComplexityBadge value="O(log² n)" /> depth, making it ideal for SIMD and GPU execution. It is not comparison-efficient for sequential CPUs but shines when wide vector units are available.

The key operation is the **bitonic merge**: compare-and-swap elements across a stride, then halve the stride. On AVX-512, a bitonic network of 16 elements fits in one register and can be sorted with a small sequence of permute-and-compare instructions.

## Practical Selection Guide

<DiagramCanvas title="Sort Selection Decision" caption="Use this flow to narrow the sort family before profiling.">

```mermaid
flowchart TD
    A[Need to sort?] --> B{Stability required?}
    B -->|Yes| C{Linked / pointer-heavy?}
    B -->|No| D{Integer / fixed-width keys?}
    C -->|Yes| E[Merge sort<br/>or Timsort]
    C -->|No| F[Stable introsort<br/>std::stable_sort]
    D -->|Yes| G{Key distribution known?}
    D -->|No| H[Introsort<br/>std::sort]
    G -->|Uniform / dense| I[Radix sort<br/>LSD]
    G -->|Sparse / variable| H
    H --> J{Small N < 1e5?}
    J -->|Yes| K[Quicksort<br/>cache-friendly]
    J -->|No| L[Introsort<br/>+ branchless partitioning]
    E --> M[Verify with<br/>benchmark]
    F --> M
    I --> M
    K --> M
    L --> M

    classDef decision fill:var(--wp-diagram-decision),stroke:var(--wp-diagram-decision-stroke)
    classDef action fill:var(--wp-diagram-action),stroke:var(--wp-diagram-action-stroke)
    classDef complete fill:var(--wp-diagram-complete),stroke:var(--wp-diagram-complete-stroke)
    classDef start fill:var(--wp-diagram-start),stroke:var(--wp-diagram-start-stroke)

    class A start
    class B,D,C,G,J decision
    class E,F,I,H,K,L action
    class M complete
```

</DiagramCanvas>

## Benchmark Notes

The repository's benchmark workflow follows this protocol:

1. Generate input distributions: random, sorted, reverse-sorted, few-unique, adversarial (median-of-medians killer).
2. Warm up the CPU and pin threads.
3. Report cycles/element and LLC misses/element from `perf stat -e cycles,cache-misses`.
4. Compare against `std::sort`, `std::stable_sort`, and `absl::c_sort` where available.

## References

<Citation :references="[
  { id: 'sedgewick2011', author: 'Sedgewick, R. & Wayne, K.', title: 'Algorithms, 4th Edition', year: 2011, source: 'Addison-Wesley', url: 'https://algs4.cs.princeton.edu/home/' },
  { id: 'musser1997', author: 'Musser, D. R.', title: 'Introspective Sorting and Selection Algorithms', year: 1997, source: 'Software: Practice and Experience, 27(8)', url: 'https://doi.org/10.1002/(SICI)1097-024X(199708)27:8<983::AID-SPE117>3.0.CO;2-#' },
  { id: 'lemire2019', author: 'Lemire, D. & Boytsov, L.', title: 'Decoding billions of integers per second through vectorization', year: 2019, source: 'Software: Practice and Experience', url: 'https://arxiv.org/abs/1209.1717' },
  { id: 'mcllroy1999', author: 'McIlroy, P. M.', title: 'A Killer Adversary for Quicksort', year: 1999, source: 'Software: Practice and Experience, 29(4)', url: 'https://doi.org/10.1002/(SICI)1097-024X(19990325)29:4<341::AID-SPE258>3.0.CO;2-9' },
]" />
