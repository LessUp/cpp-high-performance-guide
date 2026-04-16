# C++ 高性能计算优化指南

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
  <a href="README.md">English</a> | <b>简体中文</b> | <a href="https://lessup.github.io/cpp-high-performance-guide/">📚 文档</a>
</p>

<p align="center">
  <i>通过实战示例学习现代 C++20 性能工程 — CMake、内存优化、SIMD、并发和性能分析。</i>
</p>

---

## 📋 目录

- [✨ 核心特性](#-核心特性)
- [🚀 快速开始](#-快速开始)
- [📊 性能亮点](#-性能亮点)
- [📚 文档](#-文档)
- [🏗️ 项目结构](#️-项目结构)
- [👥 适合人群](#-适合人群)
- [🛠️ 技术栈](#️-技术栈)
- [🤝 参与贡献](#-参与贡献)
- [📄 许可证](#-许可证)

---

## ✨ 核心特性

| 特性 | 描述 |
|------|------|
| 🎯 **实战驱动** | 每个优化技术都有可编译、可运行、可度量的示例 |
| 📈 **量化结果** | 集成 Google Benchmark，可视化性能差异 |
| 🏗️ **现代工具链** | CMake Presets、Sanitizers、CI/CD、FlameGraph |
| 🌍 **双语文档** | 完整的中英文文档 |
| 🎓 **六周课程** | 从 CMake 基础到无锁编程的结构化学习 |
| 🔧 **工程规范** | `.clang-format`、`#pragma once`、`hpc::` 命名空间 |

---

## 🚀 快速开始

### 先决条件

- **编译器**: GCC 11+、Clang 14+ 或 MSVC 2022+
- **CMake**: 3.20 或更高版本
- **操作系统**: Linux（推荐）、macOS、Windows（WSL2）

### 安装

```bash
# 1. 克隆仓库
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# 2. 配置并构建
cmake --preset=release
cmake --build build/release

# 3. 运行测试和基准测试
ctest --preset=release
```

> 💡 **提示**: 探索其他构建预设：`debug`、`asan`、`tsan`、`ubsan`、`coverage`

### 运行你的第一个基准测试

```bash
./build/release/examples/02-memory-cache/bench/aos_soa_bench
```

见证 AOS 与 SOA 数据布局之间 2-20 倍的性能提升！

---

## 📊 性能亮点

| 模块 | 优化技术 | 加速比 | 状态 |
|------|----------|--------|------|
| **内存与缓存** | SOA vs AOS 布局 | **2-20x** | ✅ 可用 |
| **内存与缓存** | 伪共享修复 | **5-20x** | ✅ 可用 |
| **现代 C++** | 移动语义 vs 拷贝 | **10-1000x** | ✅ 可用 |
| **现代 C++** | Vector `reserve()` | **2-5x** | ✅ 可用 |
| **SIMD** | AVX2 自动向量化 | **6-8x** | ✅ 可用 |
| **SIMD** | AVX-512 内在函数 | **10-16x** | ✅ 可用 |
| **并发** | 无锁队列 | 线性扩展 | ✅ 可用 |

---

## 📚 文档

### 📖 完整文档

**[https://lessup.github.io/cpp-high-performance-guide/](https://lessup.github.io/cpp-high-performance-guide/)**

### 🎯 学习路径

跟随我们的 [6 周结构化课程](docs/zh/guides/learning-path.md)：

| 周数 | 主题 | 模块 |
|:----:|------|------|
| 1 | 构建系统 + 内存 | 现代 CMake、AOS vs SOA |
| 2 | 缓存 + 现代 C++ | 伪共享、constexpr |
| 3 | 容器 + SIMD | Vector 预留、自动向量化 |
| 4 | 高级 SIMD | 内在函数、SIMD 封装 |
| 5 | 并发编程 | 原子操作、无锁队列 |
| 6 | 性能分析 | perf、FlameGraph、VTune |

### 📁 示例模块

| 模块 | 描述 |
|------|------|
| [01. 现代 CMake](examples/01-cmake-modern/) | 基于目标的 CMake、FetchContent、Presets |
| [02. 内存与缓存](examples/02-memory-cache/) | AOS/SOA、伪共享、对齐、预取 |
| [03. 现代 C++](examples/03-modern-cpp/) | constexpr、移动语义、Ranges |
| [04. SIMD 向量化](examples/04-simd-vectorization/) | 自动向量化、SSE/AVX2/AVX-512 |
| [05. 并发编程](examples/05-concurrency/) | 原子操作、无锁编程、OpenMP |

### 🔧 快速参考

- [快速开始指南](docs/zh/getting-started/quickstart.md) - 5分钟上手
- [安装指南](docs/zh/getting-started/installation.md) - 各平台安装说明
- [性能分析指南](docs/zh/guides/profiling-guide.md) - 性能分析工具
- [常见问题](docs/zh/reference/faq.md) - 常见问题解答
- [故障排查](docs/zh/reference/troubleshooting.md) - 问题解决方案

---

## 🏗️ 项目结构

```
cpp-high-performance-guide/
├── 📁 examples/              # 5 个主题优化模块
│   ├── 01-cmake-modern/     # 基于目标的 CMake、FetchContent、Presets
│   ├── 02-memory-cache/     # AOS/SOA、伪共享、对齐、预取
│   ├── 03-modern-cpp/       # constexpr、移动语义、Ranges
│   ├── 04-simd-vectorization/ # 自动向量化、SSE/AVX2/AVX-512
│   └── 05-concurrency/       # 原子操作、无锁编程、OpenMP
├── 📁 benchmarks/           # 基准测试工具
├── 📁 tests/                # 单元测试、集成测试、属性测试
├── 📁 tools/                # FlameGraph 脚本、分析工具
├── 📁 docs/                 # 完整的双语文档
│   ├── en/                 # 英文文档
│   └── zh/                 # 中文文档
├── 📁 cmake/                # CMake 模块和模板
├── 📁 changelog/            # 版本历史
├── 📄 CMakePresets.json     # 构建配置
└── 📄 README.md             # 你在这里！
```

---

## 👥 适合人群

- **C++ 工程师** — 寻求结构化性能优化学习路径
- **学生与自学者** — 通过可执行示例学习
- **性能工程师** — 构建基准测试和性能分析技能
- **项目维护者** — 寻求现代 CMake 和 CI/CD 实践
- **教育工作者** — 教授现代 C++ 性能工程

---

## 🛠️ 技术栈

| 类别 | 技术 |
|------|------|
| **语言** | C++20 |
| **编译器** | GCC 11+、Clang 14+、MSVC（尽力支持） |
| **构建系统** | CMake 3.22+、CMake Presets、Ninja |
| **测试** | Google Test、RapidCheck |
| **基准测试** | Google Benchmark |
| **性能分析** | perf、FlameGraph、Valgrind、Intel VTune |
| **CI/CD** | GitHub Actions |
| **文档** | HonKit → GitHub Pages |

---

## 🤝 参与贡献

我们欢迎贡献！请查看我们的[贡献指南](CONTRIBUTING.zh.md)。

### 快速贡献步骤

```bash
# Fork 并克隆
git clone https://github.com/<你的用户名>/cpp-high-performance-guide.git

# 创建功能分支
git checkout -b feature/your-feature-name

# 构建并测试
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug

# 提交并推送
git commit -m "feat: add optimization example"
git push origin feature/your-feature-name
```

---

## 📄 许可证

本项目采用 [MIT 许可证](LICENSE) 授权。

---

## 🌟 Star 历史

[![Star History Chart](https://api.star-history.com/svg?repos=LessUp/cpp-high-performance-guide&type=Date)](https://star-history.com/#LessUp/cpp-high-performance-guide&Date)

---

<p align="center">
  <b>祝你优化愉快！🚀</b><br>
  <sub>用 ❤️ 由 C++ 社区构建</sub>
</p>
