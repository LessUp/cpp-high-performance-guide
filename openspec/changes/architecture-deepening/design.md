# Architecture Deepening

## 概述

本文档记录了针对C++高性能指南项目的架构深化改进，遵循"深化机会"原则：将浅层模块转化为深层模块，提升测试性和AI可导航性。

## 已完成的深化

### 1. 删除浅层冗余模块

**问题**：存在两个纯冗余的模块，增加认知负荷但未提供价值。

**解决方案**：
- 删除 `UnalignedCounter` - 与 `std::atomic<int64_t>` 无区别
- 删除 `SpinLockGuard` - 已标记 `[[deprecated]]`，`std::lock_guard<SpinLock>` 完全替代

**收益**：
- 局部性：维护者无需理解"为什么有两个几乎相同的计数器"
- 减少代码量约40行
- 消除废弃代码的认知负荷

**文件修改**：
- `examples/05-concurrency/include/concurrency_utils.hpp` - 删除冗余类型
- `tests/property/memory_properties.cpp` - 使用库代码替代本地定义

### 2. 统一缓存行对齐概念

**问题**：同一个"带填充的对齐计数器"概念有多个实现，理解缓存行对齐需要跳转6个位置。

**解决方案**：
- 保留 `AlignedCounter` 作为独立实现（避免跨模块依赖）
- 添加文档说明其等同于 `CacheLinePadded<std::atomic<int64_t>>`
- 更新测试使用库代码而非本地重新定义
- 示例代码中的 `PaddedCounter` 添加文档指向相关类型

**收益**：
- 通过文档建立概念关联而非代码耦合
- 测试验证库代码而非重复实现
- 保持模块独立性

**文件修改**：
- `examples/05-concurrency/include/concurrency_utils.hpp` - 添加文档
- `examples/02-memory-cache/src/false_sharing.cpp` - 添加文档说明
- `tests/property/memory_properties.cpp` - 使用 `AlignedCounter`

### 3. 补充无锁队列单元测试

**问题**：`SPSCQueue` 和 `MPMCQueue` 没有单元测试，只有演示代码。并发bug是时序依赖的，演示无法系统测试。

**解决方案**：
- 添加 `tests/unit/concurrency/lock_free_queue_test.cpp`
- 测试覆盖：基本入队/出队、边界条件、FIFO顺序、环形缓冲区回绕、多线程生产者-消费者

**收益**：
- 接口成为可测试的接缝
- 并发bug可在测试中复现和修复
- 7个测试用例覆盖核心功能

**文件修改**：
- 新增 `tests/unit/concurrency/lock_free_queue_test.cpp`
- 更新 `tests/unit/concurrency/CMakeLists.txt`

### 4. 提取无锁队列到共享头文件

**问题**：`SPSCQueue` 和 `MPMCQueue` 在两个地方重复定义，测试文件复制了完整实现（109行），违反DRY原则。

**解决方案**：
- 创建 `examples/05-concurrency/include/lock_free_queue.hpp`
- 将 `SPSCQueue` 和 `MPMCQueue` 模板类移至头文件
- `lock_free_queue.cpp` 保留演示代码，包含头文件
- 测试文件包含头文件而非重复定义

**收益**：
- 消除重复代码约90行
- 单一真实来源，提高局部性
- 便于未来复用队列实现
- 添加了 MPMCQueue 的测试用例

**文件修改**：
- 新增 `examples/05-concurrency/include/lock_free_queue.hpp`
- 修改 `examples/05-concurrency/src/lock_free_queue.cpp`
- 修改 `tests/unit/concurrency/lock_free_queue_test.cpp` (删除约90行重复定义)
- 更新 CMakeLists.txt 包含新头文件目录

### 5. 提取 Buffer 类到共享头文件

**问题**：`Buffer` 类在示例和基准测试中重复定义，两个实现几乎相同，修改需要同时更新两个文件。

**解决方案**：
- 创建 `examples/03-modern-cpp/include/buffer.hpp`
- 将 `Buffer` 类移至头文件，包含统计计数器
- 示例和基准测试包含共享头文件

**收益**：
- 消除重复代码约45行
- 单一真实来源，提高局部性
- 基准测试可以使用完整的 Buffer 实现（包括统计）

