# 性能分析实战

> 没有测量数据的优化是猜测。本页覆盖从 perf 到 Sanitizer 的完整工具链，所有命令均可在本仓库直接运行。

## 优化工作流

性能优化是一个迭代循环，而非一次性操作：

```
测量 → 分析 → 优化 → 验证 → 测量 → ...
```

| 阶段 | 做什么 | 工具 |
|------|--------|------|
| **测量** | 跑基准测试，拿到基线数据 | Google Benchmark、`perf stat` |
| **分析** | 定位热点函数和瓶颈类型 | `perf record`、FlameGraph、VTune |
| **优化** | 针对瓶颈做最小改动 | 编译器选项、算法变更、内存布局调整 |
| **验证** | 确认改进有效且无回归 | `benchmark_compare.py`、Sanitizer |

关键原则：

- **每次只改一个变量**。同时改算法和内存布局，无法归因。
- **先跑 Sanitizer 再跑 profiler**。未定义行为会让性能数据失去意义。
- **用 `relwithdebinfo` 做 profiling**。`release` 没有调试符号，`debug` 没有优化。

```bash
cmake --preset=relwithdebinfo
cmake --build build/relwithdebinfo
```

---

## perf 实战

`perf` 是 Linux 下最核心的性能分析工具。以下示例使用本仓库的实际 benchmark 路径。

### 前置准备

```bash
# 安装 perf（Debian/Ubuntu）
sudo apt install linux-tools-common linux-tools-generic linux-tools-$(uname -r)

# 允许非 root 用户使用 perf（临时）
sudo sysctl -w kernel.perf_event_paranoid=-1

# 构建 release 版本
cmake --preset=release
cmake --build build/release
```

### perf stat：快速概览

`perf stat` 给出硬件计数器的全局摘要，是分析的第一步：

```bash
perf stat ./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

关注输出中的 IPC（insn per cycle）、cache-misses 占比和 branch-misses 占比：

| 指标 | 健康范围 | 需要关注 |
|------|----------|----------|
| IPC（insn per cycle） | > 1.0 | < 0.5 说明 CPU 经常在等待 |
| cache-misses 占比 | < 5% | > 15% 说明内存访问模式有问题 |
| branch-misses 占比 | < 2% | > 5% 说明分支预测失败频繁 |

指定特定事件，或用 `--benchmark_filter` 对比同一 benchmark 的不同实现：

```bash
# 指定事件
perf stat -e cache-misses,cache-references,branch-misses,branches,cycles,instructions \
  ./build/release/examples/02-memory-cache/aos_vs_soa_bench

# 对比 AOS vs SOA 的缓存行为
perf stat -e cache-misses ./build/release/examples/02-memory-cache/aos_vs_soa_bench \
  --benchmark_filter="AOS"
perf stat -e cache-misses ./build/release/examples/02-memory-cache/aos_vs_soa_bench \
  --benchmark_filter="SOA"
```

### perf record + report：定位热点函数

```bash
# 录制（-g 记录调用栈，--call-graph dwarf 兼容性更好）
perf record -g --call-graph dwarf \
  ./build/release/examples/02-memory-cache/prefetch_bench

# 查看报告（交互式 TUI）
perf report

# 非交互式，按开销排序
perf report --stdio --sort=dso,symbol --percent-limit=1 | head -40
```

TUI 中按 `Enter` 展开调用栈，按 `a` 进入 annotate 视图，按 `/` 搜索函数名。

### perf annotate：查看热点指令

定位到热点函数后，用 annotate 看具体哪条指令最热：

```bash
perf annotate --stdio -s compute_soa
```

每行指令前的百分比是采样占比。重点关注：

- 高占比的 `mov` / `load` → 可能是 cache miss
- 高占比的 `cmp` / `jmp` → 可能是 branch miss
- 循环体中没有 `vmovaps` / `vfmadd` → 可能没有向量化

### 关键事件速查

| 事件 | 含义 | 典型用法 |
|------|------|----------|
| `cycles` | CPU 时钟周期 | 总体耗时基准 |
| `instructions` | 退休指令数 | 与 cycles 配合算 IPC |
| `cache-misses` | 缓存未命中 | 内存访问模式分析 |
| `cache-references` | 缓存访问总数 | 计算 miss 率 |
| `branch-misses` | 分支预测失败 | 分支密集型代码分析 |
| `LLC-load-misses` | 末级缓存加载未命中 | 内存带宽瓶颈 |
| `dTLB-load-misses` | 数据 TLB 未命中 | 大工作集 / 随机访问 |

组合使用：

```bash
# 内存瓶颈全面诊断
perf stat -e cache-misses,cache-references,LLC-load-misses,LLC-loads,dTLB-load-misses \
  ./build/release/examples/02-memory-cache/alignment_bench

# 分支预测诊断
perf stat -e branch-misses,branches \
  ./build/release/examples/03-modern-cpp/ranges_vs_loops_bench
