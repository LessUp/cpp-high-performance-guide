# Copilot instructions for this repository

This repository is in **closure and hardening mode**.

## Priorities

1. Fix drift between code, docs, OpenSpec, and GitHub workflows.
2. Prefer removing stale or redundant content over preserving it.
3. Keep the repository easy to maintain for low-frequency future updates.

## Required workflow

- Read the relevant specs in `openspec/specs/` before making structural changes.
- For non-trivial work, update or create an OpenSpec change under `openspec/changes/`.
- Keep `README.md`, `README.zh-CN.md`, `docs/`, `AGENTS.md`, and `CLAUDE.md` aligned.
- Use `/review` before merge or after a major cleanup phase.

## Repository-specific rules

- Docs stack is **VitePress**. Do not reintroduce HonKit or GitBook assumptions.
- The default validation path is **preset-driven**:
  - `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug`
  - sanitizer presets for deeper validation
- Prefer **clangd** as the LSP because the project exports `compile_commands.json`.
- Avoid broad MCP or plugin sprawl unless there is a clear repository-specific payoff.
- Keep user-facing documentation bilingual where the surface is intended for readers.

## What “good” looks like here

- One coherent repo story
- High-signal docs
- Minimal but meaningful CI
- Explicit AI workflow guidance
- OpenSpec-driven changes that are easy to archive
