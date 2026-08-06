# 05 - 并发与多线程

原子操作内存序、无锁队列与 OpenMP。
原理深入分析见文档站：[无锁并发深度专题](../../docs/zh/deep-dives/lock-free-queue.md)。

## 内容

| 示例 | 基准目标 | 主题 |
|------|----------|------|
| `src/atomic_ordering.cpp` | `atomic_ordering_bench` | memory order 语义与成本 |
| `src/lock_free_queue.cpp` | （暂无基准） | SPSC/MPMC 无锁队列正确性演示 |
| `src/openmp_basics.cpp` | `openmp_basics_bench` | OpenMP 并行模式 |
| `src/thread_affinity.cpp` | （暂无基准） | pthread_setaffinity_np 线程绑核与掩码校验（Linux） |
| `src/numa_binding.cpp` | （暂无基准） | libnuma 拓扑发现与节点本地分配（可选依赖） |

队列与并发工具来自规范库 `include/hpc/`
（`hpc::concurrency::SPSCQueue` / `MPMCQueue` / `SpinLock`）。

`numa_binding` 为可选目标：需要 libnuma 且能通过 configure 期链接探测
（conda 等 sysroot 工具链链接系统 libnuma 会缺 GLIBC 符号，此时自动跳过）。
单节点机器上两种分配带宽相同，属于预期结果。

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/05-concurrency/atomic_ordering_bench
./build/release/examples/05-concurrency/openmp_basics_bench
```

## 实测参考与诚实说明

- **SeqCst vs Relaxed fetch_add 在 x86 上基本等价**：两者编译为同一条
  `lock xadd`，本基准的差异主要体现在 ARM/POWER 等弱内存序平台
  （`bench/atomic_bench.cpp` 头部有详细说明）。
- **无锁队列 vs 互斥锁队列**：本模块目前只有正确性演示，尚无吞吐对比基准，
  因此这里不给"Nx 加速"之类的宣称——本仓库的原则是每个倍数都要有可运行的
  bench 背书。并发正确性用 TSan 验证：
  `cmake --preset=tsan && cmake --build --preset tsan && ctest --preset=tsan`。
- **OpenMP 并行 for**：计算密集负载接近线性加速，受 Amdahl 定律、内存带宽
  与伪共享限制。

以本机实测为准。多线程性能剖析见 [Profiling 指南](../../docs/zh/guides/profiling.md)。