```

---

## FlameGraph

火焰图将调用栈的采样频率可视化为宽度——越宽的函数占用 CPU 时间越多。

### 使用仓库脚本

本仓库提供了 `tools/performance/generate_flamegraph.sh`，封装了 perf → FlameGraph 的完整流水线。

**前置依赖：**

```bash
# 安装 Brendan Gregg 的 FlameGraph 脚本
git clone https://github.com/brendangregg/FlameGraph.git ~/FlameGraph
# 如果安装在其他位置：export FLAMEGRAPH_DIR=/path/to/FlameGraph
```

**基本用法：**

```bash
# 对 benchmark 生成火焰图
./tools/performance/generate_flamegraph.sh \
  ./build/release/examples/02-memory-cache/aos_vs_soa_bench

# 指定输出文件名
./tools/performance/generate_flamegraph.sh \
  ./build/release/examples/04-simd-vectorization/simd_bench simd_flamegraph.svg

# 限时录制 + 提高采样频率
./tools/performance/generate_flamegraph.sh \
  ./build/release/examples/05-concurrency/openmp_basics_bench \
  openmp.svg --duration 30 --frequency 997
```

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `--duration <秒>` | 运行到结束 | 录制时长 |
| `--frequency <Hz>` | 99 | 采样频率 |
| `--title <标题>` | 自动生成 | 火焰图标题 |
| `--width <像素>` | 1200 | SVG 宽度 |
| `--colors <方案>` | hot | 配色：`hot`、`mem`、`io` 等 |

### 如何读懂火焰图

- **X 轴**：采样频率（宽度 = 占比），**不是**时间顺序
- **Y 轴**：调用栈深度，底部是 `main`，顶部是叶子函数
- **颜色**：随机分配，没有语义
- **找什么**：顶部最宽的"平台"就是最大的热点

| 看到的形状 | 含义 |
|-----------|------|
| 顶部一个很宽的单一函数 | 计算密集型热点，考虑向量化或算法优化 |
| 很多窄条分散在不同函数 | 没有明显热点，可能是 I/O 或锁等待 |
| `malloc` / `free` 很宽 | 内存分配开销大，考虑对象池或预分配 |
| `memcpy` / `memmove` 很宽 | 数据搬运开销大，考虑减少拷贝 |

---

## Valgrind

Valgrind 在模拟 CPU 上运行程序，速度慢（10-50x），但能提供精确到指令级的缓存和调用分析。适合对关键路径做深入分析，不适合跑完整 benchmark。

### Cachegrind：缓存模拟

```bash
cmake --preset=debug && cmake --build build/debug

valgrind --tool=cachegrind \
  ./build/debug/examples/02-memory-cache/aos_vs_soa_bench \
  --benchmark_filter="AOS" --benchmark_min_time=0.1

cg_annotate cachegrind.out.<pid>       # 命令行查看
kcachegrind cachegrind.out.<pid>       # 图形化查看
```

关注 `D1mr`（L1 数据缓存读 miss）和 `DLmr`（末级缓存读 miss）。

### Callgrind：调用图分析

```bash
valgrind --tool=callgrind \
  ./build/debug/examples/03-modern-cpp/move_semantics_bench \
  --benchmark_min_time=0.1

callgrind_annotate callgrind.out.<pid>
```

Callgrind 给出每个函数的精确调用次数和指令数，不受采样误差影响。

---

## VTune

Intel VTune Profiler 提供比 perf 更细粒度的微架构分析。通过 Intel oneAPI 安装后，加载环境：

```bash
source /opt/intel/oneapi/vtune/latest/env/vars.sh
```

三种常用分析：

```bash
# 热点分析
vtune -collect hotspots -result-dir vtune_hotspots \
  ./build/release/examples/02-memory-cache/aos_vs_soa_bench
vtune -report hotspots -result-dir vtune_hotspots

# 内存访问分析
vtune -collect memory-access -result-dir vtune_memory \
  ./build/release/examples/02-memory-cache/prefetch_bench

# 微架构分析（最精确的瓶颈定位）
vtune -collect uarch-exploration -result-dir vtune_uarch \
  ./build/release/examples/04-simd-vectorization/simd_bench
