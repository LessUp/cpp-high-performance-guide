# 参与贡献到 C++ 高性能指南

[![Contributors](https://img.shields.io/badge/Contributors-Welcome-brightgreen.svg)](https://github.com/LessUp/cpp-high-performance-guide/blob/master/CONTRIBUTING.zh.md)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

感谢你对本项目的关注！本文档介绍如何为本项目贡献内容。

> 💡 **第一次贡献？** 查看我们的 [Good First Issues](https://github.com/LessUp/cpp-high-performance-guide/labels/good%20first%20issue) 来开始吧！

## 开始之前

1. Fork 本仓库
2. 克隆你的 Fork：
   ```bash
   git clone https://github.com/<your-username>/cpp-high-performance-guide.git
   ```
3. 创建功能分支：
   ```bash
   git checkout -b feature/your-feature-name
   ```

## 构建与测试

```bash
# Debug 构建并启用测试
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug

# Release 构建并运行基准测试
cmake --preset=release
cmake --build build/release
ctest --preset=release
```

## 代码风格

- 遵循项目根目录中的 `.clang-format` 配置
- 提交前格式化代码：
  ```bash
  find examples tests benchmarks -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
  ```
- 头文件统一使用 `#pragma once`
- 代码放在 `hpc::` 命名空间层级下

## 添加新的示例模块

1. 在 `examples/` 下创建新目录，遵循命名规则：`XX-topic-name/`
2. 使用统一结构：
   ```
   examples/XX-topic-name/
   ├── src/           # 带 demo main() 的源码文件
   ├── bench/         # Google Benchmark 文件
   ├── include/       # 可复用头文件（可选）
   ├── CMakeLists.txt # 使用 ExampleTemplate.cmake 中的 hpc_add_example()
   └── README.md      # 模块文档
   ```
3. 在 `examples/CMakeLists.txt` 中注册子目录
4. 在 `tests/` 下补充对应测试

## 测试要求

提交 Pull Request 之前，请确保：

1. **所有测试通过：**
   ```bash
   cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
   ```

2. **没有 AddressSanitizer 错误：**
   ```bash
   cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
   ```

3. **没有 ThreadSanitizer 错误**（并发代码尤其重要）：
   ```bash
   cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
   ```

4. **没有 UndefinedBehaviorSanitizer 错误：**
   ```bash
   cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
   ```

## Commit Message 建议

请使用清晰、可描述的提交信息：
- `feat: add matrix multiplication SIMD example`
- `fix: resolve false sharing in concurrent counter`
- `docs: update learning path with new module`
- `bench: add parameterized benchmark for prefetch distance`
- `test: add property tests for lock-free queue`

## Pull Request 流程

1. 确保你的分支与 `main` 保持同步
2. 确保所有 CI 检查通过
3. 在 PR 描述中清楚说明本次修改的内容和原因
4. 如有相关 issue，请在 PR 中引用

## Issue 和 PR 模板

我们提供了模板来帮助你提交有效的问题和 Pull Request：

- **Bug Report（错误报告）**：用于报告代码、构建或文档中的错误
- **Feature Request（功能请求）**：用于建议新的示例或增强功能
- **Documentation（文档）**：用于报告文档问题

创建新 Issue 时请选择恰当的模板。

## 行为准则

本项目遵循行为准则，我们期望所有贡献者遵守：

- 在所有互动中保持尊重和建设性
- 欢迎新人并帮助他们入门
- 专注于对社区和学习者最有利的事情
- 对他人保持同理心

## 文档同步

当进行影响文档的更改时：

1. **代码更改**：更新受影响示例的相关 README 文件
2. **新示例**：建议同时提供英文和中文 README 文件
3. **API 更改**：在相关模块的 README 中记录

虽然理想的 bilingual 同步是完美的，但仅英文的贡献也是受欢迎的 —— 维护者将协助中文翻译。

## License

通过提交贡献，你同意你的贡献将以 MIT License 进行许可。