**文件修改**：
- 新增 `examples/03-modern-cpp/include/buffer.hpp`
- 修改 `examples/03-modern-cpp/src/move_semantics.cpp`
- 修改 `examples/03-modern-cpp/bench/move_semantics_bench.cpp` (删除约45行重复定义)
- 更新 CMakeLists.txt 包含新头文件目录

### 6. 删除 benchmark_utils.hpp 中的传递层

**问题**：`DoNotOptimize`、`ClobberMemory`、`export_to_json` 等函数完全复制 Google Benchmark 的接口或功能，没有提供抽象价值。

**解决方案**：
- 删除 `DoNotOptimize` 和 `ClobberMemory` 包装函数（直接使用 Google Benchmark 原生 API）
- 删除 `BenchmarkResult`、`BenchmarkSuite`、`export_to_json`、`export_suite_to_json`（Google Benchmark 原生支持 JSON 输出）
- 保留 `Timer`、`format_throughput`、`format_time`、`calculate_speedup`（这些有实际使用价值）

**收益**：
- 消除浅模块，减少约150行维护负担
- 使用标准工具而非重复造轮子
- 测试验证真实场景（使用原生 JSON 输出）

**文件修改**：
- `benchmarks/common/benchmark_utils.hpp` (删除约150行)
- `tests/property/benchmark_properties.cpp` (重构测试逻辑，使用 Google Benchmark 原生 JSON 输出)

### 7. 重构 CMake 函数减少重复逻辑

**问题**：`hpc_set_compiler_options` 和 `hpc_enable_simd` 中有大量重复的配置逻辑。

**解决方案**：
- 创建辅助函数 `_hpc_add_config_options` 支持配置条件编译选项
- 使用 CMake 生成器表达式减少重复代码
- 简化 SIMD 选项设置逻辑

**收益**：
- 减少约30行 CMake 代码
- 提高可维护性
- 便于未来添加新配置

**文件修改**：
- `cmake/CompilerOptions.cmake` (重构约30行)

## 评估但未实施的候选

### 候选A：提取标量参考实现

**原因**：目前只有一个测试文件使用标量参考实现。

**决策**：保持现状。符合"保持简单"原则，无需过早提取。当第二个文件需要标量实现时再考虑。

### 候选B：泛化 SIMD 运行时分派器

**原因**：当前只有一个分派函数 `dispatch_add_arrays`。

**决策**：保持现状。只有一个使用点时，泛化会增加复杂性。当需要第二个分派函数时再泛化。

### 候选C：删除 SIMD 包装函数

**原因**：`add_arrays_wrapped` 等函数只是 `FloatVec` 操作的简单包装。

**决策**：保留。这是教学项目，包装函数提供了清晰的示例，便于读者理解 SIMD 编程模式。

### 候选D：清理命名空间污染

**原因**：向后兼容的 `using` 声明保持了现有代码的兼容性。

**决策**：保持现状。通过注释引导用户使用正确方式，而非破坏性变更。

## 架构原则总结

本次深化遵循以下原则：

1. **删除测试**：对每个模块应用"删除测试"，删除纯传递层，保留有价值模块
2. **接口隔离**：避免跨模块依赖，通过文档建立概念关联
3. **测试表面**：为接口添加测试，使其成为可测试的接缝
4. **局部性**：将复杂性集中在一处，而非分散到多个浅层模块
5. **杠杆**：一个深层模块服务多个用例
6. **避免过早抽象**：只有一个使用点时，保持现状

## 测试结果

所有修改后，测试套件保持65个测试，全部通过：

```
100% tests passed, 0 tests failed out of 65
```

测试数量变化：
- 删除 `validate_result` 测试（函数已删除）
- 删除 JSON 导出测试（使用 Google Benchmark 原生 JSON）
- 添加 MPMCQueue 测试（之前只有 SPSCQueue）
- 添加 Google Benchmark 原生 JSON 测试文档

## 代码统计

本次深化共：
- 新增约 300 行（共享头文件）
- 删除约 285 行（重复代码和浅层模块）
- 净减少约 15 行代码
- 提高局部性和可维护性

## 后续建议

项目已进入维护模式，当前架构状态良好：
- ✅ 无锁队列已移至头文件，便于复用
- ✅ MPMC 队列已有单元测试
- ✅ 架构决策已记录在本文档中
