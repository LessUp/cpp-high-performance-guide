# C++ High Performance Computing Optimization Guide

English | [简体中文](README.zh-CN.md) | [Documentation Home](https://lessup.github.io/cpp-high-performance-guide/)

A modern C++20 example collection for learning performance engineering across build systems, memory and cache behavior, SIMD, concurrency, benchmarking, and profiling.

## Repository Overview

- `examples/`: five themed modules covering modern CMake, memory and cache optimization, modern C++ features, SIMD, and concurrency.
- `benchmarks/` and `tools/`: benchmark runners, FlameGraph helpers, and analysis scripts.
- `docs/`: bilingual learning path, profiling guide, and HonKit or GitBook synchronization notes.
- `tests/`: unit, integration, and property-style checks for the example collection.

## Quick Start

```bash
cmake --preset=release
cmake --build build/release
ctest --preset=release
```

## Documentation

- Docs site: https://lessup.github.io/cpp-high-performance-guide/
- Learning path: `docs/en/learning-path.md` and `docs/zh/learning-path.md`
- Profiling guide: `docs/en/profiling-guide.md` and `docs/zh/profiling-guide.md`
- HonKit or GitBook sync notes: `docs/en/gitbook-sync.md` and `docs/zh/gitbook-sync.md`

## Development

- Contribution guide: `CONTRIBUTING.md` and `CONTRIBUTING.zh.md`
- Changelog: `changelog/`
- License: `LICENSE`
