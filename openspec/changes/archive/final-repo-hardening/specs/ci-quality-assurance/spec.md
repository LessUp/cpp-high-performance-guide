# CI and Quality Assurance

## ADDED Requirements

### Requirement: Workflow Verified to Pass

THE Build_System SHALL have all active GitHub Actions workflows verified to pass cleanly before the repository enters a maintenance posture.

#### Scenario: Workflow verification during hardening

- **WHEN** the final hardening phase runs
- **THEN** each of `ci.yml`, `pages.yml`, `sanitizers.yml`, and `benchmark.yml` either passes cleanly or is explicitly tightened or removed with a documented rationale

*Application:* `sanitizers.yml` and `benchmark.yml` are both verified in Phase 2. A workflow that silently exits with success while producing no real output is not considered passing — it is either fixed or removed.

---

### Requirement: No Silent No-Op Workflows

THE Build_System SHALL NOT keep workflows that execute without asserting any meaningful validation outcome.

#### Scenario: Benchmark workflow runs but asserts nothing

- **WHEN** a workflow step completes without checking that it actually ran its intended validation
- **THEN** the step is modified to assert the expected output or the workflow is removed

---

### Requirement: Compiler Matrix Currency

THE Build_System SHALL keep the CI compiler matrix current with non-end-of-life compiler versions and GitHub Actions runner images.

#### Scenario: CI matrix review

- **WHEN** a maintainer reviews `ci.yml`
- **THEN** the GCC version, Clang version, and runner images are not using deprecated or end-of-life configurations that will begin failing without warning
