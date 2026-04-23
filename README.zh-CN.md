# C++ 高性能指南

<p align="center">
  <a href="https://github.com/LessUp/cpp-high-performance-guide/stargazers">
    <img src="https://img.shields.io/github/stars/LessUp/cpp-high-performance-guide?style=flat-square&color=yellow" alt="Stars">
  </a>
  <a href="https://github.com/LessUp/cpp-high-performance-guide/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/LessUp/cpp-high-performance-guide/ci.yml?branch=master&label=CI&style=flat-square" alt="CI">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square" alt="License">
  </a>
</p>

<p align="center">
  <a href="README.md">English</a> | <b>简体中文</b> | <a href="https://lessup.github.io/cpp-high-performance-guide/">GitHub Pages</a> | <a href="https://github.com/LessUp/cpp-high-performance-guide/discussions">Discussions</a>
</p>

<p align="center">
  通过可运行示例、基准测试和 VitePress 文档站，学习可验证的现代 C++20 性能工程。
</p>

## 这个仓库解决什么问题

这个项目关注那些“很容易被谈论、但不容易被验证”的 C++ 性能主题：

- 现代 CMake 与 preset 驱动的构建流程
- 内存与缓存布局取舍
- 现代 C++ 的性能实践
- SIMD 与向量化
- 并发与无锁基础
- 基于 benchmark 与 profiling 的性能分析

每个主题都尽量做到 **可阅读、可构建、可测量**。

## 你可以从哪里开始

| 区域 | 内容 |
| --- | --- |
| `examples/01-cmake-modern/` | 现代 CMake 结构与反模式 |
| `examples/02-memory-cache/` | AOS vs SOA、伪共享、对齐、预取 |
| `examples/03-modern-cpp/` | constexpr、移动语义、reserve、ranges |
| `examples/04-simd-vectorization/` | 自动向量化、intrinsics、SIMD 封装 |
| `examples/05-concurrency/` | 原子操作、无锁队列、OpenMP |
| `docs/` | 双语学习路径、profiling、排障、参考文档 |
| `openspec/` | 规格驱动开发流程与 change 历史 |

## 快速开始

```bash
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

cmake --preset=release
cmake --build build/release
```

运行一个 benchmark：

```bash
./build/release/examples/02-memory-cache/aos_soa_bench
```

## 常用验证命令

```bash
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release

cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```

## 文档与学习路径

- **文档站：** <https://lessup.github.io/cpp-high-performance-guide/>
- **快速开始：** `docs/zh/getting-started/quickstart.md`
- **学习路径：** `docs/zh/guides/learning-path.md`
- **性能分析指南：** `docs/zh/guides/profiling-guide.md`
- **英文入口：** `README.md` 与 `docs/en/`

## 开发流程

本仓库对非 trivial 修改使用 **OpenSpec**：

1. 先阅读 `openspec/specs/` 中受影响的 capability spec。
2. 在 `openspec/changes/<change-id>/` 下创建或更新一个 change。
3. 按任务依赖顺序实施。
4. 使用 preset 驱动的命令做验证。
5. 非 trivial 修改在合并前使用 `/review`。
6. 完成后归档 change。

仓库也为 AI 工具保留了明确说明：

- `AGENTS.md`
- `CLAUDE.md`
- `.github/copilot-instructions.md`

## 技术栈

- **语言：** C++20
- **构建：** CMake 3.22+、Ninja
- **测试：** Google Test、RapidCheck
- **基准测试：** Google Benchmark
- **文档：** VitePress + GitHub Pages
- **性能分析：** perf、FlameGraph、Valgrind、VTune

## 贡献

贡献流程、hooks 配置与 OpenSpec 约定见 `CONTRIBUTING.zh.md`。
