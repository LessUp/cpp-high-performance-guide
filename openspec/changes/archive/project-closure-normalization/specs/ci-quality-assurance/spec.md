# CI and Quality Assurance

## ADDED Requirements

### Requirement: Preset-Driven CI

THE Build_System SHALL implement CI around the repository’s CMake presets.

#### Scenario: CI validates a pull request

- **WHEN** GitHub Actions runs for a change
- **THEN** it uses the documented preset-driven build and test flow

### Requirement: Explicit Failure Signaling

THE Build_System SHALL fail critical checks instead of allowing warning-only success for broken validation.

#### Scenario: Validation breaks

- **WHEN** formatting, build, test, or sanitizer validation fails
- **THEN** CI reports a failing status that blocks merge
