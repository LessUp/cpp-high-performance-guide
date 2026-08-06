# 08 - CPU 微架构

用可复现实验展示三个微架构核心概念：**分支预测**、**乱序执行与 ILP**、
**硬件性能计数器（PMU）**。原理深入分析见文档站：
[CPU 微架构专题](../../docs/zh/deep-dives/cpu-microarch.md)。

## 内容

| 示例 | 基准目标 | 主题 |
|------|----------|------|
| `src/branch_prediction.cpp` | `branch_prediction_bench` | 排序 vs 未排序经典实验 + cmov/掩码无分支化 |
| `src/dependency_chains.cpp` | （暂无基准） | 延迟链 vs 4 路 ILP（加法/乘法），PMU 直读 cycles/op |
| `src/pmu_counters.cpp` | （暂无基准） | perf_event_open 实测 IPC/缓存缺失/分支缺失 |

规范库：[`include/hpc/pmu.hpp`](../../include/hpc/pmu.hpp)
（perf_event_open 的 RAII 封装，PMU 不可用时优雅降级）。

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/08-cpu-microarch/branch_prediction
./build/release/examples/08-cpu-microarch/dependency_chains
./build/release/examples/08-cpu-microarch/pmu_counters
./build/release/examples/08-cpu-microarch/branch_prediction_bench
```

## 实测参考（AMD Ryzen 7 5800H / Zen 3，WSL2）

分支预测实验（1M 字节条件求和）：

| 变体 | 耗时 | 说明 |
|------|------|------|
| 分支，未排序 | 3.64 ms | 50% 随机误预测，每次 ~15-20 周期代价 |
| 分支，已排序 | 0.46 ms（7.9x） | 分支模式可预测，预测器全对 |
| 三目（cmov） | 0.58 ms（6.3x） | 无控制流依赖，与数据顺序无关 |
| 掩码算术 | 0.60 ms（6.0x） | 纯算术，任何优化级别都无分支 |

延迟 vs ILP（PMU 直读周期数）：

| 内核 | cycles/op | IPC |
|------|-----------|-----|
| 依赖加法链 | 3.00（= Zen 3 FP add 延迟） | 0.83 |
| 4 路独立加法 | 0.80 | 2.18 |

## 诚实说明

- 内核放在独立翻译单元 `src/microarch_kernels.cpp`，编译时关闭
  自动向量化**和 if-conversion**：GCC 的 ifcvt pass 会把 `if` 悄悄转成
  cmov，抹掉误预测效应（实测验证过这一行为）。三目版本用
  `__attribute__((optimize("if-conversion")))` 单独恢复 cmov 降级，
  保证"分支 vs 无分支"对比的两端各自真实。
- PMU 需要内核支持：WSL2/虚拟机/`perf_event_paranoid` 过严时会打开失败，
  示例打印说明并正常退出，不影响构建与测试。
