# CI and Quality Assurance

## Overview

Automated validation, workflow design, and repository quality gates for a low-noise maintenance workflow.

---

### Requirement: Preset-Driven CI

THE Build_System SHALL implement GitHub Actions around the repository’s CMake presets instead of duplicating build logic ad hoc.

#### Scenario: Standard CI run

- **WHEN** CI builds and tests the project
- **THEN** it uses the same preset-driven paths documented for local development

---

### Requirement: Meaningful Workflow Surface

THE Build_System SHALL keep only workflows that provide concrete maintenance value.

#### Scenario: Workflow reviewed

- **WHEN** maintainers review GitHub Actions coverage
- **THEN** redundant, noisy, or non-actionable workflows are simplified, gated, or removed

---

### Requirement: Explicit Failure Signaling

THE Build_System SHALL fail critical validation steps instead of hiding errors behind warning-only behavior.

#### Scenario: Validation fails

- **WHEN** formatting, build, test, or sanitizer validation fails
- **THEN** the workflow exits with a failing status rather than silently continuing

---

### Requirement: Sanitizer Integration

THE Build_System SHALL integrate AddressSanitizer, ThreadSanitizer, and UndefinedBehaviorSanitizer through dedicated validation paths.

#### Scenario: Sanitizer run

- **WHEN** sanitizer workflows execute
- **THEN** they use the matching preset or equivalent repository-standard configuration

---

### Requirement: Pages Build Validation

THE Build_System SHALL validate the docs site through the live docs stack before deployment.

#### Scenario: Pages deployment

- **WHEN** GitHub Pages is built
- **THEN** VitePress output is generated successfully and deployment stops on build failure
