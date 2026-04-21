# C++ High Performance Computing Optimization Guide

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg?style=for-the-badge&logo=c%2B%2B" alt="C++20">
  <img src="https://img.shields.io/badge/CMake-3.22%2B-green.svg?style=for-the-badge&logo=cmake" alt="CMake 3.22+">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg?style=for-the-badge" alt="Platform">
</p>

<p align="center">
  <a href="https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/LessUp/cpp-high-performance-guide/ci.yml?branch=master&label=CI&style=flat-square" alt="CI">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/pages.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/LessUp/cpp-high-performance-guide/pages.yml?label=Docs&style=flat-square" alt="Docs">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square" alt="License: MIT">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/releases">
    <img src="https://img.shields.io/github/v/release/LessUp/cpp-high-performance-guide?style=flat-square&color=orange" alt="Release">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/commits/master">
    <img src="https://img.shields.io/github/last-commit/LessUp/cpp-high-performance-guide?style=flat-square" alt="Last Commit">
  </a>
</p>

<p align="center">
  <b>English</b> | <a href="README.zh-CN.md">简体中文</a> | <a href="https://lessup.github.io/cpp-high-performance-guide/">📚 Documentation</a>
</p>

<p align="center">
  <i>Modern C++20 performance engineering through hands-on examples — CMake, memory optimization, SIMD, concurrency, and profiling.</i>
</p>

---

## 📋 Table of Contents

