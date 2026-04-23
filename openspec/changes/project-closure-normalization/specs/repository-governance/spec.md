# Repository Governance

## ADDED Requirements

### Requirement: Canonical Repository Surfaces

THE Repository SHALL define a small, explicit set of contributor-facing guidance surfaces.

#### Scenario: Contributor needs authoritative guidance

- **WHEN** a contributor looks for project process guidance
- **THEN** they can rely on `README.md`, `README.zh-CN.md`, `openspec/`, `AGENTS.md`, `CLAUDE.md`, and `.github/copilot-instructions.md`

### Requirement: Stale Surface Removal

THE Repository SHALL delete or archive stale and duplicated surfaces rather than leaving them in active paths.

#### Scenario: Legacy process file found

- **WHEN** a legacy file no longer matches the live repository architecture
- **THEN** it is removed or archived before the cleanup change is considered complete
