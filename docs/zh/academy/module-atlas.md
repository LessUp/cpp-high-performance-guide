# 模块总览

本页将当前学习表面与它们解释的仓库区域对应起来。

## 核心模块

| 模块 | 主要仓库表面 | 主要文档入口 |
| --- | --- | --- |
| 构建与工具链 | `CMakeLists.txt`、`CMakePresets.json`、`cmake/` | [快速开始](/zh/getting-started/quickstart) |
| 内存与缓存 | `examples/`、`benchmarks/`、`include/hpc/` | [学习路径](/zh/guides/learning-path) |
| SIMD 与向量化 | `include/hpc/`、`tests/`、`benchmarks/` | [优化决策树](/zh/guides/optimization-decision-tree) |
| 并发与验证 | `tests/`、`include/hpc/`、各类 preset | [验证原则](./validation-doctrine) |
| 参考资料表面 | `docs/en/reference/`、`docs/zh/reference/` | [API 入口](/zh/reference/api-reference) |

## 建议阅读顺序

1. 先从[实践手册](/zh/playbook/)进入，跑通基本环境。
2. 使用[学习路径](/zh/guides/learning-path)安排主题顺序。
3. 需要刻意练习时进入[练习](/en/exercises/README)。
4. 需要仓库级背景时回到[架构](/zh/architecture/)。
