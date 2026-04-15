# Contributing to C++ High Performance Guide

[![Contributors](https://img.shields.io/badge/Contributors-Welcome-brightgreen.svg)](https://github.com/LessUp/cpp-high-performance-guide/blob/master/CONTRIBUTING.md)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Thank you for your interest in contributing! This document provides guidelines for contributing to this project.

> 💡 **New to contributing?** Check out our [Good First Issues](https://github.com/LessUp/cpp-high-performance-guide/labels/good%20first%20issue) to get started!

## Getting Started

1. Fork the repository
2. Clone your fork:
   ```bash
   git clone https://github.com/<your-username>/cpp-high-performance-guide.git
   ```
3. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Build & Test

```bash
# Debug build with tests
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug

# Release build with benchmarks
cmake --preset=release
cmake --build build/release
ctest --preset=release
```

## Code Style

- Follow the `.clang-format` configuration in the project root
- Format your code before committing:
  ```bash
  find examples tests benchmarks -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
  ```
- Use `#pragma once` for header guards
- Place code in the `hpc::` namespace hierarchy

## Adding a New Example Module

1. Create a new directory under `examples/` following the naming convention: `XX-topic-name/`
2. Use the standard structure:
   ```
   examples/XX-topic-name/
   ├── src/           # Source files with demo main()
   ├── bench/         # Google Benchmark files
   ├── include/       # Header-only utilities (optional)
   ├── CMakeLists.txt # Use hpc_add_example() from ExampleTemplate.cmake
   └── README.md      # Module documentation
   ```
3. Register the subdirectory in `examples/CMakeLists.txt`
4. Add corresponding tests under `tests/`

## Testing Requirements

Before submitting a pull request, ensure:

1. **All tests pass:**
   ```bash
   cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
   ```

2. **No AddressSanitizer errors:**
   ```bash
   cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
   ```

3. **No ThreadSanitizer errors** (for concurrency code):
   ```bash
   cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
   ```

4. **No UndefinedBehaviorSanitizer errors:**
   ```bash
   cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
   ```

## Commit Messages

Use clear, descriptive commit messages:
- `feat: add matrix multiplication SIMD example`
- `fix: resolve false sharing in concurrent counter`
- `docs: update learning path with new module`
- `bench: add parameterized benchmark for prefetch distance`
- `test: add property tests for lock-free queue`

## Pull Request Process

1. Ensure your branch is up to date with `main`
2. All CI checks must pass
3. Include a clear description of what your PR adds or fixes
4. Reference any related issues

## Issue and PR Templates

We provide templates to help you submit effective issues and pull requests:

- **Bug Report**: Use when reporting bugs in code, build, or documentation
- **Feature Request**: Use when suggesting new examples or enhancements  
- **Documentation**: Use when reporting documentation issues

Please select the appropriate template when creating a new issue.

## Code of Conduct

This project adheres to a code of conduct that we expect all contributors to follow:

- Be respectful and constructive in all interactions
- Welcome newcomers and help them get started
- Focus on what's best for the community and learners
- Show empathy towards others

## Documentation Sync

When making changes that affect documentation:

1. **Code changes**: Update relevant README files in affected examples
2. **New examples**: Both English and Chinese README files are preferred
3. **API changes**: Document in the relevant module's README

While perfect bilingual sync is ideal, English-only contributions are also welcome — the maintainers will help with Chinese translations.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
