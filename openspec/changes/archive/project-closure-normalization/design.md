# Design: Project Closure Normalization

## Overview

This change establishes a single coherent repository contract across source layout, OpenSpec, documentation, CI, hooks, and AI workflow guidance.

## Design goals

- Reduce drift between live implementation and contributor-facing guidance.
- Prefer fewer, stronger surfaces over many weak or duplicated ones.
- Keep the final workflow easy to operate in a long-running autopilot session without relying on `/fleet`.
- Preserve enough structure for low-frequency maintenance after the repository enters a quieter phase.

## Target repository contract

### 1. OpenSpec

- `openspec/specs/<capability>/spec.md` is the only capability-spec layout.
- Active changes use `proposal.md`, `design.md`, `tasks.md`, and `specs/` deltas.
- Legacy or completed work moves to `openspec/changes/archive/`.

### 2. Documentation

- Root README files stay focused on repository onboarding.
- VitePress handles the richer project landing narrative.
- Legacy docs-stack references are removed.
- User-facing bilingual surfaces remain aligned.

### 3. Engineering workflow

- GitHub Actions use CMake presets as the source of build/test truth.
- Critical validation failures are blocking.
- Hooks provide lightweight local enforcement without adding dependency-heavy tooling.

### 4. AI workflow

- `AGENTS.md`, `CLAUDE.md`, and `.github/copilot-instructions.md` define the repo-specific AI contract.
- `clangd` is the default LSP.
- MCP and plugin expansion stay opt-in and restrained.
- `/review` is the default phase gate for non-trivial work.

### 5. Public GitHub surface

- GitHub Pages acts as the primary public landing surface.
- Repository description, topics, and homepage URL align with that positioning.

## Trade-offs

- We choose deletion / archival over preserving historical clutter in active paths.
- We keep hooks simple and shell-based instead of adopting heavier hook managers.
- We keep LSP guidance centered on `clangd` instead of creating separate per-tool language-server strategies.
