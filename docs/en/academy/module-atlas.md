# Module Atlas

This atlas connects the documentation narrative to the code that actually carries the argument. Each row names a teaching module, the files that embody it, the validation surfaces that keep it honest, and the best entry page for deeper study.

## Teaching modules

| Module | Primary code surfaces | Reusable or supporting surfaces | Validation surfaces | Recommended docs route |
| --- | --- | --- | --- | --- |
| **01. Modern CMake** | `examples/01-cmake-modern/`, root `CMakeLists.txt`, `cmake/` | `CMakePresets.json` | preset configure and target builds | [Quick Start](/en/getting-started/quickstart) |
| **02. Memory & cache** | `examples/02-memory-cache/`, `examples/02-memory-cache/include/memory_utils.hpp`, `examples/02-memory-cache/include/particle_types.hpp` | `benchmarks/common/benchmark_utils.hpp` | `tests/unit/memory/`, `tests/property/memory_properties.cpp`, release benchmarks | [Learning Path](/en/guides/learning-path) |
| **03. Modern C++** | `examples/03-modern-cpp/`, `examples/03-modern-cpp/include/buffer.hpp`, `examples/03-modern-cpp/include/compile_time.hpp`, `examples/03-modern-cpp/include/ranges_utils.hpp`, `examples/03-modern-cpp/include/vector_reserve.hpp` | `include/hpc/core.hpp` | `tests/unit/modern_cpp/`, targeted benchmarks | [Best Practices](/en/guides/best-practices) |
| **04. SIMD vectorization** | `examples/04-simd-vectorization/`, `examples/04-simd-vectorization/include/simd_utils.hpp`, `examples/04-simd-vectorization/include/simd_wrapper.hpp` | `examples/04-simd-vectorization/src/dispatch_example_main.cpp` | `tests/unit/simd/`, `tests/property/simd_properties.cpp`, `build/release/examples/04-simd-vectorization/simd_bench` | [Optimization Decision Tree](/en/guides/optimization-decision-tree) |
| **05. Concurrency** | `examples/05-concurrency/`, `examples/05-concurrency/include/concurrency_utils.hpp`, `examples/05-concurrency/include/lock_free_queue.hpp` | OpenMP-enabled examples under `examples/05-concurrency/src/` | `tests/unit/concurrency/`, `tests/property/concurrency_properties.cpp`, `tsan` | [Validation Doctrine](/en/academy/validation-doctrine) |

## Cross-cutting repository surfaces

| Surface | Why it matters | Typical question it answers |
| --- | --- | --- |
| `include/hpc/core.hpp` | Shared low-level helpers and repository-wide utility surface | What is reusable outside a single teaching module? |
| `benchmarks/common/benchmark_utils.hpp` | Common benchmark harness helpers | How are measurements normalized and exported? |
| `tests/unit/` | Fine-grained correctness checks | Which behavior is considered contract-level? |
| `tests/property/` | Invariant-oriented validation using RapidCheck | Which assumptions are stressed across many inputs? |
| `tools/performance/` | FlameGraph and benchmark-comparison utilities | How do I move from raw numbers to diagnosis? |
| `docs/` | Whitepaper, guides, and reference routes | Where is the explanation that accompanies the executable surface? |
| `openspec/` | Change intent, requirements, and archive-ready decisions | Why was a structural change made, and what was it supposed to achieve? |

## How to use this atlas

1. **Choose the performance question first.** If the question is about cache locality, start from the memory row, not from a generic reference page.
2. **Inspect the closest executable surface.** Example modules carry the primary teaching argument.
3. **Find the matching evidence surface.** Unit tests, property tests, sanitizer presets, and benchmarks each answer different kinds of questions.
4. **Only then generalize.** The whitepaper aims to keep broad advice tied to a local, inspectable workload.

## Common traversal patterns

| If you are asking... | Start with | Then inspect |
| --- | --- | --- |
| Why does data layout matter here? | `examples/02-memory-cache/` | [Profiling Guide](/en/guides/profiling-guide) and release benchmarks |
| How portable is the SIMD story? | `examples/04-simd-vectorization/` | runtime dispatch example, SIMD tests, and [Research References](/en/research/references) |
| What memory-ordering discipline is being taught? | `examples/05-concurrency/` | queue tests, `tsan`, and [Performance Methodology](/en/architecture/performance-methodology) |
| How does the build stay reproducible? | `CMakePresets.json` and `cmake/` | [Repository Topology](/en/architecture/repository-topology) |

The atlas is intentionally concrete. It exists so that expert readers can move from prose to files without losing the argument.
