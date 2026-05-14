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

---

## 2026-05-13 深化（第二轮）

### 8. 创建 CONTEXT.md 领域词汇表

**问题**：项目缺乏统一的领域术语定义，影响 AI 辅助开发的准确性。

**解决方案**：
- 创建 `CONTEXT.md` 定义核心领域术语
- 包含：缓存行对齐、伪共享、AOS/SOA、无锁、内存序、SIMD、向量化等
- 提供中英文对照

**收益**：
- AI 可导航性提升
- 文档一致性
- 新维护者快速上手

**文件修改**：
- 新增 `CONTEXT.md`

### 9. 提取测试源文件到头文件

**问题**：`modern_cpp_examples_test.cpp` 直接包含 `.cpp` 源文件，使用 `HPC_TEST_MODE` 宏切换行为。这是纯函数提取反模式——测试绑定实现细节而非接口。

**解决方案**：
- 创建 `vector_reserve.hpp`：提取 `CountingAllocator` 模板类
- 创建 `compile_time.hpp`：提取 `factorial_*`、`fnv1a_hash`、`is_prime`、`FIRST_100_PRIMES`
- 创建 `ranges_utils.hpp`：提取 `transform_*`、`filter_*`、`chain_*`、`sum_*`、`to_vector`
- 修改测试包含头文件而非源文件
- 更新 `buffer.hpp`：添加 `process_by_copy` 和 `process_by_ref` 函数

**收益**：
- 接口成为测试表面
- 修改实现无需修改测试
- 与已完成的 `buffer.hpp` 模式一致

**文件修改**：
- 新增 `examples/03-modern-cpp/include/vector_reserve.hpp`
- 新增 `examples/03-modern-cpp/include/compile_time.hpp`
- 新增 `examples/03-modern-cpp/include/ranges_utils.hpp`
- 修改 `examples/03-modern-cpp/include/buffer.hpp`
- 修改 `examples/03-modern-cpp/src/vector_reserve.cpp`
- 修改 `examples/03-modern-cpp/src/compile_time.cpp`
- 修改 `examples/03-modern-cpp/src/ranges_vs_loops.cpp`
- 修改 `examples/03-modern-cpp/src/move_semantics.cpp`
- 修改 `examples/03-modern-cpp/CMakeLists.txt`
- 修改 `tests/unit/modern_cpp/modern_cpp_examples_test.cpp`

### 10. 添加 simd_allocator 别名

**问题**：`hpc::simd::aligned_allocator` 和 `hpc::memory::AlignedAllocator` 命名相似但语义不同，可能造成混淆。

**解决方案**：
- 在 `simd_utils.hpp` 中添加 `simd_allocator` 别名
- 保留原有 `aligned_allocator` 名称（无破坏性）
- 添加文档说明与 `hpc::memory::AlignedAllocator` 的区别

**收益**：
- 命名更直观
- 无破坏性变更

**文件修改**：
- 修改 `examples/04-simd-vectorization/include/simd_utils.hpp`

### 11. 删除向后兼容层并更新引用

**问题**：`core.hpp` 包含向后兼容的 `using` 声明，将常量导入到 `hpc::memory` 和 `hpc::concurrency` 命名空间，增加认知负担。

**解决方案**：
- 删除 `core.hpp` 中的 `using` 声明
- 更新所有使用 `CACHE_LINE_SIZE` 和 `PAGE_SIZE` 的文件，添加 `hpc::core::` 前缀
- 在测试文件中添加 `using hpc::core::CACHE_LINE_SIZE;`

**收益**：
- 单一真实来源
- 命名空间更清晰
- 减少认知负荷

**文件修改**：
- 修改 `include/hpc/core.hpp`
- 修改 `examples/05-concurrency/include/lock_free_queue.hpp`
- 修改 `examples/05-concurrency/include/concurrency_utils.hpp`
- 修改 `examples/02-memory-cache/include/memory_utils.hpp`
- 修改 `examples/02-memory-cache/src/false_sharing.cpp`
- 修改 `tests/unit/memory/memory_utils_test.cpp`
- 修改 `tests/unit/concurrency/concurrency_utils_test.cpp`

