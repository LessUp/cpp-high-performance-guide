# MPI 分布式并行

> 本文所有示例均可在本仓库中直接编译运行（需开启可选的 MPI 支持）。分布式内存是 HPC 并行栈的第三层：SIMD 解决单核内的数据并行，OpenMP 解决单节点共享内存并行，MPI 解决跨节点的消息传递。

## 为什么需要消息传递

共享内存并行的前提是"所有线程能访问同一地址空间"，这在单机上成立，跨节点则不成立——每台机器有独立的物理内存。要继续扩展，只有两条路：

1. **显式消息传递**：进程之间通过网络收发数据块（MPI 走这条路）；
2. **PGAS 模型**：分区全局地址空间（如 UPC），实践中远不如 MPI 普及。

MPI（Message Passing Interface）不是库而是**标准**，主流实现有 Open MPI、MPICH、Intel MPI、Cray MPI 等。本仓库示例在 Open MPI 4.x 上验证，代码只依赖 MPI-1/2 核心接口，任何实现都能编译。

## SPMD 执行模型

MPI 程序是**单程序多数据**（SPMD）：同一个二进制在每个 rank 上启动，进程靠 `MPI_Comm_rank()` 区分身份：

```cpp
int rank = 0, size = 0;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // 我是谁
MPI_Comm_size(MPI_COMM_WORLD, &size);  // 总共几个进程
```

没有主从进程之分（除非你自己设计成主从），所有 rank 执行同一份代码，靠 `rank` 走不同分支。这与 OpenMP 的 fork-join 不同：MPI 进程的生命周期贯穿整个程序，`MPI_Init` 与 `MPI_Finalize` 之间没有隐式同步点。

仓库示例：`examples/06-distributed-mpi/src/mpi_basics.cpp`——环形传 payload 验证点对点通信，`MPI_Allreduce` 汇总各 rank 的失败计数，是分布式断言的常用模式。

## 点对点通信与死锁

`MPI_Send` / `MPI_Recv` 是最小原语。两个 rank 互相收发时，发送顺序不当就会死锁：

```cpp
// 错误：两个 rank 都先 Send，若消息超过 eager 缓冲区则互相等待
MPI_Send(buf, n, MPI_DOUBLE, peer, 0, MPI_COMM_WORLD);
MPI_Recv(buf, n, MPI_DOUBLE, peer, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
```

经典修法有三种：按 rank 奇偶错开顺序、Send 端与 Recv 端颠倒次序，或者直接用 `MPI_Sendrecv`——它在语义上保证"发送与接收作为一个整体推进"，是成对数据交换的首选：

```cpp
MPI_Sendrecv(sendbuf, n, MPI_DOUBLE, dst, tag,
             recvbuf, n, MPI_DOUBLE, src, tag,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
```

**tag 必须两端匹配**。仓库的 jacobi 示例最初版本给"左交换"和"右交换"分配了不同 tag，而邻居 rank 的对应端用的是另一个 tag——消息永远配不上对，np=4 时直接死锁。修复后每条边两端共用同一 tag，见 `examples/06-distributed-mpi/src/jacobi_1d.cpp`。这类 bug 在 np=1/2 下可能不暴露，多进程冒烟测试（ctest 的 `mpi` 标签）就是为了抓它。

## 五大集合通信

集合通信（collective）要求 communicator 里**所有 rank 同时参与**，MPI 实现内部用拓扑感知的树形/蝶形算法，手写点对点版本几乎不可能更快：

| 接口 | 语义 | 典型场景 |
|------|------|----------|
| `MPI_Bcast` | 根节点广播给所有人 | 分发配置、初始条件 |
| `MPI_Scatter` / `MPI_Gather` | 根节点切片分发 / 收集 | 域分解的输入输出 |
| `MPI_Reduce` | 逐元素归约到根节点 | 全局残差、最大值 |
| `MPI_Allreduce` | 归约结果发到**所有** rank | 并行点积、训练梯度同步 |
| `MPI_Gatherv` / `MPI_Scatterv` | 不等长版本 | 负载不均的分解 |

`MPI_Allreduce` 是大规模数值计算和分布式训练里出现频率最高的集合操作，值得单独测量（见下文基准）。

仓库示例 `examples/06-distributed-mpi/src/collectives.cpp` 对五个接口逐一做正确性校验：Scatter/Gather 用"切片平方再收集"验证顺序未被扰动，Allreduce 用分段点积对照解析解。

