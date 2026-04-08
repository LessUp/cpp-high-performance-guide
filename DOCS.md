# C++ High Performance Computing Optimization Guide

> **通过可运行的示例学习现代 C++ 性能工程** — 从构建系统到内存优化、SIMD 向量化、并发编程与性能分析，一站式掌握。
>
> Learn modern C++ performance engineering through runnable examples — covering build systems, memory & cache, SIMD, concurrency, benchmarking, and profiling.

[![CI](https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/ci.yml) [![Docs](https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/pages.yml/badge.svg)](https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/pages.yml) [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/LessUp/cpp-high-performance-guide/blob/master/LICENSE)

---

## Why This Project / 为什么选这个项目

- **实战驱动** — 每个优化技术都有可编译、可运行、可度量的示例代码，而非孤立的知识笔记。
- **从入门到进阶** — 6 个阶段、5 大模块，覆盖构建系统基础到无锁编程。
- **自带基准测试** — 集成 Google Benchmark，每个模块都能量化优化前后的性能差异。
- **工业级工程实践** — CMake Presets、Sanitizers（ASan / TSan / UBSan）、CI/CD、FlameGraph 全覆盖。
- **双语文档** — 中英文学习路径与性能分析指南同步维护。

## Tech Stack / 技术栈

| Category | Details |
| --- | --- |
| **Language** | C++20 |
| **Compilers** | GCC 11+, Clang 14+ |
| **Build** | CMake 3.22+, CMake Presets |
| **Testing** | Google Test, RapidCheck (property tests) |
| **Benchmarking** | Google Benchmark |
| **Profiling** | perf, FlameGraph, Valgrind, Intel VTune |
| **CI/CD** | GitHub Actions (Build, Sanitizers, Pages) |
| **Docs** | HonKit → GitHub Pages |

## Quick Start / 快速开始

```bash
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

cmake --preset=release
cmake --build build/release
ctest --preset=release
```

## Example Modules / 示例模块总览

| # | Module | Key Topics | Expected Speedup |
| --- | --- | --- | --- |
| 01 | [Modern CMake](examples/01-cmake-modern/README.md) | Target-based CMake, FetchContent, Presets | — |
| 02 | [Memory & Cache](examples/02-memory-cache/README.md) | AOS vs SOA, False Sharing, Alignment, Prefetch | 2–20x |
| 03 | [Modern C++](examples/03-modern-cpp/README.md) | constexpr, Move Semantics, Reserve, Ranges | 2–1000x |
| 04 | [SIMD Vectorization](examples/04-simd-vectorization/README.md) | Auto-vectorize, SSE/AVX2/AVX-512, SIMD Wrapper | 3–16x |
| 05 | [Concurrency](examples/05-concurrency/README.md) | Atomics, Lock-Free Queue, OpenMP | Linear scaling |

## Who This Is For / 适合谁

- **C++ 工程师** — 需要结构化的性能优化学习路径
- **学生与自学者** — 通过可执行示例对比优化技术
- **项目维护者** — 需要性能分析、基准测试与贡献参考

## Start Reading / 开始阅读

| Goal / 目标 | English | 中文 |
| --- | --- | --- |
| **Structured learning** / 系统学习 | [Learning Path](docs/en/learning-path.md) | [学习路径](docs/zh/learning-path.md) |
| **Find bottlenecks** / 定位瓶颈 | [Profiling Guide](docs/en/profiling-guide.md) | [性能分析指南](docs/zh/profiling-guide.md) |
| **Contribute** / 参与贡献 | [Contributing](CONTRIBUTING.md) | [参与贡献](CONTRIBUTING.zh.md) |

### Recommended Reading Paths / 推荐阅读路径

| Goal | Recommended path |
| --- | --- |
| First visit / 首次访问 | Learning Path → Example Modules → Profiling Guide |
| Hotspot analysis / 热点分析 | Profiling Guide → Memory & Cache → SIMD |
| Add a module / 新增模块 | Contributing → Module README → Learning Path |

## 6-Week Study Plan / 六周学习计划

| Week | Topics |
| --- | --- |
| 1 | Modern CMake + AOS vs SOA + Memory Alignment |
| 2 | False Sharing + Prefetch + constexpr + Move Semantics |
| 3 | Vector Reserve + Ranges + Auto-Vectorization |
| 4 | SIMD Intrinsics + SIMD Wrapper |
| 5 | Atomics + Lock-Free Queue |
| 6 | OpenMP + Profiling & Benchmarking |

## Project Links / 项目链接

- **GitHub**: [LessUp/cpp-high-performance-guide](https://github.com/LessUp/cpp-high-performance-guide)
- **Docs Site**: [lessup.github.io/cpp-high-performance-guide](https://lessup.github.io/cpp-high-performance-guide/)
- **License**: [MIT](https://github.com/LessUp/cpp-high-performance-guide/blob/master/LICENSE)
