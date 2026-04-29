# CLAUDE.md

## Mission

Help finish this repository cleanly. Prioritize hardening, consistency, and removal of stale surfaces over feature expansion.

## Project position

This repository is in **closure and hardening mode** (archive-ready for low-frequency maintenance).

## Working style

1. Read the relevant capability specs in `openspec/specs/`.
2. Work inside an OpenSpec change under `openspec/changes/`.
3. Prefer aggressive cleanup when a file or workflow is stale, duplicated, or no longer part of the final repo story.
4. Keep user-facing guidance aligned across:
   - `README.md`
   - `README.zh-CN.md`
   - `docs/`
   - `AGENTS.md`
   - `.github/copilot-instructions.md`
5. Use `/review` before merge or after a large cleanup phase.

## C++ Standards and Style

- **Target**: C++17 minimum, C++20 where beneficial
- **Style**: enforced via `.clang-format` (Google style, 100 chars)
- **Memory safety**: prefer RAII, smart pointers, avoid raw `new/delete`
- **Performance**: always measure with benchmarks before claiming improvement
- **Concurrency**: use `std::atomic` with explicit memory ordering

## Project facts

- Docs stack: **VitePress**, published via GitHub Pages
- Build system: **CMake + presets**
- Language server: **clangd** — configured via `.clangd` at repo root (`CompilationDatabase: build/debug`); all CMake presets export `compile_commands.json` via `CMAKE_EXPORT_COMPILE_COMMANDS=ON`
- Primary maintenance target: **Linux**
- Repo posture: **archive-ready / low-frequency maintenance**

## Commands

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
./build/release/benchmarks/<benchmark_name> --benchmark_time_unit=us

# Utilities
./scripts/format.sh
./scripts/setup-hooks.sh
```

## Process expectations

- Do not add new generic documentation just to fill space.
- Do not reintroduce GitBook / HonKit / `.kiro` assumptions.
- Keep workflows meaningful; delete low-value automation rather than preserving it.
- Prefer one long-running implementation session to quota-heavy parallelism unless the work clearly benefits from it.
