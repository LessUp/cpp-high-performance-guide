# 04 - SIMD 与向量化

从编译器自动向量化到手写 intrinsics，再到运行时指令集分发。
原理深入分析见文档站：[SIMD 内部机制专题](../../docs/zh/deep-dives/simd-internals.md)。

## 内容

| 示例 | 主题 |
|------|------|
| `src/auto_vectorize.cpp` | 编译器自动向量化的可识别模式 |
| `src/intrinsics_intro.cpp` | SSE/AVX intrinsics 入门 |
| `src/dispatch_example_main.cpp` | 运行时 ISA 分发（一份二进制适配多种 CPU） |
| `bench/simd_bench.cpp` | 标量 vs SIMD 基准（`simd_bench` 目标） |

示例统一使用规范库 `include/hpc/simd.hpp`
（`hpc::simd::FloatVec`、`hpc::simd::detect_simd_level()` 等封装）。

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/04-simd-vectorization/simd_bench
./build/release/examples/04-simd-vectorization/dispatch_example
```

## 实测参考

`simd_bench` 的标量基线在独立的、**禁用自动向量化**的翻译单元中编译
（见 `bench/scalar_baseline.cpp` 与模块 CMakeLists），因此对比是诚实的——
否则 `-O3 -march=native` 会把"标量"基线悄悄向量化，抹平差异：

| 实现 | 典型相对速度 |
|------|--------------|
| 标量（禁用自动向量化） | 1x |
| AVX2 intrinsics | 缓存内数据约 4-8x；大数组受内存带宽限制收敛到 ~2x |

实际加速取决于 CPU 架构、数据规模与对齐、是否受内存带宽限制，以本机实测为准。
