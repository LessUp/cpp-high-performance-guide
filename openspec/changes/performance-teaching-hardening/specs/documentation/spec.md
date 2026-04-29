# Documentation

## ADDED Requirements

### Requirement: Sanitizer Workflow Visibility

THE Documentation SHALL surface the sanitizer preset workflow so readers can find and run ASan, TSan, and UBSan without reading CMakeLists.txt or CMakePresets.json.

#### Scenario: Reader finds sanitizer instructions

- **WHEN** a reader opens the docs site validation section
- **THEN** they find the `asan`, `tsan`, and `ubsan` preset names with copy-pasteable build-and-run commands

#### Scenario: README cross-link present

- **WHEN** a reader opens the root README quick-start
- **THEN** there is a visible link to the sanitizer workflow documentation

---

### Requirement: Vectorization Diagnostics Reachable from Docs

THE Documentation SHALL link readers from the docs site SIMD entry to the vectorization diagnostics workflow.

#### Scenario: Docs site SIMD entry links diagnostics

- **WHEN** a reader navigates to the SIMD module entry on the docs site
- **THEN** they can reach instructions for enabling compiler vectorization reports
