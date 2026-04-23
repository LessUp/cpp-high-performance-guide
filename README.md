# C++ High Performance Guide

<p align="center">
  <a href="https://github.com/LessUp/cpp-high-performance-guide/stargazers">
    <img src="https://img.shields.io/github/stars/LessUp/cpp-high-performance-guide?style=flat-square&color=yellow" alt="Stars">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/LessUp/cpp-high-performance-guide/ci.yml?branch=master&label=CI&style=flat-square" alt="CI">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square" alt="License">
  </a>
</p>

<p align="center">
  <b>English</b> | <a href="README.zh-CN.md">简体中文</a> | <a href="https://lessup.github.io/cpp-high-performance-guide/">GitHub Pages</a> | <a href="https://github.com/LessUp/cpp-high-performance-guide/discussions">Discussions</a>
</p>

<p align="center">
  Learn measurable C++20 performance engineering through runnable examples, benchmarks, and a VitePress-powered learning site.
</p>

## Why this repository exists

This project is a practical guide to common C++ performance topics that are easy to talk about but harder to validate:

- modern CMake and preset-driven builds
- memory and cache layout decisions
- modern C++ performance patterns
- SIMD and vectorization
- concurrency and lock-free basics
- profiling and benchmark-driven reasoning

Every major topic is meant to be **readable, buildable, and measurable**.

## What you can explore

| Area | What it covers |
| --- | --- |
| `examples/01-cmake-modern/` | modern CMake structure and anti-patterns |
| `examples/02-memory-cache/` | AOS vs SOA, false sharing, alignment, prefetch |
| `examples/03-modern-cpp/` | constexpr, move semantics, reserve, ranges |
| `examples/04-simd-vectorization/` | auto-vectorization, intrinsics, SIMD wrappers |
| `examples/05-concurrency/` | atomics, lock-free queue, OpenMP |
| `docs/` | bilingual learning path, profiling guide, troubleshooting, reference |
| `openspec/` | spec-driven development workflow and change history |

## Quick start

```bash
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

cmake --preset=release
cmake --build build/release
```

Run one benchmark:

```bash
./build/release/examples/02-memory-cache/aos_soa_bench
```

## Validation commands

```bash
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release

cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```

## Documentation and learning path

- **Docs site:** <https://lessup.github.io/cpp-high-performance-guide/>
- **Quick start:** `docs/en/getting-started/quickstart.md`
- **Learning path:** `docs/en/guides/learning-path.md`
- **Profiling guide:** `docs/en/guides/profiling-guide.md`
- **Chinese entry:** `README.zh-CN.md` and `docs/zh/`

## Development workflow

This repository uses **OpenSpec** for non-trivial changes.

1. Read the affected capability specs in `openspec/specs/`.
2. Create or update a change under `openspec/changes/<change-id>/`.
3. Implement tasks in dependency order.
4. Run preset-driven validation.
5. Use `/review` before merge for non-trivial work.
6. Archive completed changes.

The repository also keeps AI tool guidance in:

- `AGENTS.md`
- `CLAUDE.md`
- `.github/copilot-instructions.md`

## Tech stack

- **Language:** C++20
- **Build:** CMake 3.22+, Ninja
- **Testing:** Google Test, RapidCheck
- **Benchmarking:** Google Benchmark
- **Docs:** VitePress + GitHub Pages
- **Profiling:** perf, FlameGraph, Valgrind, VTune

## Contributing

See `CONTRIBUTING.md` for the contributor workflow, hooks setup, and OpenSpec process.
