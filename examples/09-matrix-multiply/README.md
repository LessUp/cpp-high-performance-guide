# 09 - 矩阵乘法案例研究

GEMM（C = A·B）是性能优化的经典载体：内存层次、SIMD、多核三个瓶颈依次
显现，每一步优化都有清晰的物理原因。四阶段演进与逐阶段实测见文档站：
[GEMM 案例研究专题](../../docs/zh/deep-dives/gemm-case-study.md)。

## 四个阶段（规范库 `include/hpc/gemm.hpp`）

| 阶段 | 函数 | 关键思想 |
|------|------|----------|
| 1 | `hpc::gemm::naive` | 教科书 ijk 循环；B 按列访问，缓存极不友好 |
| 2 | `hpc::gemm::tiled` | 缓存分块（tiling），块内 i,k,j 序，B/C 行连续 |
| 3 | `hpc::gemm::simd` | 分块 + FMA 微内核（AVX2 每指令 8 float / NEON 4） |
| 4 | `hpc::gemm::parallel` | SIMD + OpenMP 按行并行（无 OpenMP 时退化为 simd） |

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/09-matrix-multiply/matrix_multiply
./build/release/examples/09-matrix-multiply/matrix_multiply_bench
```

示例对每个阶段都**对照 naive 验证数值正确**（不同累加顺序允许 1e-3
相对容差），然后输出 GFLOPS 与相对加速比。

## 实测参考（Ryzen 7 5800H，WSL2，16 线程）

| 阶段 | 512×512 | 256×256 | 说明 |
|------|---------|---------|------|
| naive | 0.82 GFLOPS | 1.10 GFLOPS | B 按列访问，缓存缺失主导 |
| tiled | 11.5（14x） | 14.8（13x） | 修复内存层次，一个数量级 |
| simd | 17.4（21x） | 18.9（17x） | FMA 宽度兑现，但受微内核访存限制 |
| parallel | 40.0（49x） | 5.7（**反而变慢**） | 小尺寸下线程开销 > 收益 |

两个诚实的教训：

- **SIMD 只有 ~1.2x 于 tiled**：分块修复缓存后，瓶颈转移到微内核本身——
  每个 k 都要整行加载/存回 C，流量压住了 FMA 吞吐。要再翻倍需要寄存器
  分块（一次累加多行），这是教学版与 BLAS 的差距所在。
- **parallel 在 256 时负优化**：创建/同步 16 个线程的成本超过 34 MFLOP
  的工作量。并行化永远要先算"工作量是否盖得住并行开销"。

具体数字随微架构、频率与散热波动，以本机实测为准。

## 诚实说明

- 这是教学级 GEMM：没有寄存器分块（register tiling）、没有对齐分配、
  没有 packing。工业级实现（BLAS）在此基础上还有 3–5 倍空间，但四阶段
  已覆盖 99% 的通用优化手段。
- parallel 阶段用 `schedule(static)` 按行静态划分；行工作量均等时这是
  最优选择，负载不均时应改 dynamic。
