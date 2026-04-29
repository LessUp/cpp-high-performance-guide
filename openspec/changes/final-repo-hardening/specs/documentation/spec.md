# Documentation

## ADDED Requirements

### Requirement: Bilingual Scope Policy

THE Documentation SHALL define and maintain an explicit bilingual scope policy rather than leaving the zh/en gap undocumented.

#### Scenario: Contributor notices a missing Chinese translation

- **WHEN** a contributor finds a docs page that exists in `docs/en/` but not in `docs/zh/`
- **THEN** they can determine from the scope policy whether that page is expected to be translated or is English-only by design

*Application:* `docs/en/exercises/` and `docs/en/reference/api/` subdirectory pages are English-only by design. All other sections in `getting-started/`, `guides/`, `reference/` (top-level), and `contributing/` are fully bilingual.

---

### Requirement: Docs Translation Scope Document

THE Documentation SHALL include a translation scope reference so contributors know what is and is not expected to be bilingual.

#### Scenario: New contributor prepares a Chinese translation

- **WHEN** a contributor wants to add or update a Chinese translation
- **THEN** a scope document (in `docs/TRANSLATION_SCOPE.md` or equivalent) tells them exactly which pages are in scope

---

### Requirement: Contributing Docs Non-Duplication

THE Documentation SHALL ensure that `docs/*/contributing/ai-workflow.md` summarizes and links to canonical AI instruction files rather than duplicating their content verbatim.

#### Scenario: AI workflow docs drift from canonical guidance

- **WHEN** `AGENTS.md` or `CLAUDE.md` is updated
- **THEN** `docs/contributing/ai-workflow.md` does not require a parallel rewrite because it links rather than duplicates
