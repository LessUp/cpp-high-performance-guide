# C++ 高性能计算优化指南

[English](README.md) | 简体中文 | [文档首页](https://lessup.github.io/cpp-high-performance-guide/)

这是一个面向现代 C++20 的性能工程示例集合，覆盖构建系统、内存与缓存、SIMD、并发、基准测试与性能分析等主题。

## 仓库概览

- `examples/`：5 个主题模块，涵盖现代 CMake、内存与缓存优化、现代 C++ 特性、SIMD 与并发。
- `benchmarks/` 与 `tools/`：提供基准测试、FlameGraph 和分析脚本。
- `docs/`：提供双语学习路径、性能分析指南，以及 HonKit / GitBook 同步说明。
- `tests/`：包含单元测试、集成测试和性质测试风格的检查。

## 快速开始

```bash
cmake --preset=release
cmake --build build/release
ctest --preset=release
```

## 文档入口

- 文档站点：https://lessup.github.io/cpp-high-performance-guide/
- 学习路径：`docs/en/learning-path.md` 与 `docs/zh/learning-path.md`
- 性能分析指南：`docs/en/profiling-guide.md` 与 `docs/zh/profiling-guide.md`
- HonKit / GitBook 同步说明：`docs/en/gitbook-sync.md` 与 `docs/zh/gitbook-sync.md`

## 开发信息

- 贡献指南：`CONTRIBUTING.md` 与 `CONTRIBUTING.zh.md`
- 变更记录：`changelog/`
- 许可证：`LICENSE`
