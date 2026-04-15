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
  <a href="https://github.com/LessUp/cpp-high-performance-guide/stargazers">
    <img src="https://img.shields.io/github/stars/LessUp/cpp-high-performance-guide?style=flat-square&color=orange" alt="Stars">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/commits/master">
    <img src="https://img.shields.io/github/last-commit/LessUp/cpp-high-performance-guide?style=flat-square" alt="Last Commit">
  </a>
</p>

<p align="center">
  <a href="README.md">English</a> | <b>简体中文</b> | <a href="https://lessup.github.io/cpp-high-performance-guide/">📚 文档首页</a>
</p>

<p align="center">
  <i>通过可运行的示例学习现代 C++ 性能工程 — 覆盖构建系统、内存与缓存、SIMD、并发、基准测试和性能分析</i>
</p>

---

## 📋 目录

- [✨ 核心特性](#-核心特性)
- [🚀 快速开始](#-快速开始)
- [📊 性能亮点](#-性能亮点)
- [📚 学习路径](#-学习路径)
- [📖 文档导航](#-文档导航)
- [🏗️ 项目结构](#️-项目结构)
- [👥 适合人群](#-适合人群)
- [🛠️ 技术栈](#️-技术栈)
- [🤝 参与贡献](#-参与贡献)
- [📄 许可证](#-许可证)

---

## ✨ 核心特性

| 特性 | 描述 |
|------|------|
| 🎯 **实战驱动** | 每个优化技术都有可编译、可运行、可度量的示例代码，而非孤立的知识笔记 |
| 📈 **量化结果** | 集成 Google Benchmark，可视化优化前后的性能差异 |
| 🏗️ **现代工具链** | CMake Presets、Sanitizers（ASan/TSan/UBSan）、CI/CD、FlameGraph — 工业级实践 |
| 🌍 **双语文档** | 学习路径和性能分析指南同步维护中英文版本 |
| 🎓 **六周课程** | 从 CMake 基础到无锁编程的结构化进阶路径 |
| 🔧 **工程规范** | 代码遵循 `.clang-format`、头文件使用 `#pragma once`、`hpc::` 命名空间层级 |

---

## 🚀 快速开始

只需 3 步即可开始：

```bash
# 1. 克隆仓库
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# 2. 配置并构建（Release 模式，启用了 -O3 -march=native）
cmake --preset=release
cmake --build build/release

# 3. 运行测试和基准测试
ctest --preset=release
```

> 💡 **提示**: 探索其他构建预设：`debug`、`asan`（AddressSanitizer）、`tsan`（ThreadSanitizer）、`ubsan`、`coverage`

---

## 📊 性能亮点

| 模块 | 优化技术 | 预期加速比 | 状态 |
|------|----------|-----------|------|
| **内存与缓存** | SOA vs AOS 数据布局 | **2-20x** | ✅ 可用 |
| **内存与缓存** | 伪共享修复 | **5-20x** | ✅ 可用 |
| **现代 C++** | 移动语义 vs 拷贝 | **10-1000x** | ✅ 可用 |
| **现代 C++** | Vector `reserve()` | **2-5x** | ✅ 可用 |
| **SIMD** | AVX2 自动向量化 | **6-8x** | ✅ 可用 |
| **SIMD** | AVX-512 内在函数 | **10-16x** | ✅ 可用 |
| **并发** | 无锁队列 | 线性扩展 | ✅ 可用 |

> 📈 在你的机器上运行 `./build/release/examples/XX-*/bench/*_bench` 查看实际结果！

---

## 📚 学习路径

### 六周学习计划

| 周数 | 主题 | 模块 |
|:----:|------|------|
| 1 | 构建系统 + 内存基础 | [现代 CMake](examples/01-cmake-modern/)、[AOS vs SOA](examples/02-memory-cache/) |
| 2 | 缓存优化 + 现代 C++ 基础 | [伪共享](examples/02-memory-cache/)、[constexpr](examples/03-modern-cpp/) |
| 3 | 容器优化 + 自动向量化 | [Vector 预留](examples/03-modern-cpp/)、[SIMD 入门](examples/04-simd-vectorization/) |
| 4 | 高级 SIMD 编程 | [内在函数](examples/04-simd-vectorization/)、[SIMD 封装](examples/04-simd-vectorization/) |
| 5 | 并发编程 | [原子操作](examples/05-concurrency/)、[无锁队列](examples/05-concurrency/) |
| 6 | 性能分析与基准测试 | [OpenMP](examples/05-concurrency/)、[性能分析指南](docs/zh/profiling-guide.md) |

### 快速导航

| 目标 | 资源 |
|------|------|
| 🎓 **系统学习** | [学习路径](docs/zh/learning-path.md) |
| 🔍 **定位瓶颈** | [性能分析指南](docs/zh/profiling-guide.md) |
| 📝 **新增模块** | [贡献指南](CONTRIBUTING.zh.md) |
| ❓ **常见问题** | [FAQ](docs/zh/faq.md) |
| 🐛 **故障排查** | [故障排查指南](docs/zh/troubleshooting.md) |

---

## 📖 文档导航

### 示例模块

探索我们的 5 个实战示例模块：

#### [01. 现代 CMake](examples/01-cmake-modern/)
学习基于目标的 CMake、FetchContent 依赖管理和使用 CMake Presets 的可复现构建。

#### [02. 内存与缓存](examples/02-memory-cache/)
掌握 AOS vs SOA 数据布局、伪共享消除、内存对齐和预取技术。

#### [03. 现代 C++](examples/03-modern-cpp/)
利用 `constexpr`、移动语义、容器容量管理和 C++20 Ranges 实现零开销抽象。

#### [04. SIMD 向量化](examples/04-simd-vectorization/)
编写编译器友好的循环、使用 SSE/AVX2/AVX-512 内在函数、构建可读的 SIMD 封装。

#### [05. 并发编程](examples/05-concurrency/)
实现带正确内存序的原子操作、构建无锁数据结构、使用 OpenMP 并行化。

### 其他资源

- 📘 [完整学习路径](docs/zh/learning-path.md) - 分步骤课程
- 🔬 [性能分析指南](docs/zh/profiling-guide.md) - perf、FlameGraph、Valgrind、VTune
- 🔄 [GitBook 同步](docs/zh/gitbook-sync.md) - 文档站点设置
- 💻 [在线文档](https://lessup.github.io/cpp-high-performance-guide/)

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
├── 📁 benchmarks/           # 基准测试工具和公共代码
├── 📁 tests/                # 单元测试、集成测试和属性测试
├── 📁 tools/                # FlameGraph 生成器、分析脚本
├── 📁 docs/                 # 双语文档
│   ├── en/                 # 英文文档
│   └── zh/                 # 中文文档
├── 📁 cmake/                # CMake 模块和模板
├── 📄 CMakePresets.json     # 构建配置
└── 📄 README.md             # 你在这里！
```

---

## 👥 适合人群

- **C++ 工程师** — 需要结构化的性能优化学习路径
- **学生与自学者** — 通过带基准测试的可执行示例学习
- **性能工程师** — 需要验证优化策略和工具链
- **项目维护者** — 需要性能分析、基准测试和贡献参考
- **教育工作者** — 教授现代 C++ 性能工程

---

## 🛠️ 技术栈

| 类别 | 技术 |
|------|------|
| **语言** | C++20 |
| **编译器** | GCC 11+、Clang 14+、MSVC（尽力支持） |
| **构建系统** | CMake 3.22+、CMake Presets、Ninja |
| **测试** | Google Test、RapidCheck（属性测试） |
| **基准测试** | Google Benchmark |
| **性能分析** | perf、FlameGraph、Valgrind、Intel VTune |
| **CI/CD** | GitHub Actions（构建、Sanitizers、Pages） |
| **文档** | HonKit → GitHub Pages |

---

## 🤝 参与贡献

我们欢迎贡献！详情请参阅我们的 [贡献指南](CONTRIBUTING.zh.md)。

### 快速贡献步骤

```bash
# Fork 并克隆
git clone https://github.com/<your-username>/cpp-high-performance-guide.git

# 创建功能分支
git checkout -b feature/your-feature-name

# 构建并测试
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan

# 使用清晰的提交信息
git commit -m "feat: add matrix multiplication SIMD example"

# 推送并创建 PR
git push origin feature/your-feature-name
```

### 贡献方式

- 🐛 修复 bug 并改进示例
- 📚 改进文档和翻译
- ✨ 添加新的优化示例
- 🧪 添加基于属性的测试
- 📊 改进基准测试覆盖

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
