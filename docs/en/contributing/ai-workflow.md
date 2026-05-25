# AI-Assisted Workflow

This repository uses a simplified AI workflow with a single guidance file.

## Canonical guidance file

- `CLAUDE.md` — Claude Code guidance (the authoritative AI instruction file)

When documentation drifts from code, fix the drift instead of working around it.

## Recommended flow

1. Keep documentation and code in sync.
2. Run the relevant preset-driven validation commands.
3. Use `/review` before merge or after a major cleanup phase.

## Tool choices

- **LSP**: `clangd`
- **Build/test entry points**: `cmake --preset=...` and `ctest --preset=...`
- **Hooks**: repository-managed `.githooks/`
- **Autopilot strategy**: prefer a longer single session over `/fleet` unless the work is truly parallel

## What to avoid

- reintroducing GitBook / HonKit / `.kiro` assumptions
- adding generic AI docs that duplicate existing instructions
- adding heavy MCP or plugin layers without a repository-specific payoff