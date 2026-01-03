# Implementation Plan: HPC Optimization Guide

## Overview

本实现计划将项目分为多个阶段：首先搭建构建系统基础设施，然后逐步实现各个优化示例模块，最后完善文档和 CI/CD。每个任务都是增量式的，确保每一步都能编译运行。

## Tasks

- [x] 1. 搭建项目基础结构和 CMake 构建系统
  - [x] 1.1 创建根目录 CMakeLists.txt 和项目基本结构
    - 设置 C++20 标准
    - 配置项目名称和版本
    - 创建 cmake/ 目录结构
    - _Requirements: 1.1_

  - [x] 1.2 实现 cmake/Dependencies.cmake 依赖管理
    - 使用 FetchContent 下载 Google Benchmark
    - 使用 FetchContent 下载 Google Test
    - 使用 FetchContent 下载 RapidCheck
    - _Requirements: 1.2_

  - [x] 1.3 实现 cmake/CompilerOptions.cmake 编译器选项管理
    - 创建 hpc_set_compiler_options() 函数
    - 配置警告级别 (-Wall, -Wextra, -Wpedantic)
    - 配置优化选项 (-O3, -march=native)
    - _Requirements: 1.4_

  - [x] 1.4 实现 cmake/Sanitizers.cmake 和 CMakePresets.json
    - 创建 hpc_enable_sanitizers() 函数
    - 配置 ASan, TSan, UBSan
    - 创建 CMakePresets.json (debug, release, asan, tsan)
    - _Requirements: 1.3, 8.4_

  - [x] 1.5 实现 cmake/ExampleTemplate.cmake 示例模块模板
    - 创建 hpc_add_example() 函数
    - 支持自动注册基准测试
    - 支持自动链接依赖
    - _Requirements: 1.6_

  - [x] 1.6 编写属性测试验证 CMake 模板一致性
    - **Property 2: Example Module Template Consistency**
    - **Validates: Requirements 1.6**

- [x] 2. Checkpoint - 验证构建系统
  - 确保 cmake --preset=release 能成功配置
  - 确保所有预设都能正常工作
  - 如有问题请询问用户

- [x] 3. 实现内存与缓存优化模块 (02-memory-cache)
  - [x] 3.1 实现 AOS vs SOA 示例
    - 创建 examples/02-memory-cache/src/aos_vs_soa.cpp
    - 实现 ParticleAOS 和 ParticleSOA 结构
    - 实现 update_particles_aos() 和 update_particles_soa()
    - 创建基准测试 bench/aos_soa_bench.cpp
    - _Requirements: 2.1_

  - [x] 3.2 编写属性测试验证 SOA 性能优势
    - **Property 3: SOA Performance Advantage for Sequential Access**
    - **Validates: Requirements 2.1**

  - [x] 3.3 实现伪共享示例
    - 创建 examples/02-memory-cache/src/false_sharing.cpp
    - 实现未对齐计数器多线程递增
    - 实现 alignas(64) 对齐计数器
    - 创建基准测试对比
    - _Requirements: 2.2, 5.3_

  - [x] 3.4 编写属性测试验证对齐消除伪共享
    - **Property 4: Cache-Line Aligned Counters Eliminate False Sharing**
    - **Validates: Requirements 2.2, 5.3**

  - [x] 3.5 实现内存对齐示例
    - 创建 examples/02-memory-cache/src/alignment.cpp
    - 演示对齐 vs 非对齐内存访问
    - 创建 SIMD 友好的对齐分配器
    - _Requirements: 2.3_

  - [x] 3.6 编写属性测试验证对齐内存 SIMD 性能
    - **Property 5: Aligned Memory SIMD Performance**
    - **Validates: Requirements 2.3**

  - [x] 3.7 实现预取示例
    - 创建 examples/02-memory-cache/src/prefetch.cpp
    - 演示 __builtin_prefetch 用法
    - 创建大数组遍历基准测试
    - _Requirements: 2.4_

- [x] 4. Checkpoint - 验证内存模块
  - 运行所有内存模块基准测试
  - 确保 SOA 比 AOS 快
  - 确保对齐计数器比未对齐快
  - 如有问题请询问用户

