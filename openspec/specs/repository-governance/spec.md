# Repository Governance

## Overview

Repository structure, public surfaces, and maintenance rules for keeping the project coherent and archive-ready.

---

### Requirement: Canonical Repository Surfaces

THE Repository SHALL keep a small set of canonical documentation and process surfaces.

#### Scenario: Contributor looks for project guidance

- **WHEN** a contributor opens the repository
- **THEN** the authoritative guidance is discoverable through `README.md`, `README.zh-CN.md`, `openspec/`, `AGENTS.md`, `CLAUDE.md`, and `.github/copilot-instructions.md`

---

### Requirement: Stale Surface Removal

THE Repository SHALL remove or archive stale, duplicated, or low-signal project surfaces instead of preserving them in active paths.

#### Scenario: Legacy docs stack is superseded

- **WHEN** a documentation or workflow surface no longer matches the live repository architecture
- **THEN** it is deleted or archived rather than remaining in contributor-facing paths

---

### Requirement: Focused Changelog Strategy

THE Repository SHALL maintain only changelog surfaces that still provide concrete maintenance value.

#### Scenario: Historical change log review

- **WHEN** maintainers review version-history content
- **THEN** redundant or low-value changelog surfaces are consolidated, archived, or removed

---

### Requirement: Purposeful GitHub Pages Positioning

THE Repository SHALL treat GitHub Pages as a project landing surface, not a README mirror.

#### Scenario: New visitor opens the docs site

- **WHEN** a visitor lands on GitHub Pages
- **THEN** the site explains the project’s purpose, learning path, validation story, and entry points better than the root README alone
