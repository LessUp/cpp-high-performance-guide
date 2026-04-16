# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.3.0] - 2026-04-16

### Major Changes
- Comprehensive documentation restructure with hierarchical organization
- Complete bilingual (EN/ZH) documentation parity
- Professional changelog format with standardized release notes
- New getting-started guides (quickstart, installation, prerequisites)

### Added
- New `docs/` structure: `getting-started/`, `guides/`, `reference/`, `contributing/`
- Landing pages for English and Chinese documentation
- Quick start guides for both languages
- Platform-specific installation guides (Ubuntu, Fedora, macOS, Windows/WSL)
- Prerequisites documentation with knowledge and system requirements
- Best practices guides with code examples
- API reference documentation

### Changed
- Updated SUMMARY.md with new documentation paths
- Professionalized changelog format with metadata and categories
- Improved navigation between bilingual content

### Documentation
- Complete restructuring for better discoverability
- Cross-linked bilingual content
- Enhanced getting started experience

## [1.2.0] - 2026-04-15

### Added
- Expanded README with badges, performance highlights, and 6-week learning path
- FAQ and troubleshooting guides (bilingual)
- GitHub issue templates (bug report, feature request, docs improvement)
- Pull request template
- SECURITY.md and CODE_OF_CONDUCT.md
- Helper scripts in scripts/ directory

### Changed
- Restructured documentation with standardized README format
- Consolidated DOCS.md content into README.md (single entry point)
- Unified example module READMEs with consistent structure
- Moved CLAUDE.md to docs/developers/claude-guide.md
- Reorganized tools/ to tools/performance/

### Fixed
- All internal documentation links
- Workflow branch references to use correct default branch (master)

## [1.1.0] - 2026-03-13

### Added
- DOCS.md as dedicated documentation entry point (later consolidated)
- Bilingual GitBook sync guides

### Changed
- Restructured SUMMARY.md with organized sections
- Separated repository entry (README.md) from documentation entry (DOCS.md)
- Updated pages.yml to support both main and master branches

### Documentation
- Standardized documentation information architecture
- Clarified entry point responsibilities

## [1.0.2] - 2026-03-10

### Added
- package.json and package-lock.json for HonKit version locking
- npm cache support in pages workflow for faster builds
- Sparse-checkout for efficient CI builds

### Changed
- Renamed workflows: docs.yml → pages.yml, build.yml → ci.yml
- Standardized workflow permissions and concurrency settings

### Fixed
- Docs badge link pointing to correct workflow file
- Reduced CI checkout time by excluding unnecessary directories

## [1.0.1] - 2026-03-06

### Added
- CONTRIBUTING.zh.md (Chinese contributing guide)
- .gitbook.yaml for GitBook integration
- SUMMARY.md for GitBook table of contents
- Unit tests for modern_cpp module
- HPC_TEST_MODE macro for test-friendly code reuse

### Changed
- Enhanced README with better badges and bilingual navigation
- Improved CMake modularity with per-module build switches
- Updated property tests file structure

### Fixed
- Cross-platform temporary file path issues (Windows compatibility)
- SIMD dispatch template design (separate template parameters)
- Property test entry point ordering

## [1.0.0] - 2026-02-27

### Added
- Chinese README (README.zh-CN.md)
- .clang-format configuration (Google style, C++20)
- CONTRIBUTING.md guide
- MSVC CMake presets for Visual Studio 2022
- Unit tests for memory, SIMD, and concurrency modules
- Spec documents in .kiro/specs/

### Changed
- Unified compiler version requirements (GCC 11+, Clang 14+)
- Updated design documents with architecture diagrams
- Expanded requirements with MoSCoW priorities

### Fixed
- README badge URLs (replaced YOUR_USERNAME with LessUp)
- MSVC compatibility in simd_utils.hpp
- CMakeLists.txt registration for atomic and OpenMP benchmarks

## [0.1.0] - 2026-01-09

### Added
- Initial project structure
- 5 example modules (CMake, Memory, Modern C++, SIMD, Concurrency)
- Google Benchmark integration
- Google Test integration
- RapidCheck property-based testing
- CMake presets (debug, release, asan, tsan, ubsan, coverage)
- CI/CD workflows

### Fixed
- SIMD property test compilation issues
- RapidCheck generator usage
- Empty benchmark JSON output handling
- Performance test stability in CI environments

---

[Unreleased]: https://github.com/LessUp/cpp-high-performance-guide/compare/v1.3.0...HEAD
[1.3.0]: https://github.com/LessUp/cpp-high-performance-guide/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/LessUp/cpp-high-performance-guide/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/LessUp/cpp-high-performance-guide/compare/v1.0.2...v1.1.0
[1.0.2]: https://github.com/LessUp/cpp-high-performance-guide/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/LessUp/cpp-high-performance-guide/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/LessUp/cpp-high-performance-guide/compare/v0.1.0...v1.0.0
[0.1.0]: https://github.com/LessUp/cpp-high-performance-guide/releases/tag/v0.1.0
