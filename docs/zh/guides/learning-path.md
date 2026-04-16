# 学习路径

本指南提供按从入门到进阶排序的 HPC 优化学习顺序。

---

## 先决条件

开始前建议具备：
- 基础 C++ 知识（类、模板、STL）
- 命令行工具使用经验
- 计算机体系结构基础概念

详见 [先决条件](../getting-started/prerequisites.md)。

---

## 阶段 1：构建系统基础

### 1.1 现代 CMake ([examples/01-cmake-modern](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/01-cmake-modern))

从这里开始了解项目结构与构建系统。

**主题：**
- 为什么目标驱动的 CMake 优于目录驱动
- 使用 `target_include_directories` 而非 `include_directories`
- 使用 FetchContent 管理依赖
- 通过 CMake 预设实现可复现构建

**练习：**
1. 使用不同预设构建项目
2. 使用模板新增一个示例模块
3. 对比反模式与最佳实践的 CMakeLists.txt

---

## 阶段 2：内存基础

### 2.1 数据布局 - AOS vs SOA ([examples/02-memory-cache](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/02-memory-cache))

理解数据布局是缓存优化的基础。

**关键概念：**
- 缓存行与空间局部性
- 结构体数组 vs 数组结构体
- 何时选用不同布局

**基准：**
```bash
./build/release/examples/02-memory-cache/bench/aos_soa_bench
```

### 2.2 内存对齐

理解对齐如何影响 SIMD 性能。

**关键概念：**
- `alignas` 说明符
- 对齐内存分配
- SIMD 对齐要求

### 2.3 伪共享

多线程性能的关键问题。

**关键概念：**
- 缓存行竞争
- 使用 `alignas(64)` 进行缓存行填充
- 使用 perf 检测伪共享

### 2.4 预取

高级内存优化技术。

**关键概念：**
- `__builtin_prefetch` 用法
- 预取距离调优
- 预取何时有用（何时无用）

---

## 阶段 3：现代 C++ 性能

### 3.1 编译期计算 ([examples/03-modern-cpp](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/03-modern-cpp))

将计算从运行期移至编译期。

**关键概念：**
- `constexpr` 函数与变量
- `consteval` 保证编译期求值
- 模板元编程基础

### 3.2 移动语义

避免不必要的拷贝。

**关键概念：**
- 右值引用
- 移动构造与赋值
- `std::move` 用法

### 3.3 Vector 容量管理

优化容器使用方式。

**关键概念：**
- `reserve()` 与自动扩容
- 分配次数统计
- 容量与大小的区别

### 3.4 C++20 Ranges

现代迭代模式。

**关键概念：**
- Range 适配器与视图
- 惰性求值
- 与裸循环的性能对比

---

## 阶段 4：SIMD 向量化

### 4.1 自动向量化 ([examples/04-simd-vectorization](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/04-simd-vectorization))

让编译器完成向量化。

**关键概念：**
- 适合向量化的代码模式
- 编译器向量化报告
- 常见向量化阻碍

**编译器参数：**
```bash
# GCC 向量化报告
-fopt-info-vec-optimized

# Clang 向量化报告
-Rpass=loop-vectorize
```

### 4.2 SIMD 内在函数

手动向量化以获得最大控制力。

**关键概念：**
- SSE、AVX2、AVX-512 指令集
- 内在函数
- SIMD 数据对齐

### 4.3 SIMD 封装

可读的 SIMD 代码。

**关键概念：**
- 封装内在函数
- 标量回退实现
- 类型安全的 SIMD 操作

---

## 阶段 5：并发编程

### 5.1 原子操作 ([examples/05-concurrency](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/05-concurrency))

无锁编程的基础。

**关键概念：**
- `std::atomic` 基础
- 内存序（relaxed、acquire、release、seq_cst）
- 不同内存序的使用场景

### 5.2 无锁队列

实用的无锁数据结构。

**关键概念：**
- SPSC 队列设计
- 实践中的内存序
- 正确性验证

### 5.3 OpenMP

简单并行化。

**关键概念：**
- `#pragma omp parallel for`
- 归约
- 线程扩展性

---

## 阶段 6：性能分析与诊断

### 6.1 基准测试

学会正确测量。

**主题：**
- Google Benchmark 用法
- `DoNotOptimize` 与 `ClobberMemory`
- 参数化基准

### 6.2 性能分析

定位性能瓶颈。

**工具：**
- `perf` 进行 CPU 采样
- FlameGraph 可视化
- 缓存未命中分析

详见 [性能分析指南](profiling-guide.md)。

---

## 推荐学习计划

| 周数 | 主题 |
|------|------|
| 1 | 阶段 1 + 阶段 2.1-2.2 |
| 2 | 阶段 2.3-2.4 + 阶段 3.1-3.2 |
| 3 | 阶段 3.3-3.4 + 阶段 4.1 |
| 4 | 阶段 4.2-4.3 |
| 5 | 阶段 5.1-5.2 |
| 6 | 阶段 5.3 + 阶段 6 |

---

## 下一步

完成学习路径后：
1. 分析你自己的代码以定位瓶颈
2. 应用相关优化
3. 度量改进效果
4. [为本项目贡献新的示例！](../../CONTRIBUTING.zh.md)

---

## 相关资源

- [最佳实践](best-practices.md) - 工业级测试模式
- [API参考](../reference/api-reference.md) - 工具函数
- [常见问题](../reference/faq.md) - 常见问题解答
