# CPU 微架构

> 本文所有实验均可在本仓库中直接编译运行（`examples/08-cpu-microarch/`），文中标注"本仓库实测"的数字来自 AMD Ryzen 7 5800H（Zen 3）；其他微架构请以本机实测为准。

## 为什么要懂微架构

编译器与 CPU 联手把"你写的代码"变成"实际执行的指令流"，两者都会做激进变换：

- 编译器：向量化、指令重排、if 转 cmov、循环变换；
- CPU：乱序执行、寄存器重命名、分支预测、多级缓存。

性能优化岗位的核心能力，就是能回答"**这段代码的瓶颈在哪一级**"。这需要先知道一条指令从取指到退休要经过什么，以及每一步可能卡在哪里。

## 一条指令的旅程：前端与后端

现代超标量 CPU 分两大段：

**前端（Frontend）**负责供给指令：取指（fetch）→ 解码（decode）→ 送入后端。为缓解取指/解码带宽瓶颈，引入了 **µop 缓存**（Intel DSB / AMD Op Cache）：解码过的微操作直接缓存，命中时绕过解码器。前端常见瓶颈：

- 代码体积膨胀（内联过度、模板爆炸）→ 取指带宽不够、µop 缓存命中率低；
- 分支密集 → 前端流向频繁改变，取指流水线断流。

**后端（Backend）**乱序执行：重命名寄存器消除假依赖 → 进入调度器等待操作数就绪 → 分发到**执行端口**（整数 ALU、地址生成、FP/SIMD 单元各自有限）→ 按程序序退休（retire）。关键容量参数（以 Zen 3 为例）：重排序缓冲（ROB）256 项，意味着乱序窗口约 256 条指令——这就是"缓存 miss 能被乱序执行掩盖多少"的上限。

**诊断对应**：前端受限表现为 IPC 低但缓存/分支指标正常；后端受限则进一步分为内存瓶颈与执行端口瓶颈（见下文 Top-Down）。

## 分支预测

CPU 在分支结果算出来之前就猜测方向继续执行。猜对几乎零代价；**猜错要冲掉整条流水线**，现代核心的误预测代价约 15–20 个周期。

### 本仓库实测

`examples/08-cpu-microarch/branch_prediction` 复现经典实验：对 1M 个 0–255 随机数做条件求和（阈值 128），Zen 3 实测：

| 变体 | 耗时 | 解释 |
|------|------|------|
| 分支 + 未排序 | 3.64 ms | 数据随机 → 分支方向随机 → 约 50% 误预测 |
| 分支 + 已排序 | 0.46 ms | 前段全不跳、后段全跳，预测器全对 |
| 三目（cmov） | 0.58 ms | 无条件移动，无分支可预测错 |
| 掩码算术 | 0.60 ms | 纯算术构造，任何情况下无分支 |

由 (3.64 − 0.46) ms ÷ 约 52 万次误预测 ≈ 每次误预测 ~6 ns，折合 ~15–20 周期，与文献值一致。

### 工程对策

1. **无分支化**：三目运算符（编译器降为 cmov）、位掩码、`std::min/max`；SIMD 场景用 min/max/blend 指令（见 [SIMD 向量化](simd-internals.md) 的"分支 → 无分支"一节）；
2. **让分支可预测**：数据排序、把热路径分支变成单向（`__builtin_expect`/`[[likely]]` 提示编译器布局）；
3. **算法消除**：分区（partition）代替逐元素判断；
4. **测量确认**：`perf stat -e branch-misses,branches`，误预测率 > 5% 才值得动手（见 [性能分析实战](../guides/profiling.md)）。

> 诚实提醒：`-O3` 下编译器可能**替你把 if 变成 cmov**（GCC 的 if-conversion pass）。本模块的内核翻译单元显式关闭该 pass 才复现出误预测效应——分析"分支为什么慢"时，先看汇编里到底是 `jcc` 还是 `cmov`。

## 延迟、吞吐与 ILP

三个不同的概念，面试与调优都高频出现：

