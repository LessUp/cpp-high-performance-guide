# 01 - Modern CMake Examples

<p align="center">
  <img src="https://img.shields.io/badge/CMake-3.22%2B-green.svg" alt="CMake">
  <img src="https://img.shields.io/badge/Difficulty-Beginner-green.svg" alt="Difficulty">
  <img src="https://img.shields.io/badge/Topic-Build%20System-blue.svg" alt="Topic">
</p>

> Learn modern CMake best practices through anti-pattern vs best-practice comparisons.

This module demonstrates why target-based CMake is superior to directory-based approaches and how to use modern CMake features effectively.

---

## Contents

- `anti-patterns/` - Common CMake mistakes to avoid
- `best-practices/` - Modern CMake approaches

---

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

---

## Building

```bash
# From project root
cmake --preset=release
cmake --build build/release --target cmake_anti_patterns cmake_best_practices
```

---

## Expected Results

| Practice | Benefit | Impact |
|----------|---------|--------|
| Target-based commands | Better encapsulation | Prevents dependency leaks |
| FetchContent | Automatic dependency management | No manual git submodules |
| CMake Presets | Reproducible builds | Consistent across environments |
| Modern C++20 config | Clean standard setting | No manual flag management |

---

## Common Pitfalls

### ❌ Using directory-level commands

```cmake
# BAD: Affects all targets after this point
include_directories(include/)
link_libraries(common_lib)

# GOOD: Only affects specific target
target_include_directories(my_target PRIVATE include/)
target_link_libraries(my_target PRIVATE common_lib)
```

### ❌ Not using generator expressions

```cmake
# BAD: Doesn't work for multi-config generators
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# GOOD: Works with all generators
target_compile_options(my_target PRIVATE $<$<CONFIG:Release>:-O3>)
```

### ❌ Globbing source files

```cmake
# BAD: CMake can't detect new files
file(GLOB SOURCES "src/*.cpp")

# GOOD: Explicitly list sources
set(SOURCES src/main.cpp src/utils.cpp)
```

---

## Knowledge Check

Test your understanding:

1. **Why should you prefer `target_include_directories` over `include_directories`?**
   <details>
   <summary>Click for answer</summary>
   Target-based commands limit effects to a specific target, preventing dependency leakage and making the build system more maintainable and scalable.
   </details>

2. **What's the difference between PUBLIC, PRIVATE, and INTERFACE in `target_link_libraries`?**
   <details>
   <summary>Click for answer</summary>
   - PRIVATE: Only used by this target
   - PUBLIC: Used by this target and propagated to consumers
   - INTERFACE: Only propagated to consumers, not used by this target
   </details>

3. **Why is file(GLOB ...) discouraged in CMake?**
   <details>
   <summary>Click for answer</summary>
   CMake's build system doesn't automatically detect when new files are added. Explicitly listing sources ensures CMake reconfigures when the source list changes.
   </details>

---

## Next Steps

- Continue to [Memory & Cache Optimization](../02-memory-cache/) to learn about data layout
- Read the [CMake Documentation](https://cmake.org/documentation/) for more details
- Explore [Modern CMake](https://cliutils.gitlab.io/modern-cmake/) for advanced patterns
