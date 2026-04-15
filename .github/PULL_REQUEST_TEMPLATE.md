# Pull Request

## Description

Brief description of what this PR adds or fixes.

## Type of Change

- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature/example (non-breaking change which adds functionality)
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactoring
- [ ] Build/CI improvement

## Checklist

- [ ] My code follows the project's coding style (`.clang-format`)
- [ ] I have tested my changes locally:
  - [ ] Debug build: `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug`
  - [ ] Release build: `cmake --preset=release && cmake --build build/release && ctest --preset=release`
- [ ] For code changes, I have run sanitizers:
  - [ ] AddressSanitizer: `cmake --preset=asan && cmake --build build/asan && ctest --preset=asan`
  - [ ] ThreadSanitizer (for concurrency code): `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan`
  - [ ] UndefinedBehaviorSanitizer: `cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan`
- [ ] I have updated the documentation accordingly (if applicable)
- [ ] Both English and Chinese documentation are updated (if applicable)
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works (if applicable)

## Testing Notes

Describe how you tested your changes:

- Build presets tested: 
- Benchmark results (if applicable):
- Platform/compiler:

## Related Issues

Fixes #(issue number)
Related to #(issue number)

## Screenshots (if applicable)

Add screenshots to help explain your changes.

---

**Thank you for contributing to C++ High Performance Guide! 🚀**
