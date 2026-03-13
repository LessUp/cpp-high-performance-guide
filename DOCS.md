# Documentation Home

This site is the documentation entry for the C++ High Performance Computing Optimization Guide. Start here when you want a reading path, a language-specific guide, or direct access to the example modules.

## Project Positioning

- Learn modern C++ performance engineering through runnable examples instead of isolated notes.
- Cover the path from build system fundamentals to memory and cache behavior, SIMD, concurrency, benchmarking, and profiling.
- Keep the repository README focused on repository setup while this page focuses on documentation entry points and reading order.

## Who This Is For

- C++ engineers who want a structured path into performance optimization.
- Students and self-learners comparing optimization techniques with executable examples.
- Maintainers who need profiling, benchmarking, and contribution references.

## Start Here

- English readers: [Learning Path](docs/en/learning-path.md)
- 中文读者: [学习路径](docs/zh/learning-path.md)
- Need tooling first: [Profiling Guide](docs/en/profiling-guide.md) and [性能分析指南](docs/zh/profiling-guide.md)
- Need site sync details: [GitBook Sync Guide](docs/en/gitbook-sync.md) and [GitBook 接入指南](docs/zh/gitbook-sync.md)

## Recommended Reading Paths

| Goal | Recommended path |
| --- | --- |
| First visit | Learning Path -> example modules -> profiling guide |
| Benchmark or hotspot analysis | Profiling Guide -> memory and cache module -> SIMD module |
| Add or extend a module | Contributing guide -> relevant module README -> learning path |

## Core Documents

| Topic | English | 中文 |
| --- | --- | --- |
| Learning path | [Learning Path](docs/en/learning-path.md) | [学习路径](docs/zh/learning-path.md) |
| Profiling | [Profiling Guide](docs/en/profiling-guide.md) | [性能分析指南](docs/zh/profiling-guide.md) |
| Site sync | [GitBook Sync Guide](docs/en/gitbook-sync.md) | [GitBook 接入指南](docs/zh/gitbook-sync.md) |
| Contribution | [Contributing](CONTRIBUTING.md) | [参与贡献](CONTRIBUTING.zh.md) |

## Example Modules

- [01 - Modern CMake](examples/01-cmake-modern/README.md)
- [02 - Memory & Cache Optimization](examples/02-memory-cache/README.md)
- [03 - Modern C++ Features](examples/03-modern-cpp/README.md)
- [04 - SIMD Vectorization](examples/04-simd-vectorization/README.md)
- [05 - Concurrency](examples/05-concurrency/README.md)
