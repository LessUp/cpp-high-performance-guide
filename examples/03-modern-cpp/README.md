# 03 - 现代 C++ 性能特性

constexpr/consteval、移动语义、容器容量管理与 C++20 Ranges 的零成本抽象。
原理深入分析见文档站：[现代 C++ 性能专题](../../docs/zh/deep-dives/modern-cpp-perf.md)。

## 内容

| 示例 | 基准目标 | 主题 |
|------|----------|------|
| `src/compile_time.cpp` | （无基准） | constexpr/consteval 编译期计算 |
| `src/move_semantics.cpp` | `move_semantics_bench` | 移动语义避免深拷贝 |
| `src/vector_reserve.cpp` | `vector_reserve_bench` | reserve 减少重分配 |
| `src/ranges_vs_loops.cpp` | `ranges_vs_loops_bench` | C++20 Ranges vs 手写循环 |

Buffer 与编译期工具来自规范库 `include/hpc/`
（`hpc::move_semantics`、`hpc::compile_time`、`hpc::vector_reserve`、`hpc::ranges`）。

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/03-modern-cpp/move_semantics_bench
./build/release/examples/03-modern-cpp/vector_reserve_bench
./build/release/examples/03-modern-cpp/ranges_vs_loops_bench
```

## 实测参考

| 基准 | 典型结果 | 说明 |
|------|----------|------|
| Move vs Copy | 10-1000x | 随对象数据量增大；本例 1MB Buffer 下差异显著 |
| Reserve vs 无 Reserve | 1.5-3x | 取决于元素数量与容器增长策略 |
| Ranges vs Loops | ≈1x | 零成本抽象：惰性视图与手写循环基本等价 |

以本机实测为准。
