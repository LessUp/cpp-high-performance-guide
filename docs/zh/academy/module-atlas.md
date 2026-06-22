# 模块总览

这份总览把文档叙事直接连接到真正承载论证的代码。每一行都标出一个教学模块、体现它的文件、让结论保持可信的验证表面，以及继续深入时最合适的文档入口。

## 教学模块

| 模块 | 主要代码表面 | 可复用或支撑表面 | 验证表面 | 推荐文档入口 |
| --- | --- | --- | --- | --- |
| **01. 现代 CMake** | `examples/01-cmake-modern/`、根目录 `CMakeLists.txt`、`cmake/` | `CMakePresets.json` | preset 配置与目标级构建 | [快速开始](/zh/getting-started/quickstart) |
| **02. 内存与缓存** | `examples/02-memory-cache/`、`examples/02-memory-cache/include/memory_utils.hpp`、`examples/02-memory-cache/include/particle_types.hpp` | `benchmarks/common/benchmark_utils.hpp` | `tests/unit/memory/`、`tests/property/memory_properties.cpp`、release 基准 | [学习路径](/zh/guides/learning-path) |
| **03. 现代 C++** | `examples/03-modern-cpp/`、`examples/03-modern-cpp/include/buffer.hpp`、`examples/03-modern-cpp/include/compile_time.hpp`、`examples/03-modern-cpp/include/ranges_utils.hpp`、`examples/03-modern-cpp/include/vector_reserve.hpp` | `include/hpc/core.hpp` | `tests/unit/modern_cpp/`、定向基准 | [最佳实践](/zh/guides/best-practices) |
| **04. SIMD 向量化** | `examples/04-simd-vectorization/`、`include/hpc/simd.hpp` | `examples/04-simd-vectorization/src/dispatch_example_main.cpp` | `tests/unit/simd/`、`tests/property/simd_properties.cpp`、`build/release/examples/04-simd-vectorization/simd_bench` | [优化决策树](/zh/guides/optimization-decision-tree) |
| **05. 并发** | `examples/05-concurrency/`、`examples/05-concurrency/include/concurrency_utils.hpp`、`examples/05-concurrency/include/lock_free_queue.hpp` | `examples/05-concurrency/src/` 下启用 OpenMP 的示例 | `tests/unit/concurrency/`、`tests/property/concurrency_properties.cpp`、`tsan` | [验证原则](/zh/academy/validation-doctrine) |

## 跨模块仓库表面

| 表面 | 重要性 | 常见问题 |
| --- | --- | --- |
| `include/hpc/core.hpp` | 共享的低层辅助函数与全仓库通用工具表面 | 哪些东西可以脱离单个教学模块复用？ |
| `benchmarks/common/benchmark_utils.hpp` | 通用基准测试支撑代码 | 测量如何被归一化与导出？ |
| `tests/unit/` | 细粒度正确性检查 | 哪些行为被视为契约级保证？ |
| `tests/property/` | 基于不变量的 RapidCheck 验证 | 哪些假设会在大量输入上被持续施压？ |
| `tools/performance/` | FlameGraph 与基准比较工具 | 如何从原始数字走向诊断？ |
| `docs/` | 白皮书、操作指南与参考入口 | 与可执行表面配套的解释在哪里？ |

## 如何使用这份总览

1. **先确定性能问题。** 如果问题与缓存局部性有关，就从内存模块这一行进入，而不是先去读泛化参考页。
2. **检查最近的可执行表面。** 示例模块承载主要教学论证。
3. **找到对应的证据表面。** 单元测试、性质测试、sanitizer preset 与基准各自回答不同问题。
4. **最后再概括。** 这份白皮书的目标，是让广义建议始终回到一个本地、可检查的工作负载。

## 常见穿行模式

| 如果你在问…… | 先看 | 再看 |
| --- | --- | --- |
| 这里为什么强调数据布局？ | `examples/02-memory-cache/` | [性能分析指南](/zh/guides/profiling-guide) 与 release 基准 |
| SIMD 方案的可移植性如何？ | `examples/04-simd-vectorization/` | 运行时分发示例、SIMD 测试与[研究参考资料](/zh/research/references) |
| 这里教授的内存序纪律是什么？ | `examples/05-concurrency/` | 队列测试、`tsan` 与[性能方法论](/zh/architecture/performance-methodology) |
| 构建为什么能保持可复现？ | `CMakePresets.json` 与 `cmake/` | [仓库拓扑](/zh/architecture/repository-topology) |

这份总览刻意保持具体。它存在的目的，是让熟练读者能从文字直接走到文件，同时不丢失论证链条。