## 非阻塞通信与计算重叠

阻塞通信让 CPU 等消息，非阻塞通信（`MPI_Isend` / `MPI_Irecv`）立即返回一个 `MPI_Request`，把通信交给 NIC/进展引擎，CPU 继续算自己已有的数据：

```cpp
MPI_Irecv(halo_left,  1, MPI_DOUBLE, left,  0, comm, &reqs[0]);
MPI_Irecv(halo_right, 1, MPI_DOUBLE, right, 0, comm, &reqs[1]);
MPI_Isend(my_left_boundary,  1, MPI_DOUBLE, left,  0, comm, &reqs[2]);
MPI_Isend(my_right_boundary, 1, MPI_DOUBLE, right, 0, comm, &reqs[3]);

compute_interior();  // 不依赖 halo 的计算，与传输重叠

MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);  // 读 halo 前必须等完
```

要点：**先挂接收再发送**（保证对方发来的数据有地方落），**读 halo 前必须 Wait**。这是 stencil 类代码的标准骨架，见 `examples/06-distributed-mpi/src/nonblocking_overlap.cpp`。

## 域分解实例：一维 Jacobi 松弛

`examples/06-distributed-mpi/src/jacobi_1d.cpp` 把三件事组合成完整的分布式数值程序：

1. **连续块分解**：全局 N 个内点按 rank 均分，余数分给前几个 rank（`block_size` / `block_offset`）；
2. **halo 交换**：每个 rank 的局部数组两端各留一个 ghost 单元，每次迭代前用 `MPI_Sendrecv` 刷新；
3. **自验证**：并行跑完后 `MPI_Gatherv` 收集全局结果，rank 0 用同一 stencil 串行重算并比对——本仓库实测两者逐位一致（max diff 0.0）。

"对串行参考自验证"是分布式程序最重要的正确性手段：分解方式（np=1/2/4）不应该改变数学结果。

## 通信基准与诚实测量

两个基准刻意**不使用 Google Benchmark**：benchmark 库按进程独立决定迭代次数，而成对/集合通信要求所有 rank 同步进入同一次通信——迭代次数不一致就会死锁。因此改用 `MPI_Wtime()` 手动计时循环：

- `examples/06-distributed-mpi/bench/mpi_pingpong_bench.cpp`：rank 0/1 乒乓，测单向延迟与带宽；
- `examples/06-distributed-mpi/bench/mpi_allreduce_bench.cpp`：Allreduce 随消息尺寸的延迟/带宽。

本仓库（单节点、Open MPI 共享内存路径）实测参考：

| 消息尺寸 | ping-pong 延迟 | 说明 |
|----------|----------------|------|
| ≤ 1 KiB | ~0.2–0.35 µs | eager 协议，延迟主导 |
| 4 KiB 附近 | 延迟跳变 | eager → rendezvous 切换点 |
| 1 MiB | ~7 GiB/s | 带宽主导，受拷贝路径限制 |

跨节点时延迟会升到 µs–ms 量级（取决于网卡与互连），请以本机实测为准。

## MPI vs OpenMP：怎么选

| 维度 | MPI | OpenMP |
|------|-----|--------|
| 内存模型 | 分布式，显式消息 | 共享，隐式 |
| 扩展范围 | 跨节点，理论上无上限 | 单节点（线程数上限） |
| 编程成本 | 高：分解 + 通信 + 同步 | 低：pragma |
| 典型混合用法 | 节点间 MPI，节点内 OpenMP/线程 | 同左 |

实践中大规模程序几乎都是**混合并行**：每节点起 1–2 个 MPI rank，rank 内用 OpenMP 或线程池，通信次数随 rank 数平方增长的代价因此可控。

## 构建与运行

MPI 是可选依赖（默认关闭）：

```bash
cmake --preset=release -DHPC_ENABLE_MPI=ON
cmake --build build/release

mpirun -np 4 ./build/release/examples/06-distributed-mpi/mpi_jacobi_1d
ctest -L mpi  # 冒烟测试（np=1 全路径 + np=4 多进程）
```

sanitizer 预设默认不覆盖 MPI：ASan/TSan 与 MPI 运行库混跑容易在第三方代码里误报， opt-in 设计天然避开该组合。
