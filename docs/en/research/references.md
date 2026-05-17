# References

This page collects the external sources most useful for understanding the repository's code, validation model, and performance methodology. The emphasis is on durable, official, and widely cited references.

## Standards and language references

| Source | Kind | Why it matters here |
| --- | --- | --- |
| [cppreference: C++ standard library and language reference](https://en.cppreference.com/w/) | reference site | quickest authoritative lookup for atomics, ranges, alignment, and language rules used across the examples |
| [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) | living guidelines | useful for balancing performance-oriented code with safety and maintainability |
| [CMake Presets manual](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) | official manual | directly relevant because this repository treats presets as the canonical build and validation interface |
| [GoogleTest](https://github.com/google/googletest) | public repository | unit-test infrastructure used by the repository |
| [RapidCheck](https://github.com/emil-e/rapidcheck) | public repository | property-based testing library used for invariant-oriented validation |

## Measurement and profiling

| Source | Kind | Why it matters here |
| --- | --- | --- |
| [Google Benchmark](https://github.com/google/benchmark) | public repository | microbenchmark harness used throughout the ecosystem and reflected in this repository's measurement style |
| [perf wiki](https://perf.wiki.kernel.org/) | official documentation | canonical Linux-first reference for counters, sampling, and profiler usage |
| [Brendan Gregg's FlameGraph](https://www.brendangregg.com/flamegraphs.html) | article and tooling entry point | explains the flamegraph model used by the repository's performance scripts |
| [Intel VTune Profiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) | vendor documentation | advanced profiling route for Intel-focused investigations |
| [Compiler Explorer](https://godbolt.org/) | public tooling site | convenient for checking code generation and vectorization decisions outside a local build |

## Microarchitecture and systems-performance reading

| Source | Kind | Why it matters here |
| --- | --- | --- |
| [Agner Fog optimization resources](https://www.agner.org/optimize/) | manuals and tables | high-value reference for instruction costs, vectorization details, and CPU behavior |
| [Intel 64 and IA-32 Architectures Optimization Reference Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel64-and-ia32-architectures-optimization.html) | vendor optimization manual | helps interpret many cache and SIMD results on mainstream x86 systems |
| [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf) | long-form article | foundational reading for the memory and cache module |
| [Gallery of Processor Cache Effects](http://igoro.com/archive/gallery-of-processor-cache-effects/) | article | approachable illustrations of cache behavior that complement the repository's examples |

## Algorithms and data structures

| Source | Kind | Why it matters here |
| --- | --- | --- |
| [Sedgewick & Wayne, *Algorithms* (4th ed.)](https://algs4.cs.princeton.edu/home/) | textbook | canonical reference for the sorting, searching, and graph algorithms discussed in the repository |
| [Knuth, *The Art of Computer Programming* Vol. 3](https://www-cs-faculty.stanford.edu/~knuth/taocp.html) | textbook | definitive analysis of sorting and searching algorithms; the benchmark against which all teaching references are measured |
| [Celis (1985), *Robin Hood Hashing*](https://cs.uwaterloo.ca/research/tr/1985/CS-85-14.pdf) | PhD thesis | foundational paper for the Robin Hood hashing discussion in the algorithms module |
| [Facebook F14 Design Notes](https://engineering.fb.com/2019/04/25/developer-tools/f14/) | engineering blog | explains the group-probing, SIMD-accelerated hash table design referenced in the hashing guide |
| [Abseil Swiss Tables](https://abseil.io/about/design/swisstables) | public documentation | the design rationale behind `absl::flat_hash_map`, a direct comparison point for the repository's hash-table teaching implementation |
| [Lemire & Boytsov (2019), *Decoding billions of integers per second through vectorization*](https://arxiv.org/abs/1209.1717) | research paper | demonstrates SIMD-friendly algorithm design applicable to sorting, compression, and bitmap indexing |
| [McIlroy (1999), *A Killer Adversary for Quicksort*](https://doi.org/10.1002/(SICI)1097-024X(19990325)29:4<341::AID-SPE258>3.0.CO;2-9) | research paper | shows why robust pivot selection matters in real-world quicksort implementations |

## Public repositories and libraries used as comparison points

| Source | Kind | Why it matters here |
| --- | --- | --- |
| [xsimd](https://github.com/xtensor-stack/xsimd) | public repository | contrast point for portable SIMD abstractions |
| [oneTBB](https://github.com/oneapi-src/oneTBB) | public repository | reference for production-grade parallelism and scheduling |
| [folly](https://github.com/facebook/folly) | public repository | shows what a much broader systems utility library looks like |
| [Abseil C++](https://github.com/abseil/abseil-cpp) | public repository | useful comparison for API surface and low-level utility design |
| [ska::flat_hash_map](https://github.com/skarupke/flat_hash_map) | public repository | teaching-sized Robin Hood hash map; close in scope to the repository's hash-table examples |
| [tsl::robin_map](https://github.com/Tessil/robin-map) | public repository | another Robin Hood implementation with emphasis on API compatibility and performance |

## Repository cross-links

- [Performance Methodology](/en/architecture/performance-methodology)
- [Validation Doctrine](/en/academy/validation-doctrine)
- [Profiling Guide](/en/guides/profiling-guide)
- [Related Work](/en/research/related-work)
- [Algorithms Overview](/en/algorithms/)

This reference shelf is intentionally selective. It favors sources that help readers inspect or challenge a concrete claim in the repository.
