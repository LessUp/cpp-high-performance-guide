# AI-Assisted Development

## Overview

Repository-specific guidance for AI-assisted development tools, review flow, and tooling choices.

---

### Requirement: Tool-Specific Instruction Files

THE Repository SHALL provide explicit, project-specific instruction files for the primary AI tools used on the project.

#### Scenario: AI tool loads repository guidance

- **WHEN** an AI assistant is invoked in the repository
- **THEN** it can discover high-signal project guidance through `AGENTS.md`, `CLAUDE.md`, or `.github/copilot-instructions.md`

---

### Requirement: OpenSpec-First Change Flow

THE Repository SHALL require non-trivial changes to be described through OpenSpec before implementation proceeds.

#### Scenario: Structural cleanup requested

- **WHEN** a contributor performs a repository-wide cleanup or architectural adjustment
- **THEN** the work is tracked through an OpenSpec change with proposal, design, tasks, and affected capability deltas

---

### Requirement: Review Gate

THE Repository SHALL encourage an explicit AI or human review step before merge for non-trivial work.

#### Scenario: Cleanup phase completed

- **WHEN** a substantial implementation phase finishes
- **THEN** `/review` or an equivalent review pass is run before merge

---

### Requirement: Default LSP Choice

THE Repository SHALL standardize on `clangd` as the default language server.

#### Scenario: Contributor sets up local tooling

- **WHEN** a contributor configures editor support for the C++ codebase
- **THEN** the documented default path uses `clangd` and `compile_commands.json`

---

### Requirement: Restrained Tooling Expansion

THE Repository SHALL avoid adding MCP servers, plugins, or AI-specific tooling layers that do not provide repository-specific value.

#### Scenario: New AI integration proposed

- **WHEN** a new AI integration is considered
- **THEN** it is adopted only if it clearly reduces maintenance or improves this repository’s workflow without unnecessary context overhead
