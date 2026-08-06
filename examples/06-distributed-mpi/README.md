# 06 - 分布式内存与 MPI

MPI（Message Passing Interface）面向**分布式内存**：每个进程有独立的地址
空间，数据交换必须通过显式消息传递。它与 SIMD（单进程内的数据并行）和
OpenMP（单节点共享内存并行）共同构成 HPC 的三层并行栈。本模块是可选
依赖，默认不构建。

## 构建

MPI 是重量级外部依赖，默认关闭。显式开启：

```bash
cmake --preset=release -DHPC_ENABLE_MPI=ON
cmake --build build/release
```

未安装 MPI 时配置阶段会打印警告并跳过本模块，不影响其他目标。

## 内容

| 示例 | 主题 |
|------|------|
| `src/mpi_basics.cpp` | SPMD 执行模型、rank/size、点对点 Send/Recv 环形传递 |
| `src/collectives.cpp` | 五大集合通信：Bcast / Scatter / Gather / Reduce / Allreduce |
| `src/nonblocking_overlap.cpp` | Isend/Irecv 非阻塞通信与内部计算重叠（halo 交换） |
| `src/jacobi_1d.cpp` | 一维域分解 + halo 交换的 Jacobi 松弛，对串行参考自验证 |
| `bench/mpi_pingpong_bench.cpp` | 点对点延迟/带宽（手动计时） |
| `bench/mpi_allreduce_bench.cpp` | Allreduce 延迟/带宽随消息尺寸变化（手动计时） |

## 运行

```bash
mpirun -np 4 ./build/release/examples/06-distributed-mpi/mpi_basics
mpirun -np 4 ./build/release/examples/06-distributed-mpi/mpi_collectives
mpirun -np 4 ./build/release/examples/06-distributed-mpi/mpi_nonblocking_overlap
mpirun -np 4 ./build/release/examples/06-distributed-mpi/mpi_jacobi_1d
mpirun -np 2 ./build/release/examples/06-distributed-mpi/mpi_pingpong_bench
mpirun -np 4 ./build/release/examples/06-distributed-mpi/mpi_allreduce_bench
```

单进程（`-np 1`）也能运行全部示例，ctest 冒烟测试即用此模式。

## 诚实说明

- **通信基准没有用 Google Benchmark**：benchmark 库按进程独立决定迭代
  次数，而成对/集合通信要求所有 rank 同步进入同一次迭代，两者组合可能
  死锁。因此 `bench/` 下两个基准用 `MPI_Wtime()` 手动计时循环实现。
- **sanitizer 预设默认不覆盖 MPI**：ASan/TSan 与 MPI 运行库混跑容易产生
  第三方误报，本模块默认关闭（`HPC_ENABLE_MPI=OFF`），天然避开该组合。
- ping-pong 延迟与带宽强依赖互联（同节点共享内存 vs 跨节点网卡）与 MPI
  实现的 eager/rendezvous 阈值，请以本机实测为准。

## 延伸阅读

原理深入分析见文档站：[MPI 分布式并行专题](../../docs/zh/deep-dives/mpi-distributed.md)。
