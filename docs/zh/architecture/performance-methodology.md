# 性能方法论

本仓库对性能工作采用一套保守方法：先建立正确的基线，再隔离单一变量，用合适的 preset 进行测量，在因果不清晰时做 profiling，只有经得起重复的结论才会被发布。

## 测量契约

| 步骤 | 要做什么 | 为什么重要 |
| --- | --- | --- |
| 1. 复现 | 先构建并运行未修改的工作负载 | 避免去优化一个传闻 |
| 2. 选择 preset | 根据问题使用 `debug`、`release` 或 `relwithdebinfo` | 让结果始终绑定到明确的执行模式 |
| 3. 验证正确性 | 在相信数字前先跑相关测试 | 快但错误的代码只会制造噪声 |
| 4. 测量 | 有意识地使用基准可执行文件或计数器 | 避免把 profiler 当成仪式化动作 |
| 5. 分析 | 当原因并不明显时检查调用路径或计数器 | 解释结果为何改变 |
| 6. 重跑并记录 | 确认效果，并记下边界条件 | 让结论在未来仍可审计 |

## Preset 选择矩阵

| Preset | 最佳用途 | 典型命令 |
| --- | --- | --- |
| `debug` | 建立行为基线与排查失败测试 | `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` |
| `release` | 获取具代表性的基准数字 | `cmake --preset=release && cmake --build build/release && ctest --preset=release` |
| `relwithdebinfo` | 在保持优化的前提下进行符号化 profiling | `cmake --preset=relwithdebinfo && cmake --build build/relwithdebinfo` |
| `asan` | 调查内存安全问题 | `cmake --preset=asan && cmake --build build/asan && ctest --preset=asan` |
| `tsan` | 调查并发与内存序问题 | `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan` |
| `ubsan` | 检查低层代码中的未定义行为 | `cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan` |

## 基准协议

当仓库使用基于 Google Benchmark 的可执行文件时，基本协议很直接：

1. 构建优化后的二进制，通常位于 `build/release/examples/<module>/`
2. 运行你修改过的模块所对应的精确基准可执行文件
3. 在比较变体时保持数据集与编译器选项稳定
4. 与其追逐一次性离群值，不如做多次短而可比较的运行

典型例子包括：

```bash
./build/release/examples/02-memory-cache/aos_vs_soa_bench
./build/release/examples/04-simd-vectorization/simd_bench
```

只有当被改变的变量足够明确时，基准数字才是强证据。如果代码布局、编译器、数据集与同步策略同时变化，结果可以描述现象，却不足以诊断原因。

## Profiling 协议

当你需要解释一个结果，而不只是展示它时，再使用 profiling。

### 面向计数器的快速检查

```bash
perf stat -d ./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

适合快速判断缓存未命中、分支行为，或者粗略区分当前更像 CPU 受限还是内存受限。

### 调用路径检查

```bash
perf record -g --call-graph dwarf ./build/relwithdebinfo/examples/04-simd-vectorization/simd_bench
perf report
```

当基准发生变化，而原因并不能通过源码直观看出时，使用这一路径。

### FlameGraph 工作流

```bash
./tools/performance/generate_flamegraph.sh ./build/relwithdebinfo/examples/02-memory-cache/aos_vs_soa_bench
```

当解释受益于一个可持久保存的可视化工件，或者宽调用树需要被折叠成更易评审的图片时，使用这一路径。

## 负责任地解释结果

### 缓存与内存工作

对于内存布局改动，优先给出基于缓存行为、访问局部性或伪共享减轻的解释。只有速度提升而没有模型，结论仍然不完整。

### SIMD 工作

对于向量化，应区分以下三类工程路径：

- 编译器自动向量化
- 显式 intrinsic 或 wrapper 驱动的 SIMD
- 根据 ISA 在运行时分发实现

它们是不同的工程选择，也有不同的维护成本。

### 并发工作

对于线程相关改动，不能只把吞吐量当作充分证据。只要改动触及原子、队列或线程交互，就应把结果与同步推理以及 `tsan` 放在一起看。

## 外部锚点

本仓库的方法论刻意贴近广泛使用的公开参考资料：

- [Google Benchmark](https://github.com/google/benchmark)，用于微基准结构
- [Brendan Gregg 的 FlameGraph 项目](https://github.com/brendangregg/FlameGraph)，用于调用栈可视化
- [perf wiki](https://perf.wiki.kernel.org/)，用于 Linux 性能计数器
- [Agner Fog 的优化资料](https://www.agner.org/optimize/)，用于微架构解释

如需更完整的来源书架，请继续阅读[研究参考资料](/zh/research/references)。