### 第二轮深化总结

本次深化共：
- 新增约 250 行（新头文件）
- 修改约 20 个文件
- 删除向后兼容层，统一命名空间
- 创建领域词汇表

测试验证：
- Debug 构建：65/65 测试通过
- ASAN 构建：65/65 测试通过

---

## 2026-05-13 深化（第三轮）

### 12. 删除测试代码中的 SPSCQueue 重复定义

**问题**：`tests/property/concurrency_properties.cpp` 定义了简化版 `SPSCQueue`，与 `lock_free_queue.hpp` 中的真实实现不同：
- 测试版本使用 `T buffer_[Capacity]`，真实版本使用 `std::optional<T> buffer_[Capacity]`
- 测试版本缺少 `push(T&&)`、`size()`、`capacity()` 方法
- **测试可能测试的是错误实现**

**解决方案**：
- 删除测试文件中的重复定义（约50行）
- 添加 `#include "lock_free_queue.hpp"` 包含真实实现

**收益**：
- 局部性：队列实现变更只需一处
- 可测试性：确保测试真实实现
- AI 可导航性：减少重复代码

**文件修改**：
- 修改 `tests/property/concurrency_properties.cpp`

### 13. 统一分配器命名风格

**问题**：`hpc::simd::aligned_allocator` 使用 snake_case，与 `hpc::memory::AlignedAllocator` 的 PascalCase 不一致。

**解决方案**：
- 将 `aligned_allocator` 重命名为 `AlignedAllocator`（PascalCase）
- 添加 `aligned_allocator` 作为 `[[deprecated]]` 别名保持向后兼容
- 简化 `AlignedAllocator` 接口（删除已弃用的 `pointer`、`reference` 等类型别名）

**收益**：
- 命名一致性
- 向后兼容
- 更简洁的接口

**文件修改**：
- 修改 `examples/04-simd-vectorization/include/simd_utils.hpp`

### 14. 添加核心模块测试

**问题**：`include/hpc/core.hpp` 没有独立测试文件，功能仅被其他模块隐式测试。

**解决方案**：
- 创建 `tests/unit/core/core_test.cpp`
- 测试覆盖：
  - `cache_line_size()` 返回值是 2 的幂且在合理范围（16-256）
  - `page_size()` 返回值是 2 的幂且在合理范围（1024-65536）
  - `hardware_concurrency()` 返回值 >= 1 且与 `std::thread::hardware_concurrency()` 一致
  - 编译时常量与运行时函数的关系

**收益**：
- 可测试性：核心功能有独立测试覆盖
- AI 可导航性：新贡献者可快速理解核心 API

**文件修改**：
- 新增 `tests/unit/core/core_test.cpp`
- 新增 `tests/unit/core/CMakeLists.txt`
- 修改 `tests/unit/CMakeLists.txt`

### 15. 添加锁自由队列压力测试

**问题**：当前测试覆盖基本操作和简单并发场景，缺少高竞争压力测试。

**解决方案**：
- 添加 `SPSCQueueStressTest.HighThroughputOneMillionOperations` - 100万次操作
- 添加 `MPMCQueueStressTest.HighContentionMultipleProducersConsumers` - 4生产者×4消费者
- 添加 `SPSCQueueStressTest.RandomInterleavingWithDelays` - 随机延迟测试
- 添加 `SPSCQueueStressTest.NonTrivialTypeStress` - 非平凡类型（std::string）测试

**收益**：
- 可测试性：更高置信度的并发正确性
- TSan 验证通过，无数据竞争

**文件修改**：
- 修改 `tests/unit/concurrency/lock_free_queue_test.cpp`

### 16. 更新 particle_types 注释

