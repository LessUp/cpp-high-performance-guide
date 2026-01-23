# 性能分析指南

本指南覆盖 C++ 性能分析的关键工具与方法。

## 概览

性能优化遵循一个简单循环：
1. **测量** - 通过分析找到瓶颈
2. **分析** - 理解根因
3. **优化** - 有针对性地改进
4. **验证** - 再次测量确认收益

## 工具

### perf（Linux）

`perf` 是 Linux 上的标准性能分析工具。

#### 安装

```bash
# Ubuntu/Debian
sudo apt-get install linux-tools-common linux-tools-generic

# Fedora
sudo dnf install perf
```

#### 基本用法

```bash
# 记录 CPU 采样
perf record -g ./your_benchmark

# 查看报告
perf report

# 查看注释源代码
perf annotate
```

#### 常用命令

```bash
# CPU 周期统计
perf stat ./your_benchmark

# 缓存未命中分析
perf stat -e cache-references,cache-misses,L1-dcache-load-misses ./your_benchmark

# 分支预测
perf stat -e branches,branch-misses ./your_benchmark

# 记录调用图（C++ 用 dwarf）
perf record -g --call-graph dwarf ./your_benchmark
```

### FlameGraph

FlameGraph 以直观方式展示时间花费。

#### 使用项目脚本

```bash
# 为某个基准生成 FlameGraph
./tools/flamegraph/generate_flamegraph.sh ./build/release/examples/02-memory-cache/bench/aos_soa_bench

# 查看结果
firefox flamegraph.svg
```

#### 手动生成

```bash
# 克隆 FlameGraph 工具（如果尚未完成）
git clone https://github.com/brendangregg/FlameGraph.git

# 使用 perf 采样
perf record -F 99 -g ./your_benchmark

# 生成 FlameGraph
perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > flamegraph.svg
```

#### 读懂 FlameGraph

- **宽度** = 时间开销（越宽时间越多）
- **高度** = 调用栈深度
- **颜色** = 随机（无意义）
- **顶部** = 当前执行函数
- **底部** = 入口点（main）

关注点：
- 宽平台（热点函数）
- 深调用栈（过深的调用）
- 意外函数耗时

### Valgrind

Valgrind 提供细粒度的内存与缓存分析。

#### Cachegrind（缓存模拟）

```bash
# 运行缓存模拟
valgrind --tool=cachegrind ./your_benchmark

# 查看结果
cg_annotate cachegrind.out.*
```

输出包含：
- I1 缓存未命中（指令缓存）
- D1 缓存未命中（L1 数据缓存）
- LL 缓存未命中（最后一级缓存）

#### Callgrind（调用图分析）

```bash
# 运行调用图分析
valgrind --tool=callgrind ./your_benchmark

# 使用 KCachegrind 查看（GUI）
kcachegrind callgrind.out.*
```

### Intel VTune（高级）

在 Intel CPU 上，VTune 提供最详细的分析能力。

#### 安装

从 [Intel oneAPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) 下载。

#### 基本用法

```bash
# 热点分析
vtune -collect hotspots ./your_benchmark

# 内存访问分析
vtune -collect memory-access ./your_benchmark

# 微架构分析
vtune -collect uarch-exploration ./your_benchmark

# 查看结果
vtune-gui
```

## 分析策略

### CPU 密集型代码

1. 先用 `perf stat` 获取概览
2. 用 `perf record` + FlameGraph 找热点函数
3. 用 `perf annotate` 查看热点指令
4. 结合编译器报告检查向量化情况

```bash
# 检查代码是否被向量化
g++ -O3 -march=native -fopt-info-vec-optimized your_code.cpp
```

### 内存密集型代码

1. 通过 `perf stat` 查看缓存未命中
2. 用 Cachegrind 进行细粒度缓存分析
3. 关注：
   - L1 未命中率高（> 5%）
   - LLC 未命中率高（> 1%）
   - 空间局部性差

```bash
# 快速缓存检查
perf stat -e L1-dcache-load-misses,L1-dcache-loads ./your_benchmark
```

### 多线程代码

1. 检查伪共享
2. 分析锁竞争
3. 验证线程扩展性

```bash
# 检查缓存行抖动（伪共享指标）
perf stat -e cache-misses ./your_benchmark

# 使用不同线程数运行
OMP_NUM_THREADS=1 ./your_benchmark
OMP_NUM_THREADS=2 ./your_benchmark
OMP_NUM_THREADS=4 ./your_benchmark
```

## 常见性能问题

### 1. 缓存未命中

**现象：**
- L1/L2/L3 未命中率高
- 内存带宽饱和

**解决思路：**
- 改善数据局部性（SOA 布局）
- 使用预取
- 缩小工作集

### 2. 分支预测失败

**现象：**
- branch-misses 数量高
- 控制流不可预测

**解决思路：**
- 使用无分支代码
- 对数据排序以改善预测
- 使用 CMOV 指令

### 3. 伪共享

**现象：**
- 多线程扩展性差
- 缓存到缓存传输高

**解决思路：**
- 将数据填充到缓存行边界
- 使用线程本地存储
- 减少共享状态

### 4. 向量化失败

**现象：**
- 热点循环仍是标量代码
- 汇编中看不到 SIMD 指令

**解决思路：**
- 对齐数据
- 使用 `restrict` 指针
- 简化循环结构
- 使用显式 SIMD 内在函数

## 基准测试最佳实践

### 避免测量误差

```cpp
// 防止死代码消除
benchmark::DoNotOptimize(result);

// 确保内存写入可见
benchmark::ClobberMemory();
```

### 预热缓存

```cpp
// 测量前先运行若干次
for (int i = 0; i < warmup_iterations; ++i) {
    do_work();
}
```

### 控制环境

```bash
# 禁用 CPU 频率动态调整
sudo cpupower frequency-set --governor performance

# 绑定到指定 CPU
taskset -c 0 ./your_benchmark

# 关闭 ASLR 以提高可复现性
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

### 统计显著性

- 运行多次
- 报告均值、中位数与标准差
- 使用 Google Benchmark 内置统计

```bash
# 输出统计信息
./your_benchmark --benchmark_repetitions=10 --benchmark_report_aggregates_only=true
```

## 速查表

| 任务 | 工具 | 命令 |
|------|------|------|
| CPU 热点 | perf | `perf record -g ./bench && perf report` |
| 缓存未命中 | perf | `perf stat -e cache-misses ./bench` |
| 可视化分析 | FlameGraph | `./tools/flamegraph/generate_flamegraph.sh ./bench` |
| 详细缓存 | Valgrind | `valgrind --tool=cachegrind ./bench` |
| 调用图 | Valgrind | `valgrind --tool=callgrind ./bench` |
| 向量化 | GCC | `-fopt-info-vec-optimized` |
| 向量化 | Clang | `-Rpass=loop-vectorize` |
