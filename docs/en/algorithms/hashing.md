# Hashing & Hash Tables

Hash tables are the most frequently used data structure in performance-critical code after arrays. Their speed is not a mystery: it is the result of careful engineering around cache lines, load factors, and probe sequences.

---

## Overview

| Technique | Lookup | Insert | Delete | Memory | Cache | Notes |
|-----------|--------|--------|--------|--------|-------|-------|
| Separate chaining | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(n)" /> + overhead | Poor | Pointers scatter memory; simple |
| Linear probing | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(n)" /> | Good | Primary clustering; best cache behavior |
| Robin Hood | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(n)" /> | Good | Bounded probe distance; deletion via tombstones |
| Cuckoo hashing | <ComplexityBadge value="O(1)" /> worst | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> worst | <ComplexityBadge value="O(n)" /> | Moderate | Two tables + rehash on cycle |
| Swiss Tables (F14) | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(1)" /> exp | <ComplexityBadge value="O(n)" /> | Excellent | SIMD group probing; flat layout |

## Hash Function Quality

Before table layout matters, the hash function must distribute keys uniformly. A poor hash function can turn any open-addressing table into a linked list.

### What makes a hash function fast in C++

1. **Avoid modulo by prime.** Prefer power-of-two table sizes and bitmasking (`hash & (size - 1)`). This is one integer op instead of a division.
2. **Avalanche property.** A single-bit change in the key should flip approximately half the bits in the hash output.
3. **Specialization for common types.** Integer hashing should mix the value; string hashing should process the string in word-sized chunks.

```cpp
// A fast, decent integer hash (MurmurHash3 finalizer)
uint64_t hash_u64(uint64_t x) {
  x ^= x >> 33;
  x *= 0xff51afd7ed558ccdULL;
  x ^= x >> 33;
  x *= 0xc4ceb9fe1a85ec53ULL;
  x ^= x >> 33;
  return x;
}
```

For strings, the repository uses a FNV-1a variant or hardware CRC32 where available. See `examples/02-memory-cache/` for a benchmark comparing hash functions on the same key distribution.

## Linear Probing and Its Limits

Linear probing is the simplest open-addressing scheme: if slot `h(key)` is occupied, try `(h(key) + 1) % M`, then `+2`, and so on.

### Why it is fast

- **Cache-friendly.** Probing a contiguous array means prefetchers and cache lines work in your favor.
- **Simple code.** No extra indirections, no pointer chasing.

### Why it can fail

- **Primary clustering.** Contiguous runs of occupied slots form, increasing average probe distance.
- **Sensitive to load factor.** Above 0.7–0.75, performance degrades rapidly.

```cpp
// Simplified linear-probing lookup
Value* find(const Key& key) {
  size_t idx = hash(key) & mask;
  while (states[idx] != EMPTY) {
    if (states[idx] == OCCUPIED && keys[idx] == key) return &values[idx];
    idx = (idx + 1) & mask;
  }
  return nullptr;
}
```

## Robin Hood Hashing

Robin Hood hashing bounds the maximum probe distance by displacing existing entries when a new entry would have a longer probe sequence.

### The Robin Hood principle

When inserting, if the new element's distance-from-ideal-slot is *greater* than the existing element's distance, swap them and continue probing with the displaced element. This keeps the variance of probe distances low.

### Deletion

Standard open-addressing deletion requires tombstones. Robin Hood tables can use backward shifting: on deletion, scan forward and shift elements back to their ideal positions if possible.

### SIMD-accelerated Robin Hood

Some implementations (e.g., `ska::flat_hash_map`) store metadata bytes (hash fragments) in a separate parallel array. Lookup first compares 16 metadata bytes at once with SSE/NEON, then only checks keys on matching fragments. This reduces cache misses and key comparisons.

## Swiss Tables and F14

Facebook's F14 and Abseil's Swiss Tables represent the current state of the art for general-purpose hash tables in C++.

### Key design ideas

