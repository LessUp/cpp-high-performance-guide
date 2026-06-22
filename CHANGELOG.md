# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Removed
- Dead `openspec/` archive tree (26 files). The OpenSpec workflow was already retired in 1.3.0; the lingering archive directory contradicted that decision and carried no active specs.
- Stale OpenSpec references from the pull request template, feature request issue template, and README contributor pointers.
- Dangling `AGENTS.md` reference on the bilingual docs homepages; `CLAUDE.md` is the only authoritative AI guidance file.
- Redundant `examples/03-modern-cpp/include/.gitkeep` (the directory now ships five real headers).

## [1.3.0] - 2025-05

### Added
- Runtime SIMD dispatch module with AVX-512/AVX2/SSE2 fallback (`include/hpc/simd.hpp`)
- Header-only safety hardening across all modules
- Benchmark comparison tool with Markdown report generation (`tools/performance/benchmark_compare.py`)

### Changed
- VitePress documentation redesign with Quantum Cobalt theme
- Bilingual homepage landing flow improvements
- Consolidated AI guidance into single `CLAUDE.md` file
- Removed OpenSpec workflow for simplified maintenance

## [1.0.0] - 2024

### Added
- Initial project structure with CMake preset-driven builds
- Memory and cache optimization examples (AOS/SOA, false sharing, alignment, prefetch)
- Modern C++ performance patterns (constexpr, move semantics, ranges)
- SIMD vectorization examples (auto-vectorization, SSE/AVX intrinsics)
- Concurrency examples (atomics, lock-free queue, OpenMP)
- VitePress bilingual documentation site (English + Chinese)
- Google Test and RapidCheck testing frameworks
- Google Benchmark integration
- Sanitizer presets (ASAN, TSAN, UBSAN)