- **延迟（latency）**：一条指令从发射到结果可用的周期数；
- **吞吐（throughput）**：单位时间能完成多少条（受执行端口数限制）；
- **ILP（指令级并行）**：相互**无依赖**的指令可被乱序后端重叠执行。

依赖链的步进速度受延迟限制；独立链可以被后端叠起来，逼近吞吐。`examples/08-cpu-microarch/dependency_chains` 用 PMU 直读周期数（Zen 3 实测）：

| 内核 | cycles/op | IPC |
|------|-----------|-----|
| 依赖加法链 | 3.00（= FP add 延迟） | 0.83 |
| 4 路独立加法 | 0.80 | 2.18 |
| 依赖乘法链 | 3.01（= FP mul 延迟） | 0.83 |
| 4 路独立乘法 | 0.80 | 2.19 |

结论：**同样的指令数，仅因依赖结构不同，速度差 3.75 倍**。这解释了为什么"展开 + 多累加器"能加速归约（本仓库 `dot_product_wrapped` 目前用单累加器，正是延迟受限形态——这是留给读者的练习）。

常用数量级（现代核心，以 uops.info / Agner Fog 表为准）：整数 add 1 周期延迟；FP add/mul 3–4 周期延迟；FP div 10+ 且常不完全流水；L1 load-to-use 4–5 周期。

## IPC 与 Top-Down 方法论

IPC（每周期指令数）是微架构效率的总指标，但"IPC 低"本身不指导行动。Intel 提出的 **Top-Down Microarchitecture Analysis** 把流水线槽位按去向四分类，AMD 上有对应方法：

| 类别 | 含义 | 典型症状 | 对策方向 |
|------|------|----------|----------|
| Frontend Bound | 前端供不上指令 | 代码过大、µop 缓存 miss 高 | 减小热路径体积、调整内联策略 |
| Bad Speculation | 槽位浪费在猜错上 | branch-misses 高 | 无分支化、数据布局（本文上一节） |
| Backend Bound – Memory | 等数据 | cache-misses/LLC-misses 高 | 布局/tiling/预取（见 [内存布局](memory-layout.md)） |
| Backend Bound – Core | 执行端口/依赖链饱和 | IPC 接近端口上限 | 换指令、提 ILP、算法变更 |
| Retiring | 有效工作 | 越高越好 | 优化算法本身 |

仓库工具链：`hpc/pmu.hpp`（进程内计数器，见 `examples/08-cpu-microarch/src/pmu_counters.cpp`）与 `perf stat -e cycles,instructions,cache-misses,branch-misses`（见 [性能分析实战](../guides/profiling.md)）。实测例子：同一加法内核，标量版 IPC 0.80 / 14.7M 指令，SIMD 分发版 6.3M 指令——指令数降到 43%，而两者都受内存带宽限制（96 MiB 工作集），说明此时瓶颈已不在指令吞吐。

## 与仓库其他主题的映射

| 微架构概念 | 对应模块 |
|------------|----------|
| 缓存层次与局部性 | 02-memory-cache + [内存布局专题](memory-layout.md) |
| SIMD 宽度与掩码 | 04-simd-vectorization + [SIMD 专题](simd-internals.md) |
| 伪共享（缓存一致性协议） | `examples/02-memory-cache/src/false_sharing.cpp` |
| 分支预测 | 08-cpu-microarch（本文） |
| ILP 与依赖链 | 08-cpu-microarch（本文） |
| PMU 测量 | `include/hpc/pmu.hpp` |

## 参考资料

- Agner Fog, *Optimization manuals*（指令延迟/吞吐表的经典来源）, https://www.agner.org/optimize/
- uops.info——按微架构查询指令的延迟/吞吐/端口（本文数字的核对处）。
- Yasin, "A Top-Down Method for Performance Analysis and Counters Architecture", ISPASS 2014.
- Chips & Cheese 的微架构深潜文章（Zen/Intel 各代的 ROB/端口/预测器细节）。
- Intel *64 and IA-32 Architectures Optimization Reference Manual*。
