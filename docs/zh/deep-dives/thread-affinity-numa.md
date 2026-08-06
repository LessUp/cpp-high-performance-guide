# 线程亲和与 NUMA

> 本文示例在 `examples/05-concurrency/`（Linux 专属，可选依赖）。线程数变多之后，"线程跑在哪个核、数据分配在哪个内存节点"从无关紧要变成一阶变量。

## 为什么需要绑核

操作系统调度器会在核间迁移线程。迁移的代价有两层：

1. **缓存失效**：线程搬走后，原核 L1/L2 里积累的工作集作废，新核要重新预热；
2. **NUMA 错位**：线程迁移到另一个 socket 后，它访问的仍是旧节点的内存，每次访问都走节点间互连。

对延迟敏感的角色——网络收包循环、自旋等待的消费者、按核分片的 worker——通常显式绑定 CPU 亲和，把自己钉死在一个核上。

`examples/05-concurrency/src/thread_affinity.cpp` 演示完整流程：`pthread_setaffinity_np` 设置掩码 → `pthread_getaffinity_np` 回读验证掩码确实生效 → `sched_getcpu()` 确认线程真的跑在目标核上。三重校验是因为"设置成功"不等于"生效"（cgroup、cpuset 都可能覆盖你的掩码）。

```cpp
cpu_set_t mask;
CPU_ZERO(&mask);
CPU_SET(target_cpu, &mask);
pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
```

## NUMA：内存也有"位置"

多 socket 机器的每个 CPU 插槽有自己的本地内存控制器。访问本地内存是标称延迟，访问挂在别的节点上的内存要跨互连（UPI/Infinity Fabric），延迟通常贵 1.5–2 倍。`numactl -H` 可以查看拓扑。

libnuma 提供两件事：**发现拓扑**和**控制分配位置**：

```cpp
numa_available();                 // 内核支持检查，<0 表示不可用
numa_num_configured_nodes();      // 节点数
numa_node_of_cpu(sched_getcpu()); // 当前 CPU 属于哪个节点
numa_alloc_onnode(bytes, node);   // 在指定节点分配
numa_alloc_interleaved(bytes);    // 页粒度交错分布到所有节点
```

`examples/05-concurrency/src/numa_binding.cpp` 打印拓扑后对照"节点本地分配 vs 交错分配"的流式写带宽。注意一个诚实细节：本机只有 1 个 NUMA 节点时两者带宽相同，这是预期结果而不是 bug——单节点机器上"NUMA 优化"没有可优化的对象。

## 完整配方：首触策略 + 绑定

生产环境的 NUMA 配方通常是三步：

1. **绑线程**：worker i 绑到节点 N 的 CPU 集合；
2. **绑内存**：该 worker 的内存用 `numa_alloc_onnode(N)`（或进程级 `numactl --membind=N`）；
3. **首触策略**（first-touch）：页在第一次写时才真正分配物理帧，谁首触就落在谁的节点上。并行 `memset`/初始化必须由"将来使用它的线程"执行，否则页会落在初始化线程的节点上，绑定就白做了。

这就是"亲和 + 分配 + 首触"必须三件套一起做的原因。

## 构建与依赖说明

- `thread_affinity` 仅依赖 pthread（Linux 门控）；
- `numa_binding` 需要 libnuma，且 configure 期做**真实链接探测**：发行版 libnuma 可能引用比 sysroot 工具链更新的 glibc 符号（例如 conda gcc 链接系统 libnuma 缺 `GLIBC_2.38` 符号），探测失败则跳过示例而不是让构建失败；
- 两个示例都不注册基准：亲和的收益体现在具体负载（队列吞吐、尾延迟）上，空循环基准测不出意义。