- [✨ Key Features](#-key-features)
- [🚀 Quick Start](#-quick-start)
- [📊 Performance Highlights](#-performance-highlights)
- [📚 Documentation](#-documentation)
- [🏗️ Project Structure](#️-project-structure)
- [👥 Who This Is For](#-who-this-is-for)
- [🛠️ Tech Stack](#️-tech-stack)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🎯 **Hands-On Examples** | Every optimization has compilable, runnable, and measurable code |
| 📈 **Quantified Results** | Google Benchmark integration for performance visualization |
| 🏗️ **Modern Toolchain** | CMake Presets, Sanitizers, CI/CD, FlameGraph |
| 🌍 **Bilingual Docs** | Complete English and Chinese documentation |
| 🎓 **6-Week Curriculum** | Structured learning from CMake basics to lock-free programming |
| 🔧 **Industrial Grade** | `.clang-format`, `#pragma once`, `hpc::` namespace hierarchy |

---

## 🚀 Quick Start

### Prerequisites

- **Compiler**: GCC 11+, Clang 14+, or MSVC 2022+
- **CMake**: 3.20 or higher
- **OS**: Linux (recommended), macOS, Windows (WSL2)

### Installation

```bash
# 1. Clone the repository
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# 2. Configure and build
cmake --preset=release
cmake --build build/release

# 3. Run tests and benchmarks
ctest --preset=release
```

> 💡 **Tip**: Explore other build presets: `debug`, `asan`, `tsan`, `ubsan`, `coverage`

### Run Your First Benchmark

```bash
./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

See 2-20x speedup between AOS and SOA data layouts!

---

## 📊 Performance Highlights

| Module | Technique | Speedup | Status |
|--------|-----------|---------|--------|
| **Memory & Cache** | SOA vs AOS Layout | **2-20x** | ✅ Ready |
| **Memory & Cache** | False Sharing Fix | **5-20x** | ✅ Ready |
| **Modern C++** | Move vs Copy Semantics | **10-1000x** | ✅ Ready |
| **Modern C++** | Vector `reserve()` | **2-5x** | ✅ Ready |
| **SIMD** | AVX2 Auto-vectorization | **6-8x** | ✅ Ready |
| **SIMD** | AVX-512 Intrinsics | **10-16x** | ✅ Ready |
| **Concurrency** | Lock-Free Queue | Linear scaling | ✅ Ready |

---

## 📚 Documentation

### 📖 Full Documentation

**[https://lessup.github.io/cpp-high-performance-guide/](https://lessup.github.io/cpp-high-performance-guide/)**

### 🎯 Learning Path

Follow our [6-week structured curriculum](docs/en/guides/learning-path.md):

| Week | Topics | Modules |
|:----:|--------|---------|
| 1 | Build System + Memory | Modern CMake, AOS vs SOA |
| 2 | Cache + Modern C++ | False Sharing, constexpr |
| 3 | Containers + SIMD | Vector Reserve, Auto-vectorize |
| 4 | Advanced SIMD | Intrinsics, SIMD Wrapper |
| 5 | Concurrency | Atomics, Lock-Free Queue |
| 6 | Profiling | perf, FlameGraph, VTune |

### 📁 Example Modules

| Module | Description |
|--------|-------------|
| [01. Modern CMake](examples/01-cmake-modern/) | Target-based CMake, FetchContent, Presets |
| [02. Memory & Cache](examples/02-memory-cache/) | AOS/SOA, False Sharing, Alignment, Prefetch |
| [03. Modern C++](examples/03-modern-cpp/) | constexpr, Move Semantics, Ranges |
| [04. SIMD Vectorization](examples/04-simd-vectorization/) | Auto-vectorize, SSE/AVX2/AVX-512 |
| [05. Concurrency](examples/05-concurrency/) | Atomics, Lock-Free, OpenMP |

### 🔧 Quick Reference

- [Quick Start Guide](docs/en/getting-started/quickstart.md) - Get started in 5 minutes
- [Installation](docs/en/getting-started/installation.md) - Platform-specific setup
- [Profiling Guide](docs/en/guides/profiling-guide.md) - Performance analysis tools
- [FAQ](docs/en/reference/faq.md) - Common questions
- [Troubleshooting](docs/en/reference/troubleshooting.md) - Problem solving

---

## 🏗️ Project Structure

```
cpp-high-performance-guide/
├── 📁 examples/              # 5 themed optimization modules
│   ├── 01-cmake-modern/     # Target-based CMake, FetchContent, Presets
│   ├── 02-memory-cache/     # AOS/SOA, False Sharing, Alignment, Prefetch
│   ├── 03-modern-cpp/       # constexpr, Move Semantics, Ranges
│   ├── 04-simd-vectorization/ # Auto-vectorize, SSE/AVX2/AVX-512
│   └── 05-concurrency/       # Atomics, Lock-Free, OpenMP
├── 📁 benchmarks/           # Benchmark utilities
├── 📁 tests/                # Unit, integration, property tests
├── 📁 tools/                # FlameGraph scripts, analysis tools
├── 📁 docs/                 # Complete bilingual documentation
│   ├── en/                 # English docs
│   └── zh/                 # Chinese docs
├── 📁 cmake/                # CMake modules and templates
├── 📁 changelog/            # Version history
├── 📄 CMakePresets.json     # Build configurations
└── 📄 README.md             # You are here!
```

---

## 👥 Who This Is For

- **C++ Engineers** seeking structured performance optimization learning
- **Students & Self-learners** who learn through executable examples
- **Performance Engineers** building benchmarking and profiling skills
- **Project Maintainers** seeking modern CMake and CI/CD practices
- **Educators** teaching modern C++ performance engineering

---

## 🛠️ Tech Stack

| Category | Technologies |
|----------|--------------|
| **Language** | C++20 |
| **Compilers** | GCC 11+, Clang 14+, MSVC (best-effort) |
| **Build System** | CMake 3.22+, CMake Presets, Ninja |
| **Testing** | Google Test, RapidCheck |
| **Benchmarking** | Google Benchmark |
| **Profiling** | perf, FlameGraph, Valgrind, Intel VTune |
| **CI/CD** | GitHub Actions |
| **Documentation** | HonKit → GitHub Pages |

---

## 🤝 Contributing

We welcome contributions! See our [Contributing Guide](CONTRIBUTING.md).

### Quick Contributing Steps

```bash
# Fork and clone
git clone https://github.com/<your-username>/cpp-high-performance-guide.git

# Create feature branch
git checkout -b feature/your-feature-name

# Build and test
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug

# Commit and push
git commit -m "feat: add optimization example"
git push origin feature/your-feature-name
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

---

## 🌟 Star History

[![Star History Chart](https://api.star-history.com/svg?repos=LessUp/cpp-high-performance-guide&type=Date)](https://star-history.com/#LessUp/cpp-high-performance-guide&Date)

---

<p align="center">
  <b>Happy Optimizing! 🚀</b><br>
  <sub>Built with ❤️ by the C++ community</sub>
</p>