- [x] 5. 实现现代 C++ 特性优化模块 (03-modern-cpp)
  - [x] 5.1 实现 constexpr/consteval 示例
    - 创建 examples/03-modern-cpp/src/compile_time.cpp
    - 演示编译期计算 vs 运行时计算
    - 展示 consteval 强制编译期求值
    - _Requirements: 3.1_

  - [x] 5.2 实现移动语义示例
    - 创建 examples/03-modern-cpp/src/move_semantics.cpp
    - 实现包含大缓冲区的类
    - 对比拷贝构造 vs 移动构造性能
    - _Requirements: 3.2_

  - [x] 5.3 编写属性测试验证移动语义性能优势
    - **Property 6: Move Semantics Performance Advantage**
    - **Validates: Requirements 3.2**

  - [x] 5.4 实现 vector 容量管理示例
    - 创建 examples/03-modern-cpp/src/vector_reserve.cpp
    - 演示 reserve() 减少重分配
    - 使用自定义分配器统计分配次数
    - _Requirements: 3.3_

  - [x] 5.5 编写属性测试验证 reserve 减少分配
    - **Property 7: Vector Reserve Reduces Allocations**
    - **Validates: Requirements 3.3**

  - [x] 5.6 实现 C++20 Ranges vs Raw Loops 示例
    - 创建 examples/03-modern-cpp/src/ranges_vs_loops.cpp
    - 对比 std::ranges 和手写循环性能
    - 分析编译器优化差异
    - _Requirements: 3.4_

- [x] 6. Checkpoint - 验证现代 C++ 模块
  - 运行所有现代 C++ 模块基准测试
  - 确保移动比拷贝快
  - 如有问题请询问用户

- [x] 7. 实现 SIMD 向量化模块 (04-simd-vectorization)
  - [x] 7.1 实现自动向量化示例
    - 创建 examples/04-simd-vectorization/src/auto_vectorize.cpp
    - 展示易于自动向量化的代码模式
    - 展示阻碍向量化的反模式
    - 添加编译器向量化报告
    - _Requirements: 4.1_

  - [x] 7.2 实现 SIMD intrinsics 入门示例
    - 创建 examples/04-simd-vectorization/src/intrinsics_intro.cpp
    - 实现 SSE 版本向量加法
    - 实现 AVX2 版本向量加法
    - 实现 AVX-512 版本向量加法（条件编译）
    - _Requirements: 4.2_

  - [x] 7.3 实现 SIMD 封装库
    - 创建 examples/04-simd-vectorization/include/simd_wrapper.hpp
    - 封装常用 SIMD 操作为 C++ 类
    - 提供标量回退实现
    - _Requirements: 4.3_

  - [x] 7.4 编写属性测试验证 SIMD 封装正确性
    - **Property 8: SIMD Wrapper Correctness**
    - **Validates: Requirements 4.2, 4.3**

  - [x] 7.5 创建 SIMD 基准测试
    - 创建 bench/simd_bench.cpp
    - 对比标量 vs SSE vs AVX2 vs AVX-512
    - 计算加速比
    - _Requirements: 4.5_

  - [x] 7.6 编写属性测试验证向量化加速
    - **Property 9: Vectorized vs Scalar Speedup**
    - **Validates: Requirements 4.1, 4.5**

- [x] 8. Checkpoint - 验证 SIMD 模块
  - 运行所有 SIMD 基准测试
  - 确保向量化版本比标量快
  - 如有问题请询问用户

- [x] 9. 实现并发与多线程模块 (05-concurrency)
  - [x] 9.1 实现 std::atomic 内存序示例
    - 创建 examples/05-concurrency/src/atomic_ordering.cpp
    - 演示不同内存序的使用场景
    - 展示 relaxed, acquire-release, seq_cst 差异
    - _Requirements: 5.1_

  - [x] 9.2 编写属性测试验证原子操作正确性
    - **Property 10: Atomic Operations Correctness**
    - **Validates: Requirements 5.1**

  - [x] 9.3 实现无锁队列示例
    - 创建 examples/05-concurrency/src/lock_free_queue.cpp
    - 实现简单的无锁 SPSC 队列
    - 添加正确性验证测试
    - _Requirements: 5.2_

  - [x] 9.4 编写属性测试验证无锁队列不变量
    - **Property 11: Lock-Free Queue Invariants**
    - **Validates: Requirements 5.2**

  - [x] 9.5 实现 OpenMP 示例
    - 创建 examples/05-concurrency/src/openmp_basics.cpp
    - 演示 parallel for, reduction, sections
    - 创建线程扩展性基准测试
    - _Requirements: 5.4_

  - [x] 9.6 编写属性测试验证 OpenMP 扩展效率
    - **Property 12: OpenMP Scaling Efficiency**
    - **Validates: Requirements 5.4, 5.5**

