# CLAUDE.md

## Project position

This repository is in **closure and hardening mode** (archive-ready for low-frequency maintenance). Prefer normalization, defect fixing, and removal of stale or low-signal surfaces over adding new features.

## C++ Standards and Style

- **Target**: C++20 (required, `CMAKE_CXX_STANDARD_REQUIRED ON`)
- **Style**: enforced via `.clang-format` (Google style, 100 chars)
- **Memory safety**: prefer RAII, smart pointers, avoid raw `new/delete`
- **Performance**: always measure with benchmarks before claiming improvement
- **Concurrency**: use `std::atomic` with explicit memory ordering
- **Header-only design**: all library code in `include/hpc/` and `examples/*/include/` is header-only. Platform-specific code is hidden behind preprocessor conditionals within headers.

## Repository layout

```text
cpp-high-performance-guide/
├── CMakeLists.txt
├── CMakePresets.json
├── cmake/
├── examples/
├── tests/
├── benchmarks/
├── tools/
├── scripts/
├── docs/                  # VitePress site (Chinese only)
├── .githooks/             # Project-managed Git hooks
└── .github/
```

## Standard commands

```bash
# Build and test
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release

# Sanitizers
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan

# Benchmarks
cmake --preset=release && cmake --build build/release
./build/release/examples/<module-dir>/<benchmark_name> --benchmark_time_unit=us
# 例: ./build/release/examples/02-memory-cache/aos_vs_soa_bench --benchmark_time_unit=us

# Utilities
./scripts/format.sh
./scripts/setup-hooks.sh
```

## Engineering rules

- Prefer **deleting or archiving stale content** instead of preserving redundant surfaces.
- Keep **README** focused on repository entry and onboarding; use the docs site for the richer narrative.
- Treat **GitHub Pages** as a project landing page, not a thin README mirror.
- Documentation is **Chinese only**. Do not reintroduce English documentation surfaces.
- Use **CMake presets** as the default build and test entry points.
- Keep workflows **preset-driven, low-noise, and meaningful**.
- Avoid over-engineering with extra plugins, MCP servers, or automation layers unless they clearly reduce maintenance cost.

## AI tooling policy

- **CLAUDE.md**: Claude Code specific guidance (this file).
- **LSP**: prefer `clangd` backed by `compile_commands.json`. The `.clangd` config at the repo root points `CompilationDatabase` to `build/debug`.
- **Review model**: use `/review` before merge or after major cleanup phases.

## Editor integration

The `.vscode/` directory is **gitignored**. Use `.clangd`, `.editorconfig`, and `CMakePresets.json` for machine-readable project conventions.

## Things to avoid reintroducing

- non-blocking CI steps that hide real failures
- per-example shim headers that only re-export a canonical `include/hpc/` header
- wrapper scripts that duplicate `cmake --preset` commands
- English documentation surfaces or bilingual parity machinery