1. **Group probing.** The table is divided into groups of 16 slots. A SIMD comparison checks all 16 control bytes in parallel to find candidates.
2. **Flat layout.** Keys and values are stored inline in a single array; no separate chaining or pointer indirection.
3. **Two-tier storage.** F14 uses a small metadata array (control bytes) and a separate value array, allowing values to be reallocated without moving keys.

```cpp
// Conceptual SIMD group probe (SSE2/NEON)
// control_bytes: 16 bytes, each is (hash_fragment | special_states)
// target: 8-bit hash fragment of key
uint16_t match = simd_eq_16x8(control_bytes, target);
while (match) {
  int slot = ctz(match);  // count trailing zeros
  if (keys[slot] == key) return &values[slot];
  match &= match - 1;     // clear lowest set bit
}
```

### When to use Swiss Tables

- General-purpose maps where lookup dominates.
- When `absl::flat_hash_map` or `folly::F14VectorMap` is already available as a dependency.
- When the repository's teaching-sized implementation is not enough for production workloads.

## Practical Selection Guide

<DiagramCanvas title="Hash Table Selection" caption="Match workload characteristics to table family.">

```mermaid
flowchart TD
    A[Hash table needed] --> B{Size known upfront?}
    B -->|Yes| C{Lookup-heavy?}
    B -->|No| D[Robin Hood<br/>or F14Vector]
    C -->|Yes| E{SIMD available?}
    C -->|No| F[Robin Hood<br/>linear probing]
    E -->|Yes| G[Swiss Tables / F14<br/>group probing]
    E -->|No| H[Robin Hood<br/>metadata probing]
    D --> I{Deletion-heavy?}
    I -->|Yes| J[F14<br/>two-tier storage]
    I -->|No| K[Robin Hood<br/>backward shift]
    F --> L[Verify with<br/>benchmark]
    G --> L
    H --> L
    J --> L
    K --> L

    classDef decision fill:var(--wp-diagram-decision),stroke:var(--wp-diagram-decision-stroke)
    classDef action fill:var(--wp-diagram-action),stroke:var(--wp-diagram-action-stroke)
    classDef complete fill:var(--wp-diagram-complete),stroke:var(--wp-diagram-complete-stroke)
    classDef start fill:var(--wp-diagram-start),stroke:var(--wp-diagram-start-stroke)

    class A start
    class B,C,E,I decision
    class D,F,G,H,J,K action
    class L complete
```

</DiagramCanvas>

## Benchmark Notes

The repository benchmarks hash tables with:

1. **Insertion burst.** Insert N elements, measure cycles/insertion.
2. **Lookup mix.** 80% successful lookups, 20% misses (typical cache pattern).
3. **Deletion pattern.** Delete half, re-insert half, measure rehash rate.
4. **Memory footprint.** RSS at peak and after steady state.

All benchmarks run with `perf stat -e cycles,cache-misses,branch-misses`.

## References

<Citation :references="[
  { id: 'celis1985', author: 'Celis, P.', title: 'Robin Hood Hashing', year: 1985, source: 'PhD thesis, University of Waterloo', url: 'https://cs.uwaterloo.ca/research/tr/1985/CS-85-14.pdf' },
  { id: 'leiserson2020', author: 'Leiserson, C. E. et al.', title: 'F14: A Memory-Efficient Hash Table', year: 2020, source: 'Facebook Engineering', url: 'https://engineering.fb.com/2019/04/25/developer-tools/f14/' },
  { id: 'lemire2018', author: 'Lemire, D.', title: 'Fast Random Integer Generation in an Interval', year: 2018, source: 'ACM Trans. Model. Comput. Simul.', url: 'https://doi.org/10.1145/3230636' },
  { id: 'absl2024', author: 'Abseil Authors', title: 'Swiss Tables Design Notes', year: 2024, source: 'Abseil C++ Documentation', url: 'https://abseil.io/about/design/swisstables' },
]" />
