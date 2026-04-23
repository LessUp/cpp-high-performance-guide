# AI-Assisted Workflow

This repository uses **OpenSpec-first development** with a small set of explicit AI instruction files.

## Canonical guidance files

- `openspec/` — requirements, change intent, and task order
- `AGENTS.md` — shared repo guidance for terminal copilots and Codex-style agents
- `CLAUDE.md` — Claude Code guidance
- `.github/copilot-instructions.md` — GitHub Copilot repository guidance

When these surfaces drift, fix the drift instead of inventing a fourth workflow.

## Recommended flow

1. Read the affected capability specs in `openspec/specs/`.
2. Create or update one change under `openspec/changes/<change-id>/`.
3. Implement tasks in dependency order.
4. Run the relevant preset-driven validation commands.
5. Use `/review` before merge or after a major cleanup phase.
6. Archive the change when the work is complete.

## Tool choices

- **LSP**: `clangd`
- **Build/test entry points**: `cmake --preset=...` and `ctest --preset=...`
- **Hooks**: repository-managed `.githooks/`
- **Autopilot strategy**: prefer a longer single session over `/fleet` unless the work is truly parallel

## What to avoid

- reintroducing GitBook / HonKit / `.kiro` assumptions
- adding generic AI docs that duplicate existing instructions
- adding heavy MCP or plugin layers without a repository-specific payoff
