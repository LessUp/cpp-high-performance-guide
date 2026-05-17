# 相关工作

这个仓库处在三种角色的交界处：教学指南、可执行笔记本，以及可归档维护的工程成果。下面这些工作之所以重要，是因为它们分别照亮了这三种角色中的至少一种。

## 测量与性能分析基础设施

| 工作 | 类型 | 它在这里的重要性 | 本仓库的不同之处 |
| --- | --- | --- | --- |
| [Google Benchmark](https://github.com/google/benchmark) | 公开仓库与基准库 | C++ 生态中的许多示例基准都采用它的 fixture 与报告模型 | 本仓库把基准可执行文件当作证据，但项目本身并不是基准框架 |
| [Brendan Gregg 的 FlameGraph](https://github.com/brendangregg/FlameGraph) | 公开仓库与方法论 | 它支撑了 `tools/performance/` 与 profiling 文档中引用的 flamegraph 工作流 | 本仓库教授的是何时使用 flamegraph，而不是如何维护这套工具本身 |
| [perf wiki](https://perf.wiki.kernel.org/) | 官方项目文档 | 它为实践手册中的 Linux 优先计数器与采样工作流提供框架 | 本仓库把指导收窄到具体的示例工作负载 |
| [Intel VTune Profiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) | 厂商工具文档 | 它代表面向 Intel 目标的、更细粒度的厂商级 profiling 路径 | 文档中把 VTune 当作升级路径，而不是必选依赖 |

## SIMD 与低层优化参考

| 工作 | 类型 | 它在这里的重要性 | 本仓库的不同之处 |
| --- | --- | --- | --- |
| [xsimd](https://github.com/xtensor-stack/xsimd) | 公开仓库与 SIMD 抽象库 | 它展示了现代 C++ 中一条成熟的可移植 SIMD wrapper 路径 | 本仓库保留一个更小的教学 wrapper，以便直接暴露取舍 |
| [Agner Fog 优化手册](https://www.agner.org/optimize/) | 手册与文章 | 它提供了指令吞吐、延迟与 CPU 行为的微架构背景 | 本仓库把这些思想转成小型可运行示例，而不是做成穷尽式架构目录 |
| [Intel 64 and IA-32 Architectures Optimization Reference Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel64-and-ia32-architectures-optimization.html) | 厂商优化手册 | 在 Intel 硬件上解释 SIMD 与缓存敏感结果时非常有用 | 仓库在结构上保持厂商中立，并把 Intel 材料当作多个权威来源之一 |
| [Compiler Explorer](https://godbolt.org/) | 公共工具站点 | 它非常适合检查生成代码与向量化决策 | 仓库把汇编观察放在本地基准证据旁边，但不会把它们混为一谈 |

## 并发与工业级 C++ 基础设施

| 工作 | 类型 | 它在这里的重要性 | 本仓库的不同之处 |
| --- | --- | --- | --- |
| [oneTBB](https://github.com/oneapi-src/oneTBB) | 公开仓库与任务库 | 它是可扩展并行模式与生产级调度的重要参照物 | 本仓库教授的是原子、队列与 OpenMP 等基础，而不是直接采纳完整任务运行时 |
| [folly](https://github.com/facebook/folly) | 公开仓库 | 它展示了生产规模的高性能数据结构与系统工具设计 | 本仓库偏向小而可检查的教学示例，而不是大面积工具表面 |
| [Abseil C++](https://github.com/abseil/abseil-cpp) | 公开仓库 | 它适合作为 API 设计、可移植性与低层工具工程的对照项 | 本仓库是一份带示例的指南，而不是通用依赖库 |
| [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) | 持续更新的指南集 | 它为性能工作之外的安全性与设计纪律提供长期语言 | 本仓库聚焦可测量的性能主题，而不是覆盖全部 C++ 设计议题 |

## 这组比较说明了什么

把这些工作放在一起看，可以更清楚地理解本项目想成为什么：

- **不是** oneTBB、folly 或 Abseil 这类生产级工具库的替代品
- **不是** 独立的基准或 profiling 工具
- **不是** 脱离可执行证据的泛化语言教程
- **而是** 一份紧贴代码、验证与公开参考的紧凑型性能工程指南

这种更窄的范围，本身就是仓库长期可维护性故事的一部分。
