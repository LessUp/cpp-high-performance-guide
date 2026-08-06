# 快速开始

## 环境要求

| 工具 | 最低版本 | 说明 |
|------|---------|------|
| GCC | 11+ | 或 Clang 14+，需支持 C++20 |
| CMake | 3.20+ | 使用 CMake Presets (v6) |
| Git | 2.x | 克隆仓库 |
| Ninja | 1.10+ | 默认生成器 |

可选工具（性能分析专题使用）：

- `perf` — Linux 性能计数器采样
- Valgrind (`valgrind`) — 内存错误检测
- Intel VTune — 微架构级分析

## 构建与验证

```bash
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

cmake --preset=release
cmake --build build/release
ctest --preset=release
```

全部测试通过即表示环境就绪。

## 构建预设

| 预设 | 用途 | 关键标志 |
|------|------|---------|
| `release` | 优化构建，运行基准测试 | `-O2`/`-O3`，Ninja |
| `debug` | 开发调试 | `-O0 -g` |
| `relwithdebinfo` | 带符号的优化构建，用于 perf 采样 | `-O2 -g` |
| `asan` | 地址错误检测 | `-fsanitize=address` |
| `tsan` | 数据竞争检测（需 Clang） | `-fsanitize=thread` |
| `ubsan` | 未定义行为检测 | `-fsanitize=undefined` |
| `coverage` | 代码覆盖率 | `-fprofile-arcs -ftest-coverage` |

切换预设只需替换 `--preset` 参数：

```bash
cmake --preset=asan
cmake --build build/asan
ctest --preset=asan
```

## 运行基准测试

Release 构建包含所有基准测试可执行文件：

```bash
./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

输出类似：

```
-----------------------------------------------------------
Benchmark                 Time             CPU   Iterations
-----------------------------------------------------------
BM_AoS_Sum/1024        ... ns          ... ns        ...
BM_SoA_Sum/1024        ... ns          ... ns        ...
```

具体数值取决于硬件。关注同一基准内不同实现的相对比值，而非绝对数字。

## 仓库结构

```
examples/
  01-cmake-modern/       # CMake 现代实践
  02-memory-cache/       # 内存布局与缓存友好访问
  03-modern-cpp/         # C++20 特性与性能
  04-simd-vectorization/ # SIMD 手动向量化
  05-concurrency/        # 无锁并发、OpenMP、线程亲和与 NUMA
  06-distributed-mpi/    # 分布式内存与 MPI（可选，-DHPC_ENABLE_MPI=ON）
  07-io-performance/     # 文件 I/O：read/pread/mmap 与写缓冲（Linux）
  08-cpu-microarch/      # 分支预测、ILP/依赖链、PMU 计数器
  09-matrix-multiply/    # GEMM 四阶段优化案例（tiling/SIMD/OpenMP）
include/hpc/             # 规范头文件库 (core, simd, memory, concurrency, io, modern-cpp 等)
tests/
  unit/                  # 单元测试 (Google Test)
  property/              # 属性测试
benchmarks/common/       # 基准测试公共工具
docs/                    # VitePress 文档站
scripts/                 # 辅助脚本 (format, setup)
```

## 下一步

- [内存布局与缓存](/zh/deep-dives/memory-layout) — AOS vs SOA、预取、对齐、false sharing
- [CPU 微架构](/zh/deep-dives/cpu-microarch) — 分支预测、延迟 vs 吞吐、Top-Down 方法论
- [GEMM 案例研究](/zh/deep-dives/gemm-case-study) — naive→tiling→SIMD→OpenMP 四步优化
- [基准测试方法论](/zh/guides/benchmark-methodology) — 可信测量的纪律与陷阱
- [SIMD 向量化](/zh/deep-dives/simd-internals) — 手动 intrinsics、NEON 与编译器自动向量化对比
- [MPI 分布式并行](/zh/deep-dives/mpi-distributed) — 点对点、集合通信、域分解与 halo 交换
- [无锁并发](/zh/deep-dives/lock-free-queue) — lock-free queue 实现与基准
- [线程亲和与 NUMA](/zh/deep-dives/thread-affinity-numa) — 绑核与节点本地分配
- [I/O 性能](/zh/deep-dives/io-performance) — read/pread/mmap 对照与写缓冲开销
- [性能分析实战](/zh/guides/profiling) — perf + FlameGraph 实战
- [故障排查](/zh/reference/troubleshooting) — 常见构建与运行问题
