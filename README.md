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
  <a href="https://github.com/LessUp/cpp-high-performance-guide/stargazers">
    <img src="https://img.shields.io/github/stars/LessUp/cpp-high-performance-guide?style=flat-square&color=orange" alt="Stars">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/commits/master">
    <img src="https://img.shields.io/github/last-commit/LessUp/cpp-high-performance-guide?style=flat-square" alt="Last Commit">
  </a>
</p>

<p align="center">
  <b>English</b> | <a href="README.zh-CN.md">简体中文</a> | <a href="https://lessup.github.io/cpp-high-performance-guide/">📚 Documentation Home</a>
</p>

<p align="center">
  <i>Learn modern C++ performance engineering through runnable examples — covering build systems, memory & cache, SIMD, concurrency, benchmarking, and profiling.</i>
</p>

---

## 📋 Table of Contents

- [✨ Key Features](#-key-features)
- [🚀 Quick Start](#-quick-start)
- [📊 Performance Highlights](#-performance-highlights)
- [📚 Learning Path](#-learning-path)
- [📖 Documentation](#-documentation)
- [🏗️ Project Structure](#️-project-structure)
- [👥 Who This Is For](#-who-this-is-for)
- [🛠️ Tech Stack](#️-tech-stack)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🎯 **Hands-On Examples** | Every optimization technique includes compilable, runnable, and measurable code — not just theoretical notes |
| 📈 **Quantified Results** | Integrated with Google Benchmark to visualize performance differences before and after optimization |
| 🏗️ **Modern Toolchain** | CMake Presets, Sanitizers (ASan/TSan/UBSan), CI/CD, FlameGraph — production-ready practices |
| 🌍 **Bilingual Docs** | Learning paths and profiling guides maintained in both English and Chinese |
| 🎓 **6-Week Curriculum** | Structured progression from CMake basics to lock-free programming |
| 🔧 **Industrial Grade** | Code follows `.clang-format`, `#pragma once` headers, and `hpc::` namespace hierarchy |

---

## 🚀 Quick Start

Get started in 3 simple steps:

```bash
# 1. Clone the repository
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# 2. Configure and build (Release mode with -O3 -march=native)
cmake --preset=release
cmake --build build/release

# 3. Run tests and benchmarks
ctest --preset=release
```

> 💡 **Tip**: Explore other build presets: `debug`, `asan` (AddressSanitizer), `tsan` (ThreadSanitizer), `ubsan`, `coverage`

---

## 📊 Performance Highlights

| Module | Technique | Expected Speedup | Status |
|--------|-----------|------------------|--------|
| **Memory & Cache** | SOA vs AOS Layout | **2-20x** | ✅ Ready |
| **Memory & Cache** | False Sharing Fix | **5-20x** | ✅ Ready |
| **Modern C++** | Move vs Copy Semantics | **10-1000x** | ✅ Ready |
| **Modern C++** | Vector `reserve()` | **2-5x** | ✅ Ready |
| **SIMD** | AVX2 Auto-vectorization | **6-8x** | ✅ Ready |
| **SIMD** | AVX-512 Intrinsics | **10-16x** | ✅ Ready |
| **Concurrency** | Lock-Free Queue | Linear scaling | ✅ Ready |

> 📈 Run `./build/release/examples/XX-*/bench/*_bench` to see results on your machine!

---

## 📚 Learning Path

### 6-Week Study Plan

| Week | Topics | Modules |
|:----:|--------|---------|
| 1 | Build System + Memory Fundamentals | [Modern CMake](examples/01-cmake-modern/), [AOS vs SOA](examples/02-memory-cache/) |
| 2 | Cache Optimization + Modern C++ Basics | [False Sharing](examples/02-memory-cache/), [constexpr](examples/03-modern-cpp/) |
| 3 | Container Optimization + Auto-vectorization | [Vector Reserve](examples/03-modern-cpp/), [SIMD Intro](examples/04-simd-vectorization/) |
| 4 | Advanced SIMD Programming | [Intrinsics](examples/04-simd-vectorization/), [SIMD Wrapper](examples/04-simd-vectorization/) |
| 5 | Concurrent Programming | [Atomics](examples/05-concurrency/), [Lock-Free Queue](examples/05-concurrency/) |
| 6 | Profiling & Benchmarking | [OpenMP](examples/05-concurrency/), [Profiling Guide](docs/en/profiling-guide.md) |

### Quick Navigation

| Goal | Resource |
|------|----------|
| 🎓 **Structured Learning** | [Learning Path](docs/en/learning-path.md) |
| 🔍 **Find Bottlenecks** | [Profiling Guide](docs/en/profiling-guide.md) |
| 📝 **Add a Module** | [Contributing Guide](CONTRIBUTING.md) |
| ❓ **Common Issues** | [FAQ](docs/en/faq.md) |
| 🐛 **Troubleshooting** | [Troubleshooting](docs/en/troubleshooting.md) |

---

## 📖 Documentation

### Example Modules

Explore our 5 hands-on example modules:

#### [01. Modern CMake](examples/01-cmake-modern/)
Learn target-based CMake, FetchContent dependency management, and reproducible builds with CMake Presets.

#### [02. Memory & Cache](examples/02-memory-cache/)
Master AOS vs SOA data layout, false sharing mitigation, memory alignment, and prefetching techniques.

#### [03. Modern C++](examples/03-modern-cpp/)
Leverage `constexpr`, move semantics, vector capacity management, and C++20 Ranges for zero-cost abstractions.

#### [04. SIMD Vectorization](examples/04-simd-vectorization/)
Write compiler-friendly loops, use SSE/AVX2/AVX-512 intrinsics, and build readable SIMD wrappers.

#### [05. Concurrency](examples/05-concurrency/)
Implement atomic operations with proper memory ordering, build lock-free data structures, and parallelize with OpenMP.

### Additional Resources

- 📘 [Full Learning Path](docs/en/learning-path.md) - Step-by-step curriculum
- 🔬 [Profiling Guide](docs/en/profiling-guide.md) - perf, FlameGraph, Valgrind, VTune
- 🔄 [GitBook Sync](docs/en/gitbook-sync.md) - Documentation site setup
- 💻 [Online Documentation](https://lessup.github.io/cpp-high-performance-guide/)

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
├── 📁 benchmarks/           # Benchmark utilities and common code
├── 📁 tests/                # Unit, integration, and property tests
├── 📁 tools/                # FlameGraph generators, analysis scripts
├── 📁 docs/                 # Bilingual documentation
│   ├── en/                 # English docs
│   └── zh/                 # Chinese docs
├── 📁 cmake/                # CMake modules and templates
├── 📄 CMakePresets.json     # Build configurations
└── 📄 README.md             # You are here!
```

---

## 👥 Who This Is For

- **C++ Engineers** looking for a structured path to master performance optimization
- **Students & Self-learners** who learn best through executable examples with benchmarks
- **Performance Engineers** needing to validate optimization strategies and tooling
- **Project Maintainers** seeking profiling, benchmarking, and contribution references
- **Educators** teaching modern C++ performance engineering

---

## 🛠️ Tech Stack

| Category | Technologies |
|----------|--------------|
| **Language** | C++20 |
| **Compilers** | GCC 11+, Clang 14+, MSVC (best-effort) |
| **Build System** | CMake 3.22+, CMake Presets, Ninja |
| **Testing** | Google Test, RapidCheck (property tests) |
| **Benchmarking** | Google Benchmark |
| **Profiling** | perf, FlameGraph, Valgrind, Intel VTune |
| **CI/CD** | GitHub Actions (Build, Sanitizers, Pages) |
| **Documentation** | HonKit → GitHub Pages |

---

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Quick Contributing Steps

```bash
# Fork and clone
git clone https://github.com/<your-username>/cpp-high-performance-guide.git

# Create feature branch
git checkout -b feature/your-feature-name

# Build and test
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan

# Commit with clear messages
git commit -m "feat: add matrix multiplication SIMD example"

# Push and create PR
git push origin feature/your-feature-name
```

### Ways to Contribute

- 🐛 Fix bugs and improve examples
- 📚 Improve documentation and translations
- ✨ Add new optimization examples
- 🧪 Add property-based tests
- 📊 Improve benchmark coverage

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