- [x] 10. Checkpoint - 验证并发模块
  - 运行所有并发模块测试
  - 使用 TSan 检查数据竞争
  - 如有问题请询问用户

- [x] 11. 实现基准测试框架和工具
  - [x] 11.1 创建基准测试通用工具
    - 创建 benchmarks/common/benchmark_utils.hpp
    - 实现 DoNotOptimize 和 ClobberMemory 封装
    - 实现 JSON 结果导出
    - _Requirements: 6.1, 6.4_

  - [x] 11.2 编写属性测试验证 JSON 输出有效性
    - **Property 13: Benchmark JSON Output Validity**
    - **Validates: Requirements 6.4**

  - [x] 11.3 创建 FlameGraph 生成脚本
    - 创建 tools/flamegraph/generate_flamegraph.sh
    - 集成 perf 和 FlameGraph 工具
    - 添加使用说明
    - _Requirements: 6.3_

  - [x] 11.4 创建基准测试比较工具
    - 创建 tools/analysis/benchmark_compare.py
    - 实现基准测试结果对比
    - 实现性能回归检测
    - _Requirements: 6.2_

- [x] 12. 实现现代 CMake 示例模块 (01-cmake-modern)
  - [x] 12.1 创建 CMake 反模式 vs 最佳实践示例
    - 创建 examples/01-cmake-modern/anti-patterns/
    - 创建 examples/01-cmake-modern/best-practices/
    - 展示 include_directories vs target_include_directories
    - 展示依赖管理最佳实践
    - _Requirements: 1.5_

- [x] 13. 配置 CI/CD
  - [x] 13.1 创建 GitHub Actions 构建工作流
    - 创建 .github/workflows/build.yml
    - 配置多编译器矩阵 (GCC, Clang)
    - 配置多构建类型 (Debug, Release)
    - _Requirements: 8.1, 8.3_

  - [x] 13.2 创建基准测试工作流
    - 创建 .github/workflows/benchmark.yml
    - 配置基准测试运行
    - 配置性能回归检测
    - _Requirements: 8.2_

  - [x] 13.3 配置 Sanitizer 测试
    - 在 CI 中启用 ASan, TSan, UBSan
    - 配置失败时阻止合并
    - _Requirements: 8.4, 8.5, 5.6_

- [x] 14. 创建项目文档
  - [x] 14.1 创建主 README.md
    - 项目介绍和快速开始
    - 构建说明
    - 模块概览
    - _Requirements: 7.1_

  - [x] 14.2 创建学习路径文档
    - 创建 docs/learning-path.md
    - 按难度组织示例
    - 提供推荐学习顺序
    - _Requirements: 7.2, 7.4_

  - [x] 14.3 创建性能分析指南
    - 创建 docs/profiling-guide.md
    - 介绍 perf, valgrind, VTune 基础
    - 提供 FlameGraph 使用教程
    - _Requirements: 6.6_

  - [x] 14.4 为每个示例模块创建 README
    - 每个 examples/XX-*/ 目录添加 README.md
    - 解释优化原理和使用场景
    - _Requirements: 7.3_

- [x] 15. Final Checkpoint - 完整验证
  - 运行所有测试和基准测试
  - 验证 CI 工作流正常
  - 验证文档完整性
  - 如有问题请询问用户

## Notes

- 所有任务都是必须完成的，包括属性测试任务
- 每个 Checkpoint 用于验证阶段性成果
- 属性测试使用 RapidCheck 库，每个测试至少运行 100 次迭代
- 单元测试和属性测试互补，共同保证代码质量
