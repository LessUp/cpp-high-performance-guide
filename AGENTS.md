# AGENTS.md

## Project position

This repository is in **closure and hardening mode**. Prefer normalization, defect fixing, and removal of stale or low-signal surfaces over adding new features.

## Canonical sources of truth

1. **Current code and build configuration**
2. **`openspec/`** for requirements, change intent, and implementation order
3. **`AGENTS.md` / `CLAUDE.md` / `.github/copilot-instructions.md`** for AI workflow guidance

If these sources disagree, fix the drift instead of working around it.

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
├── docs/                  # VitePress site and bilingual learning docs
├── openspec/              # Capability specs and change proposals
├── .githooks/             # Project-managed Git hooks
└── .github/
```

## OpenSpec workflow

1. **Inspect capability specs first** in `openspec/specs/`.
2. **Create or update one change** under `openspec/changes/<change-id>/` with:
   - `proposal.md`
   - `design.md`
   - `tasks.md`
   - `specs/<capability>/spec.md` deltas for changed capabilities
3. **Implement in dependency order** from `tasks.md`.
4. **Validate with existing project commands** before considering the work complete.
5. **Run `/review` at phase gates** or before merging non-trivial changes.
6. **Archive completed changes** into `openspec/changes/archive/`.

## Engineering rules

- Prefer **deleting or archiving stale content** instead of preserving redundant surfaces in place.
- Keep **README** focused on repository entry and onboarding; use the docs site for the richer narrative.
- Treat **GitHub Pages** as a project landing page, not a thin README mirror.
- Keep user-facing documentation **English + Chinese** where the surface is meant to be bilingual.
- Use **CMake presets** as the default build and test entry points.
- Keep workflows **preset-driven, low-noise, and meaningful**. Do not keep jobs that only produce churn.
- Avoid over-engineering with extra plugins, MCP servers, or automation layers unless they clearly reduce maintenance cost.

## Standard commands

```bash
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug

cmake --preset=release
cmake --build build/release
ctest --preset=release

cmake --preset=asan
cmake --build build/asan
ctest --preset=asan

cmake --preset=tsan
cmake --build build/tsan
ctest --preset=tsan

cmake --preset=ubsan
cmake --build build/ubsan
ctest --preset=ubsan

./scripts/format.sh
./scripts/setup-hooks.sh
```

## AI tooling policy

- **AGENTS.md**: shared repo guidance for Codex-style agents and terminal copilots.
- **CLAUDE.md**: Claude Code specific guidance.
- **`.github/copilot-instructions.md`**: GitHub Copilot repository instructions.
- **LSP**: prefer `clangd` backed by `compile_commands.json`.
- **Review model**: use `/review` before merge or after major cleanup phases.
- **Autopilot strategy**: prefer a longer single-session implementation flow over `/fleet` unless the task is truly parallelizable.

## Things to avoid reintroducing

- HonKit / GitBook-era configuration
- `.kiro/`-style legacy spec structures
- duplicate changelog surfaces with no maintenance value
- non-blocking CI steps that hide real failures
- generic AI instructions that do not reflect this repository
