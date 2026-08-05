# 02 - 内存与缓存优化

数据布局、对齐、预取——现代 CPU 上最常被忽视、收益却最大的优化面。
原理深入分析见文档站：[内存布局深度专题](../../docs/zh/deep-dives/memory-layout.md)。

## 内容

| 示例 | 基准目标 | 主题 |
|------|----------|------|
| `src/aos_vs_soa.cpp` | `aos_vs_soa_bench` | AoS vs SoA 数据布局 |
| `src/false_sharing.cpp` | `false_sharing_bench` | 伪共享与缓存行填充 |
| `src/alignment.cpp` | `alignment_bench` | 内存对齐与 SIMD 加载 |
| `src/prefetch.cpp` | `prefetch_bench` | 软件预取 |

所有示例依赖仓库根的规范库 `include/hpc/`（`hpc::memory` 命名空间）。

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/02-memory-cache/aos_vs_soa_bench
./build/release/examples/02-memory-cache/false_sharing_bench
./build/release/examples/02-memory-cache/alignment_bench
./build/release/examples/02-memory-cache/prefetch_bench
```

## 实测参考

| 基准 | 典型结果 | 说明 |
|------|----------|------|
| SoA vs AoS | 2-4x | 收益主要来自可向量化性与流式访问效率 |
| 填充 vs 紧凑计数器（伪共享） | 数倍至 10x+ | 线程数越多、争用越久越明显 |
| 对齐 vs 未对齐（SIMD） | ≈1.0x | 现代 CPU 对不跨缓存行的未对齐访问惩罚接近零（`src/alignment.cpp` 内有说明） |
| 软件预取 vs 无预取 | ≈1.0x（顺序访问） | 硬件预取器已覆盖顺序模式；收益出现在不规则/跨步访问场景 |

结果随 CPU 架构与数据规模变化，以本机实测为准。
性能剖析工作流见 [Profiling 指南](../../docs/zh/guides/profiling.md)。
