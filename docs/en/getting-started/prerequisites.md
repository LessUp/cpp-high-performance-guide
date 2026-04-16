# Prerequisites

Before diving into this guide, ensure you have the necessary background knowledge and tools.

---

## Knowledge Prerequisites

### Required

| Topic | Level | Why It Matters |
|-------|-------|----------------|
| **C++ Basics** | Intermediate | Classes, templates, STL containers |
| **Pointers & Memory** | Intermediate | Understanding memory layout is crucial |
| **Command Line** | Basic | Building and running examples |
| **Computer Architecture** | Basic | CPU, memory hierarchy, caches |

### Helpful But Not Required

| Topic | Benefit |
|-------|---------|
| **CMake** | Understanding the build system |
| **Assembly basics** | Reading compiler output |
| **Parallel programming** | Concurrency section |

---

## System Requirements

### Minimum Requirements

- **OS**: Linux (Ubuntu 20.04+, Fedora 35+), macOS 12+, or Windows 10/11
- **CPU**: x86-64 with AVX support (Intel Haswell/AMD Zen or newer)
- **RAM**: 4 GB minimum, 8 GB recommended
- **Disk**: 2 GB free space

### Recommended for Optimal Experience

- **OS**: Linux (for full profiling tool support)
- **CPU**: Intel with AVX-512 or AMD with AVX2
- **RAM**: 16 GB or more
- **Compiler**: GCC 12+ or Clang 15+

### Checking Your CPU Features

```bash
# Linux - check CPU features
grep flags /proc/cpuinfo | head -1

# Look for: sse4_2, avx, avx2, avx512f
```

---

## Tool Requirements

### Essential Tools

| Tool | Minimum Version | Purpose |
|------|-----------------|---------|
| **CMake** | 3.20 | Build system |
| **C++ Compiler** | GCC 11+ / Clang 14+ / MSVC 2022+ | Compile code |
| **Git** | 2.30 | Version control |

### Optional Tools (for profiling)

| Tool | Purpose |
|------|---------|
| **perf** | Linux system profiler |
| **FlameGraph** | Performance visualization |
| **Valgrind** | Memory and cache analysis |
| **Intel VTune** | Advanced profiling (Intel CPUs) |

---

## IDEs and Editors

### Recommended

- **CLion** - Full CMake integration, excellent C++ support
- **VS Code** with C++ extension - Lightweight, extensible
- **Visual Studio 2022** - Best for Windows development

### Configuration Files Included

The project includes configuration for:
- `.vscode/` - VS Code settings and tasks
- `.clang-format` - Code formatting rules
- `.editorconfig` - Editor consistency

---

## Pre-Study Checklist

Before starting the learning path:

- [ ] You can write and compile a basic C++ program
- [ ] You understand `std::vector`, `std::map`, and basic STL
- [ ] You know what pointers and references are
- [ ] You can use the command line (cd, ls/dir, basic commands)
- [ ] You have a C++20-capable compiler installed
- [ ] You can clone a Git repository

---

## Next Step

Ready? Head to the [Quick Start](quickstart.md) or dive into the [Learning Path](../guides/learning-path.md).

Not ready? Brush up on:
- [Learn C++](https://learncpp.com/) - Free C++ tutorial
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) - Official CMake guide
