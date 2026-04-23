# Internationalization Support

## Overview

Multi-language documentation support for Chinese and English speakers.

---

### Requirement: Bilingual Main README

THE Documentation SHALL provide Chinese (zh) and English (en) versions of the main README.

#### Scenario: README in both languages

- **WHEN** a user opens the project
- **THEN** both README.md (English) and README.zh-CN.md (Chinese) are available

---

### Requirement: Bilingual Learning Path

THE Documentation SHALL provide Chinese and English versions of the learning path document.

#### Scenario: Learning path bilingual

- **WHEN** a user accesses the learning path
- **THEN** `docs/zh/guides/learning-path.md` and `docs/en/guides/learning-path.md` are both available

---

### Requirement: Bilingual Profiling Guide

THE Documentation SHALL provide Chinese and English versions of the profiling guide.

#### Scenario: Profiling guide bilingual

- **WHEN** a user accesses the profiling guide
- **THEN** `docs/zh/guides/profiling-guide.md` and `docs/en/guides/profiling-guide.md` are both available

---

### Requirement: Documentation Synchronization

WHEN documentation is updated, THE Documentation SHALL keep both language versions in sync.

#### Scenario: Both languages updated together

- **WHEN** documentation content changes
- **THEN** both Chinese and English versions are updated

---

### Requirement: Bilingual Entry Surfaces

THE Documentation SHALL keep the main repository entry surfaces available in both English and Chinese.

#### Scenario: User chooses language at entry

- **WHEN** a user lands on the repository or docs site
- **THEN** they can reach both English and Chinese entry pages without broken or stale paths

---

### Requirement: Bilingual Directory Structure

THE HPC_Guide SHALL organize multi-language documents under `docs/zh/` and `docs/en/` directories.

#### Scenario: Language directories exist

- **WHEN** a user explores the docs directory
- **THEN** zh/ and en/ subdirectories contain respective language documents
