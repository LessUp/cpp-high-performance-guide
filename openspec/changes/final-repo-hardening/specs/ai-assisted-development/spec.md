# AI-Assisted Development

## ADDED Requirements

### Requirement: AI Instruction File Consistency

THE Repository SHALL keep `AGENTS.md`, `CLAUDE.md`, and `.github/copilot-instructions.md` mutually consistent with no contradictions between them.

#### Scenario: AI tool loads multiple instruction files

- **WHEN** an AI assistant reads both `AGENTS.md` and `.github/copilot-instructions.md`
- **THEN** it does not receive contradictory instructions about the repository's workflow, tools, or conventions

*Application:* Verified in Phase 3 audit. Any contradiction found is resolved by aligning the lower-priority file to the canonical source (`AGENTS.md`).

---

### Requirement: Compile Commands Export Verified

THE Repository SHALL verify that `CMAKE_EXPORT_COMPILE_COMMANDS` is enabled so `clangd` can function correctly without additional configuration.

#### Scenario: Contributor configures clangd

- **WHEN** a contributor runs any CMake preset and opens the project in an editor backed by `clangd`
- **THEN** `compile_commands.json` is present in the build output directory and `clangd` can index the project without manual setup

---

### Requirement: No Editor Config Committed

THE Repository SHALL NOT commit editor-specific configuration directories (e.g., `.vscode/`) to source control.

#### Scenario: Editor config discovered in repository

- **WHEN** a `.vscode/` or equivalent editor-specific directory is found committed to the repository
- **THEN** it is removed and added to `.gitignore`, or its presence is explicitly justified with a documented rationale

*Rationale:* The project documents `clangd` as the LSP choice but does not prescribe a specific editor. Committed editor config creates maintenance surface without broad benefit.
