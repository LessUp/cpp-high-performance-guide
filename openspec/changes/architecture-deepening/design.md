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

## 评估但未实施的候选

### 候选3：提取共享对齐基础设施

**原因**：两个对齐分配器有不同的目的：
- `hpc::memory::AlignedAllocator` - 编译期缓存行对齐（CACHE_LINE_SIZE）
- `hpc::simd::aligned_allocator` - 运行期SIMD对齐（get_simd_alignment()）

**决策**：保持现状。代码文档已解释设计决策，合并会增加复杂性和耦合。

### 候选5：统一标量参考实现

**原因**：目前只有一个测试文件使用标量参考实现。

**决策**：保持现状。符合"保持简单"原则，无需过早提取。

### 候选6：清理命名空间污染

**原因**：向后兼容的 `using` 声明保持了现有代码的兼容性。

**决策**：保持现状。通过注释引导用户使用正确方式，而非破坏性变更。

## 架构原则总结

本次深化遵循以下原则：

1. **删除测试**：对每个模块应用"删除测试"，删除纯传递层，保留有价值模块
2. **接口隔离**：避免跨模块依赖，通过文档建立概念关联
3. **测试表面**：为接口添加测试，使其成为可测试的接缝
4. **局部性**：将复杂性集中在一处，而非分散到多个浅层模块
5. **杠杆**：一个深层模块服务多个用例

## 测试结果

所有修改后，测试套件从54个测试增加到61个测试，全部通过：

```
100% tests passed, 0 tests failed out of 61
```

## 后续建议

如果项目进入维护模式，建议：

1. 考虑将 `SPSCQueue` 和 `MPMCQueue` 模板移至头文件，便于复用
2. 添加MPMC队列的单元测试
3. 在CLAUDE.md中添加架构决策记录引用
