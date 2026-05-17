# 参考资料

本页汇总了最有助于理解仓库代码、验证模型与性能方法论的外部来源。筛选标准优先考虑耐久、官方或被广泛引用的资料。

## 标准与语言参考

| 来源 | 类型 | 它在这里的重要性 |
| --- | --- | --- |
| [cppreference: C++ 标准库与语言参考](https://en.cppreference.com/w/) | 参考站点 | 对原子、ranges、对齐与示例中使用的语言规则进行快速权威查阅 |
| [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) | 持续更新的指南 | 在性能导向代码与安全性、可维护性之间提供平衡参照 |
| [CMake Presets manual](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) | 官方手册 | 直接相关，因为本仓库把 preset 视为规范构建与验证接口 |
| [GoogleTest](https://github.com/google/googletest) | 公开仓库 | 仓库使用的单元测试基础设施 |
| [RapidCheck](https://github.com/emil-e/rapidcheck) | 公开仓库 | 用于不变量导向验证的性质测试库 |

## 测量与性能分析

| 来源 | 类型 | 它在这里的重要性 |
| --- | --- | --- |
| [Google Benchmark](https://github.com/google/benchmark) | 公开仓库 | 生态中常见的微基准框架，也反映了本仓库的测量风格 |
| [perf wiki](https://perf.wiki.kernel.org/) | 官方文档 | Linux 优先计数器、采样与 profiler 使用的规范参考 |
| [Brendan Gregg 的 FlameGraph](https://www.brendangregg.com/flamegraphs.html) | 文章与工具入口 | 解释仓库性能脚本采用的 flamegraph 模型 |
| [Intel VTune Profiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) | 厂商文档 | 面向 Intel 定位问题时的高级 profiling 路线 |
| [Compiler Explorer](https://godbolt.org/) | 公共工具站点 | 方便在本地构建之外检查代码生成与向量化决策 |

## 微架构与系统性能阅读

| 来源 | 类型 | 它在这里的重要性 |
| --- | --- | --- |
| [Agner Fog 优化资料](https://www.agner.org/optimize/) | 手册与表格 | 指令成本、向量化细节与 CPU 行为的重要参考 |
| [Intel 64 and IA-32 Architectures Optimization Reference Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel64-and-ia32-architectures-optimization.html) | 厂商优化手册 | 有助于解释主流 x86 系统上的许多缓存与 SIMD 结果 |
| [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf) | 长文 | 内存与缓存模块的重要基础阅读 |
| [Gallery of Processor Cache Effects](http://igoro.com/archive/gallery-of-processor-cache-effects/) | 文章 | 用易理解的图示补充仓库示例中的缓存行为 |

## 作为对照项的公开仓库与库

| 来源 | 类型 | 它在这里的重要性 |
| --- | --- | --- |
| [xsimd](https://github.com/xtensor-stack/xsimd) | 公开仓库 | 用于对照可移植 SIMD 抽象方案 |
| [oneTBB](https://github.com/oneapi-src/oneTBB) | 公开仓库 | 生产级并行与调度的参考点 |
| [folly](https://github.com/facebook/folly) | 公开仓库 | 展示更广泛的系统工具库应有的规模与复杂度 |
| [Abseil C++](https://github.com/abseil/abseil-cpp) | 公开仓库 | 适合作为 API 表面与低层工具设计的参照 |

## 仓库内交叉链接

- [性能方法论](/zh/architecture/performance-methodology)
- [验证原则](/zh/academy/validation-doctrine)
- [性能分析指南](/zh/guides/profiling-guide)
- [相关工作](/zh/research/related-work)

这份来源书架刻意保持克制。它优先保留那些能帮助读者检查、复核或质疑仓库中具体结论的资料。
