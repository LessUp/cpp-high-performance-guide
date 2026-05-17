# Related Work

This repository sits between a teaching guide, an executable notebook, and an archive-ready maintenance artifact. The works below matter because they illuminate one of those three roles.

## Measurement and profiling infrastructure

| Work | Type | Why it matters here | How this repository differs |
| --- | --- | --- | --- |
| [Google Benchmark](https://github.com/google/benchmark) | public repository and benchmarking library | many example benchmarks in the C++ ecosystem use its fixture and reporting model | this repository uses benchmark executables as evidence, but the project itself is not a benchmark framework |
| [Brendan Gregg's FlameGraph](https://github.com/brendangregg/FlameGraph) | public repository and methodology | underpins the flamegraph workflow referenced from `tools/performance/` and profiling docs | this repository teaches when to use flamegraphs, not how to maintain the tooling itself |
| [perf wiki](https://perf.wiki.kernel.org/) | official project documentation | frames the Linux-first counter and sampling workflows used in the playbook | this repository narrows the guidance to concrete example workloads |
| [Intel VTune Profiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) | vendor tool documentation | represents the more detailed vendor-grade profiling path for Intel targets | VTune is discussed as an escalation path, not as a required dependency |

## SIMD and low-level optimization references

| Work | Type | Why it matters here | How this repository differs |
| --- | --- | --- | --- |
| [xsimd](https://github.com/xtensor-stack/xsimd) | public repository and SIMD abstraction library | shows one mature approach to portable SIMD wrappers in modern C++ | this repository keeps a smaller teaching wrapper to expose the trade-off directly |
| [Agner Fog optimization manuals](https://www.agner.org/optimize/) | manuals and articles | provide the microarchitectural background for instruction throughput, latency, and CPU behavior | the repository turns those ideas into small runnable examples rather than exhaustive architecture catalogs |
| [Intel 64 and IA-32 Architectures Optimization Reference Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel64-and-ia32-architectures-optimization.html) | vendor optimization manual | useful when interpreting SIMD and cache-sensitive results on Intel hardware | the repository stays vendor-neutral in structure and uses Intel material as one authoritative source among several |
| [Compiler Explorer](https://godbolt.org/) | public tooling site | invaluable for checking generated code and vectorization decisions | the repository keeps assembly inspection adjacent to, but separate from, local benchmark evidence |

## Concurrency and industrial C++ infrastructure

| Work | Type | Why it matters here | How this repository differs |
| --- | --- | --- | --- |
| [oneTBB](https://github.com/oneapi-src/oneTBB) | public repository and tasking library | a reference point for scalable parallel patterns and production-grade scheduling | this repository teaches fundamentals such as atomics, queues, and OpenMP rather than adopting a full task runtime |
| [folly](https://github.com/facebook/folly) | public repository | demonstrates high-performance data structures and systems-oriented utility design at production scale | this repository favors small, inspectable teaching examples over large utility surfaces |
| [Abseil C++](https://github.com/abseil/abseil-cpp) | public repository | useful as a contrast in API design, portability, and low-level utility engineering | the repository is a guide with examples, not a broad dependency library |
| [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) | living guideline set | supplies durable language and design guidance that complements performance work | the repository focuses on measurable performance topics, not the full range of C++ design guidance |

## What this comparison clarifies

Taken together, these works show what this project is trying to be:

- **not** a replacement for production utility libraries such as oneTBB, folly, or Abseil
- **not** a standalone benchmarking or profiling tool
- **not** a generic language tutorial divorced from executable evidence
- **instead**, a compact performance-engineering guide that keeps its claims close to code, validation, and public references

That narrower scope is part of the repository's long-term maintainability story.
