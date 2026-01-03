# Modern CMake Examples

This module demonstrates CMake best practices through anti-pattern vs best-practice comparisons.

## Contents

- `anti-patterns/` - Common CMake mistakes to avoid
- `best-practices/` - Modern CMake approaches

## Key Concepts

### Target-Based Commands

**Anti-pattern:**
```cmake
include_directories(${PROJECT_SOURCE_DIR}/include)
link_libraries(some_lib)
```

**Best practice:**
```cmake
target_include_directories(my_target PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(my_target PRIVATE some_lib)
```

### Why Target-Based is Better

1. **Encapsulation** - Dependencies don't leak to other targets
2. **Transitivity** - PUBLIC/PRIVATE/INTERFACE control propagation
3. **Scalability** - Works correctly in large projects
4. **IDE Support** - Better integration with modern IDEs

### Dependency Management

Use FetchContent for automatic dependency download:

```cmake
include(FetchContent)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(benchmark)
```

### CMake Presets

Use `CMakePresets.json` for reproducible builds:

```bash
cmake --preset=release
cmake --build build/release
```

## Building

```bash
# From project root
cmake --preset=release
cmake --build build/release --target cmake_anti_patterns cmake_best_practices
```

## Further Reading

- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [CMake Documentation](https://cmake.org/documentation/)