**问题**：模块过于浅层，但这是有意为之的教学示例。

**解决方案**：
- 添加注释明确说明"示例模块，非生产用途"
- 列出生产使用应考虑的改进点

**收益**：
- 明确模块定位
- 避免误用

**文件修改**：
- 修改 `examples/02-memory-cache/include/particle_types.hpp`

### 17. 记录 header-only 设计决策

**问题**：平台检测宏在头文件中可见，但这是有意的设计决策。

**解决方案**：
- 在 `AGENTS.md` 中记录 header-only 设计决策
- 说明优先考虑集成便利性而非编译速度

**收益**：
- 设计决策有文档记录
- 未来维护者理解设计意图

**文件修改**：
- 修改 `AGENTS.md`

### 第三轮深化总结

本次深化共：
- 新增约 150 行（核心测试、压力测试）
- 删除约 50 行（重复定义）
- 修改 9 个文件
- 测试数量从 65 增加到 84

**附加修复**：修复 `compile_time.hpp` 缺少 `#include <cstddef>` 导致 ASan 构建失败的问题。

测试验证：
- Debug 构建：84/84 测试通过
- ASan 构建：84/84 测试通过
- TSan 构建：所有并发测试通过，无数据竞争
- 格式检查：通过

---

## Code Review 修复（2026-05-13）

对第三轮深化修改进行 code review 后发现以下问题并修复：

### 问题 1：AlignedAllocator 缺少边界处理

**问题**：`allocate(0)` 行为未定义，`deallocate(nullptr)` 可能导致问题。

**修复**：
- `allocate(0)` 返回 `nullptr`
- `deallocate(nullptr)` 直接返回不做处理

### 问题 2：AlignedAllocator 缺少 rebind 支持

**问题**：删除了原有的 `rebind` 结构，可能与旧代码不兼容。

**修复**：恢复 `rebind` 结构体。

### 问题 3：MPMCQueueStressTest 缺少超时机制

**问题**：主线程无限等待消费者完成，可能死锁。

**修复**：添加 10 秒超时，超时后 FAIL 并报告进度。

### 问题 4：吞吐量计算除零风险

**问题**：如果测试执行太快（duration == 0），会导致除零。

**修复**：添加除零保护，`duration > 0` 时才计算吞吐量。

**文件修改**：
- `examples/04-simd-vectorization/include/simd_utils.hpp`
- `tests/unit/concurrency/lock_free_queue_test.cpp`

---

## 第二次 Code Review 修复（2026-05-13）

对修复后的代码再次 review，核实并修复以下问题：

### 问题 1：AlignedAllocator 缺少整数溢出保护 ✅ 真实问题

**问题**：`n * sizeof(T)` 可能溢出，导致分配比预期小得多的内存。

**修复**：添加溢出检查，与 `hpc::memory::AlignedAllocator` 保持一致：
```cpp
if (n > std::numeric_limits<size_type>::max() / sizeof(T)) {
    throw std::bad_alloc();
}
```

同时添加 `<limits>` 头文件。

### 问题 2：冗余 return 语句 ✅ 代码清理

**问题**：`FAIL()` 后的 `return` 永不执行（`FAIL()` 抛异常中止测试）。

**修复**：删除冗余 `return;`。

### 问题 3：allocate(0) 返回 nullptr - 不修改

**分析**：C++ 标准未强制要求 `allocate(0)` 返回非空指针。当前实现返回 `nullptr`，配合 `deallocate(nullptr)` 的空指针保护，行为安全。

### 问题 4：随机数生成器线程安全 - 不是问题

**分析**：`rng` 只在 producer 线程中使用，主线程在 `join()` 后才继续，无并发访问。

**文件修改**：
- `examples/04-simd-vectorization/include/simd_utils.hpp`（添加溢出保护、`<limits>` 头文件）
- `tests/unit/concurrency/lock_free_queue_test.cpp`（删除冗余 return）

---

## 第三次 Code Review 修复（2026-05-13）

