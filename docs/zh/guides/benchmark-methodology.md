# 基准测试方法论

> 性能优化岗位的一半工作是"证明优化有效"。不可信的基准比没有基准更糟——它会给错误方向背书。本文总结本仓库所有基准遵守的测量纪律。

## 测量的敌人清单

| 干扰源 | 现象 | 对策 |
|--------|------|------|
| CPU 频率漂移 | 同样代码两次跑出 20% 差异 | Turbo/温控：取多次最优值而非平均；重要测量锁频（`cpupower frequency-set`，需 root） |
| 缓存冷热 | 首次迭代显著更慢 | 预热（warmup）后再计时；google benchmark 自动丢弃首轮 |
| 页缓存/文件系统 | I/O 基准第一次读磁盘、之后读缓存 | 明确声明测的是哪个层级（本仓库 I/O 示例全部明示"页缓存驻留"） |
| 后台进程/中断 | 偶发尖刺 | 多次重复取最优（min）而非平均——尖刺只会拉高平均 |
| 编译器把基准优化掉 | 耗时 ~0 | `benchmark::DoNotOptimize` + `ClobberMemory`（见下） |
| 并行基准的测量本身 | 计时包含线程创建 | 计时区间只圈住工作本体；小尺寸并行负优化要先怀疑开销 |

## 取最优值（min）而不是平均值

延迟类测量的误差分布是**单边**的：干扰只会让结果变慢，不会变快。因此多次重复的最小值才是对"无干扰执行时间"的无偏估计；平均值系统性偏高。本仓库示例统一用 best-of-N（如 `branch_prediction` 的 best of 10）。吞吐类基准同理：取最快一次。

例外：关心**尾延迟**（P99）的服务场景要报百分位数，min 和平均都没有意义。

## DoNotOptimize 与 ClobberMemory 的原理

`-O3` 会把"结果没人用"的整个循环删除——基准测出 0 ns 是经典事故。两个工具：

- `benchmark::DoNotOptimize(x)`：把 x 放入"编译器必须当作可能被读取"的位置（内联汇编屏障），阻止死代码消除；
- `benchmark::ClobberMemory()`：声明所有内存可能被改写，阻止跨迭代的提升（hoisting）——当循环写内存时必须配合使用，否则编译器可能把多次迭代合并。

注意 google benchmark 1.8+ 对**小的平凡类型传 const 引用**的 `DoNotOptimize` 用法已弃用（会允许不希望的优化），传非 const 局部变量即可——本仓库 `microarch_bench` 踩过并修复了这个坑。

## 对照实验的纪律

1. **单一变量**：A/B 对比时只改一个因素。"标量 vs SIMD"基准里，标量基线必须真的标量——本仓库 `04-simd-vectorization/bench/scalar_baseline.cpp` 单独关闭自动向量化，否则 `-O3 -march=native` 会把"标量"基线悄悄向量化，抹平差异；同理 `08-cpu-microarch` 的内核关闭 if-conversion，防止 GCC 把 if 变 cmov 抹掉误预测效应。
2. **同一二进制、同窗口**：跨机器、跨重启的对比不可信（频率、温控、邻居进程都不同）。虚拟机/WSL2 环境尤其如此——同一终端窗口内顺序跑 A/B 是最低要求。
3. **验证正确性再谈性能**：任何"更快的实现"必须先对照参考实现校验输出。本仓库所有对照示例（GEMM、I/O 读路径、SIMD）都内建校验和/逐元素比对。
4. **报告条件**：CPU 型号、频率状态、编译器与 flags、数据规模。没有这些数字没有意义。

## Google Benchmark 使用要点

```bash
--benchmark_min_time=0.1s   # 每个 case 的最短测量时长（默认太短易抖）
--benchmark_filter="dot"    # 只跑子集
--benchmark_repetitions=5   # 多次重复（配合 --benchmark_report_aggregates_only）
```

- `state.range()` 参数化尺寸 + `SetBytesProcessed`/`SetItemsProcessed` 让输出直接是带宽/吞吐；
- 大对象的构造放在循环外（或每轮 `PauseTiming`/`ResumeTiming`），setup 不计时；
- **集合通信/多进程场景不能用 google benchmark**：它按进程独立决定迭代次数，MPI 集合同步会死锁——本仓库 MPI 基准因此用 `MPI_Wtime` 手动计时。

## 从数字到结论

报告基准结果时给出三层：**绝对值**（GFLOPS / MiB/s / ns/op）、**相对比值**（vs 基线）、**物理解释**（为什么是这个倍数）。没有物理解释的倍数是巧合：`branch_prediction` 的 7.9x 要能归因到"50% 误预测 × ~20 周期代价"，GEMM 的并行 2.3x 要能归因到"微内核访存受限"。能被解释的数字才可复现，可复现才是工程结论。
