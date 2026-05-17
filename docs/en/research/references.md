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

## Public repositories and libraries used as comparison points

| Source | Kind | Why it matters here |
| --- | --- | --- |
| [xsimd](https://github.com/xtensor-stack/xsimd) | public repository | contrast point for portable SIMD abstractions |
| [oneTBB](https://github.com/oneapi-src/oneTBB) | public repository | reference for production-grade parallelism and scheduling |
| [folly](https://github.com/facebook/folly) | public repository | shows what a much broader systems utility library looks like |
| [Abseil C++](https://github.com/abseil/abseil-cpp) | public repository | useful comparison for API surface and low-level utility design |

## Repository cross-links

- [Performance Methodology](/en/architecture/performance-methodology)
- [Validation Doctrine](/en/academy/validation-doctrine)
- [Profiling Guide](/en/guides/profiling-guide)
- [Related Work](/en/research/related-work)

This reference shelf is intentionally selective. It favors sources that help readers inspect or challenge a concrete claim in the repository.