### 问题：`producers_done` 变量未使用（死代码）

**发现**：`MPMCQueueStressTest` 中声明并递增了 `producers_done`，但从未读取。

**修复**：删除未使用的变量和相关代码。

**文件修改**：
- `tests/unit/concurrency/lock_free_queue_test.cpp`

---

## 最终验证

所有修改后：
- Debug 构建：84/84 测试通过
- ASan 构建：84/84 测试通过
- TSan 构建：并发测试通过，无数据竞争
- 格式检查：通过

---

## 2026-05-14 深化（第四轮）—— 消除全局静态计数器污染

### 18. 创建可注入的 OperationMetrics seam

**问题**：`Buffer::copy_count_` / `move_count_` 和 `CountingAllocator` 的四重计数器都是 `inline static` 全局状态。
- 测试必须在每个 `TEST` 开头显式 `reset_counts()`，否则状态泄漏到下一个测试。
- 测试顺序依赖：一个测试忘记 reset，后续测试会失败。
- 测试无法并行运行。
- 这是全局状态污染反模式，破坏了 **locality** 和 **可测试性**。

**解决方案**：
- 创建 `examples/03-modern-cpp/include/instrumentation.hpp`，定义 `OperationMetrics` 类：
  - 非静态、可注入的计数器容器（copy/move/allocation/deallocation/bytes）
  - 内嵌 `Scope` RAII 类：构造时自动 `reset()`，析构无操作
  - `Scope` 不可拷贝/不可移动，确保生命周期明确
- `Buffer` 改造：
  - 删除 `copy_count_`、`move_count_`、`reset_counts()`
  - 构造函数添加 `OperationMetrics* metrics = nullptr` 参数
  - 拷贝/移动构造自动继承源对象的 `metrics_`（也可由调用者显式覆盖）
  - `metrics_ == nullptr` 时零开销
- `CountingAllocator` 改造：
  - 删除所有 `static` 计数器和 `reset_counts()`
  - 构造函数接收 `OperationMetrics*` 并保存到实例中
  - `allocate` / `deallocate` 通过实例指针报告
  - `rebind` 结构体保留，支持 STL 容器要求

**收益**：
- **Locality**：计数行为集中到一处 `OperationMetrics`；修改计数逻辑只需改一个文件。
- **可测试性**：测试之间零耦合，支持并行执行；不再需要手动 `reset_counts()`。
- **零开销**：benchmark 中的 `Buffer` 默认 `metrics = nullptr`，无额外开销。
- **接口成为 seam**：计数是可注入的行为，不是类的固有副作用。

**文件修改**：
- 新增 `examples/03-modern-cpp/include/instrumentation.hpp`
- 修改 `examples/03-modern-cpp/include/buffer.hpp`
- 修改 `examples/03-modern-cpp/include/vector_reserve.hpp`
- 修改 `examples/03-modern-cpp/src/move_semantics.cpp`
- 修改 `examples/03-modern-cpp/src/vector_reserve.cpp`
- 修改 `tests/unit/modern_cpp/modern_cpp_examples_test.cpp`

### 第四轮深化总结

本次深化共：
- 新增约 110 行（instrumentation.hpp）
- 删除约 40 行（静态计数器定义和初始化）
- 修改 6 个文件
- 测试数量保持 84，全部通过

**删除测试验证**：
- 若删除 `OperationMetrics`，计数复杂性会在每个需要追踪的测试/演示处重现——说明 seam 在赚取它的位置。
- 若删除 `Buffer` 或 `CountingAllocator` 的计数能力（即始终 `metrics = nullptr`），它们的核心行为（内存管理/分配）不受影响——说明计数是可分离的横切关注点。

测试验证：
- Debug 构建：84/84 测试通过
- ASan 构建：84/84 测试通过

---

## 2026-05-14 深化（第五轮）—— 候选 2-5 实施

### 19. 提取通用 SIMD 运行时分派器 seam

