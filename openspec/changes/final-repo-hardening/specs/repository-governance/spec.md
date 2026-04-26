# Repository Governance

## ADDED Requirements

### Requirement: Empty Directory Removal

THE Repository SHALL NOT contain empty directories that are artifacts of superseded tooling or abandoned planning workflows.

#### Scenario: Stale scaffolding directory found

- **WHEN** a directory in the repository root or active paths has no content and no documented future purpose
- **THEN** it is deleted in the current hardening change rather than left as a silent maintenance burden

*Rationale:* `_bmad/`, `_bmad-output/`, and `changelog/` are empty artifacts. Their presence without content implies intent where none exists.

---

### Requirement: Changelog Surface Clarity

THE Repository SHALL NOT maintain an empty `changelog/` directory as the only changelog surface.

#### Scenario: Contributor looks for change history

- **WHEN** a contributor wants to understand what changed between releases
- **THEN** they are directed to Git tags and GitHub Releases, not to an empty directory

---

### Requirement: OpenSpec Active Path Cleanliness

THE OpenSpec active changes path (`openspec/changes/`) SHALL contain only changes that are in progress.

#### Scenario: Contributor inspects active changes

- **WHEN** a contributor opens `openspec/changes/`
- **THEN** they see only changes that are not yet archived; completed changes are moved to `openspec/changes/archive/`

*Application:* `project-closure-normalization` must be archived when `final-repo-hardening` is complete. `final-repo-hardening` itself must be archived as its last act.