```

`uarch-exploration` 给出前端停顿（frontend bound）、后端停顿（backend bound）、坏推测（bad speculation）和退休（retiring）的比例，是判断优化方向最精确的工具。

---

## Sanitizer 验证

优化之后必须验证正确性。本仓库通过 CMake preset 集成了三种 Sanitizer。

> **顺序很重要**：先跑 Sanitizer 确认无未定义行为，再做性能分析。UB 会让性能数据不可信。

### AddressSanitizer（ASan）

检测：堆/栈/全局缓冲区溢出、use-after-free、use-after-return、内存泄漏。

```bash
cmake --preset=asan
cmake --build build/asan
ctest --preset=asan
```

preset 已配置 `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`，遇到第一个错误即停止。

### ThreadSanitizer（TSan）

检测：数据竞争（data race）。

```bash
cmake --preset=tsan
cmake --build build/tsan
ctest --preset=tsan
```

preset 强制使用 Clang（`CC=clang CXX=clang++`），并配置 `TSAN_OPTIONS=halt_on_error=1`。TSan 和 ASan 不能同时使用。

### UndefinedBehaviorSanitizer（UBSan）

检测：有符号整数溢出、空指针解引用、对齐违规、类型不匹配等。

```bash
cmake --preset=ubsan
cmake --build build/ubsan
ctest --preset=ubsan
```

preset 配置 `UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1`。

### 建议工作流

```
1. debug 构建 + ctest          → 基本功能正确
2. asan 构建 + ctest           → 内存安全
3. tsan 构建 + ctest           → 并发安全（多线程代码）
4. ubsan 构建 + ctest          → 无未定义行为
5. release 构建 + benchmark    → 性能基线
6. 优化
7. 重复 1-5                    → 验证优化没有引入问题
```

每次提交前至少跑 debug + 一个 sanitizer。CI 应覆盖全部。

---

## 基准测试最佳实践

本仓库的 benchmark 基于 [Google Benchmark](https://github.com/google/benchmark)。

### DoNotOptimize 和 ClobberMemory

编译器可能把没有副作用的计算完全消除。Google Benchmark 提供两个工具防止这种情况：

```cpp
#include <benchmark/benchmark.h>

static void BM_Compute(benchmark::State& state) {
    std::vector<float> data(1024, 1.0f);
    for (auto _ : state) {
        float result = compute(data);
        benchmark::DoNotOptimize(result);   // 防止编译器消除计算
        benchmark::ClobberMemory();         // 防止编译器缓存内存写入
    }
}
```

- `DoNotOptimize(value)`：告诉编译器 `value` 被"使用"了，不能消除产生它的计算
- `ClobberMemory()`：告诉编译器所有内存都可能被读取，不能缓存之前的写入

本仓库的所有 benchmark（`examples/*/bench/`）都使用了这两个函数，可作为参考。

### 预热与环境控制

Google Benchmark 自动处理预热迭代。如果涉及文件系统或网络，在循环外手动调用一次即可。

性能测试对环境噪声敏感，减少干扰：

```bash
# 固定 CPU 频率（需要 root）
sudo cpupower frequency-set -g performance

# 绑定到特定 CPU 核心 + 禁用 ASLR
taskset -c 2 setarch $(uname -m) -R \
  ./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

### 统计显著性与对比

单次运行不可靠。使用重复和 JSON 输出：

```bash
# 运行 10 次，输出 JSON
./build/release/examples/02-memory-cache/aos_vs_soa_bench \
  --benchmark_repetitions=10 --benchmark_format=json > result.json
```

标准差超过均值的 5% 时结果不可信——检查环境干扰或增加重复次数。

使用仓库提供的 `tools/performance/benchmark_compare.py` 对比两次运行：

```bash
# 优化前 / 优化后分别保存 JSON
./build/release/examples/02-memory-cache/aos_vs_soa_bench \
  --benchmark_format=json > baseline.json
# ... 做优化 ...
./build/release/examples/02-memory-cache/aos_vs_soa_bench \
  --benchmark_format=json > current.json

# 对比（默认阈值 10%）
python tools/performance/benchmark_compare.py baseline.json current.json

# 自定义阈值 + Markdown 报告
python tools/performance/benchmark_compare.py baseline.json current.json \
  --threshold 0.05 --report comparison.md

# CI 中使用：有回归则返回非零退出码
python tools/performance/benchmark_compare.py baseline.json current.json \
  --fail-on-regression --quiet
```

---

## 速查表

| 任务 | 工具 | 命令 |
|------|------|------|
| 快速看全局性能指标 | perf stat | `perf stat ./build/release/examples/02-memory-cache/aos_vs_soa_bench` |
| 找热点函数 | perf record | `perf record -g --call-graph dwarf <binary> && perf report` |
| 看热点指令 | perf annotate | `perf annotate --stdio -s <函数名>` |
| 生成火焰图 | 仓库脚本 | `./tools/performance/generate_flamegraph.sh <binary> output.svg` |
| 缓存模拟分析 | Valgrind | `valgrind --tool=cachegrind <binary>` |
| 调用图分析 | Valgrind | `valgrind --tool=callgrind <binary>` |
| 微架构分析 | VTune | `vtune -collect uarch-exploration <binary>` |
| 内存安全检查 | ASan | `cmake --preset=asan && cmake --build build/asan && ctest --preset=asan` |
| 数据竞争检查 | TSan | `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan` |
| 未定义行为检查 | UBSan | `cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan` |
| 对比 benchmark | 仓库脚本 | `python tools/performance/benchmark_compare.py base.json cur.json` |
| 构建 profiling 版本 | CMake | `cmake --preset=relwithdebinfo && cmake --build build/relwithdebinfo` |