**问题**：`dispatch_add_arrays` 的分派逻辑（`__builtin_cpu_init`、`__builtin_cpu_supports`）与 `add_arrays` 实现捆绑在 `detail` 命名空间中。当前只有一个分派函数，但如果需要为 `dot_product` 或 `scale_array` 做分派，必须复制整份 CPU 检测逻辑。

**解决方案**：
- 在 `hpc::simd` 命名空间中创建通用模板 `resolve_best<Func>(scalar, sse2, avx2, avx512)`
- 该模板接收四个同类型的函数指针，返回运行时检测到的最佳实现
- `dispatch_add_arrays` 简化为 `resolve_best<Fn>(&scalar, &sse2, &avx2, nullptr)` 的调用
- `detail` 命名空间只保留具体实现，删除重复的 `resolve_add_arrays`

**收益**：
- **Leverage**：一套 CPU 检测逻辑服务任意数量的分派操作
- **Locality**：分派策略集中在一处，新增 SIMD 分派时只需调用 `resolve_best`

**删除测试**：删除 `resolve_best` 会让每个分派操作重复 CPU 检测代码——说明它有深度。

**文件修改**：
- 修改 `examples/04-simd-vectorization/include/simd_utils.hpp`

### 20. 为两个 AlignedAllocator 建立文档 seam

**问题**：两个 `AlignedAllocator` 名称相同但语义不同。当前的区别只在代码注释中，维护者必须跳转两个文件才能理解。

**解决方案**：
- 在 `CONTEXT.md` 中新增两个领域术语：cache-line allocator 和 SIMD-width allocator
- 更新两个头文件的注释，指向 `CONTEXT.md`

**收益**：
- **Locality**：对齐策略的知识集中到一处文档
- **AI 可导航性**：新维护者只需读一个定义即可理解 seam

**文件修改**：
- 修改 `CONTEXT.md`
- 修改 `examples/02-memory-cache/include/memory_utils.hpp`
- 修改 `examples/04-simd-vectorization/include/simd_utils.hpp`

### 21. 标记 ranges_utils.hpp 为教学模块

**问题**：`ranges_utils.hpp` 包含 9 个浅层函数，每个都是标准库算法的薄 wrapper。

**解决方案**：
- 保留所有函数（教学价值不可替代）
- 更新文件头注释，明确标注"teaching example module, not production-ready code"

**收益**：
- 明确模块定位，避免未来的"过度抽象"尝试破坏教学价值

**文件修改**：
- 修改 `examples/03-modern-cpp/include/ranges_utils.hpp`

### 22. 删除 prefetch_* 不可测试的浅层封装

**问题**：`prefetch_read()` 等是编译器内置函数的薄包装。接口≈实现，行为不可观察。

**解决方案**：
- 从 `memory_utils.hpp` 中删除 `prefetch_read`、`prefetch_write`、`prefetch`
- `prefetch.cpp` 示例源码直接使用 `__builtin_prefetch`
- `CONTEXT.md` 中保留"prefetch / 预取"概念定义

**收益**：
- 减少约 47 行不可测试的浅层代码
- **Locality**：预取知识只在文档和示例源码中出现

**删除测试**：删除后复杂性不分散到 N 个调用方——说明是 pass-through。

**文件修改**：
- 修改 `examples/02-memory-cache/include/memory_utils.hpp`
- 修改 `examples/02-memory-cache/src/prefetch.cpp`

### 第五轮深化总结

本次深化共：
- 删除约 50 行
- 新增约 15 行
- 修改 6 个文件
- 测试数量保持 84，全部通过

**删除测试验证**：
- `resolve_best`：删除后每个 SIMD 分派操作需重复 CPU 检测——有深度
- prefetch 包装函数：删除后复杂性不分散——是 pass-through

测试验证：
- Debug 构建：84/84 测试通过
- ASan 构建：84/84 测试通过
- 格式检查：通过
