# Domain Glossary

本文档定义 C++ 高性能指南项目的核心领域术语，统一中英文表达。

## Memory Optimization / 内存优化

### cache-line alignment / 缓存行对齐
数据对齐到 CPU 缓存行边界（通常 64 字节，某些 ARM 系统为 128 字节）。
目的：避免跨缓存行访问导致的性能下降。

### false sharing / 伪共享
多线程访问同一缓存行的不同变量时，由于缓存一致性协议导致的性能下降。
解决方案：使用缓存行填充（padding）将变量隔离到不同缓存行。

### AOS/SOA / 结构数组 vs 数组结构
- **AOS (Array of Structures)**: `struct { float x, y, z; } particles[N]`
- **SOA (Structure of Arrays)**: `struct { float x[N], y[N], z[N]; } particles`

SOA 布局更利于 SIMD 向量化，AOS 布局更符合面向对象思维。

### memory alignment / 内存对齐
数据地址是其大小的整数倍。对齐访问通常比非对齐访问更快。

### prefetch / 预取
在数据实际使用前将其加载到缓存，隐藏内存延迟。

## Concurrency / 并发

### lock-free / 无锁
不使用互斥锁的并发数据结构，通过原子操作实现线程安全。
特点：即使某个线程被挂起，其他线程仍能继续执行。

### memory ordering / 内存序
原子操作的内存可见性顺序：
- `relaxed`: 无同步，仅保证原子性
- `acquire/release`: 建立同步点
- `seq_cst`: 顺序一致性（最强保证）

### SPSC / MPMC
- **SPSC (Single Producer Single Consumer)**: 单生产者单消费者队列
- **MPMC (Multiple Producer Multiple Consumer)**: 多生产者多消费者队列

SPSC 实现更简单，性能更高；MPMC 更通用但实现复杂。

### atomic / 原子操作
不可分割的操作，要么完全执行，要么完全不执行。
C++ 中通过 `std::atomic` 模板提供。

### spinlock / 自旋锁
线程在获取锁失败时循环等待（忙等待），而不是让出 CPU。
适用于短时间持锁场景。

## SIMD / 单指令多数据

### SIMD (Single Instruction Multiple Data)
一条指令同时处理多个数据元素。
x86 平台：SSE (128-bit), AVX (256-bit), AVX-512 (512-bit)
ARM 平台：NEON (128-bit)

### vectorization / 向量化
编译器自动或手动使用 SIMD 指令优化循环。
- 自动向量化：编译器分析循环并生成 SIMD 代码
- 手动向量化：使用 intrinsics 或 SIMD 包装库

### intrinsic
编译器提供的底层函数，直接映射到特定 CPU 指令。
例：`_mm_add_ps` 对应 SSE 的 addps 指令。

### SIMD width / 向量宽度
一次 SIMD 操作处理的数据量：
- SSE: 4 个 float (128-bit)
- AVX: 8 个 float (256-bit)
- AVX-512: 16 个 float (512-bit)

## Build System / 构建系统

### preset-driven build / 预设驱动构建
使用 CMakePresets.json 统一构建配置，确保本地和 CI 环境一致。

### sanitizer / 消毒器
运行时检测工具，发现未定义行为和内存错误：
- **ASAN (AddressSanitizer)**: 检测内存错误（越界、释放后使用等）
- **TSAN (ThreadSanitizer)**: 检测数据竞争
- **UBSAN (UndefinedBehaviorSanitizer)**: 检测未定义行为
- **MSAN (MemorySanitizer)**: 检测未初始化内存读取

### header-only library / 仅头文件库
只需包含头文件即可使用的库，无需编译链接。
优点：集成简单；缺点：可能增加编译时间。

## Performance Measurement / 性能测量

### benchmark / 基准测试
测量代码执行时间的标准化方法。
关键要素：预热、多次运行、统计中位数/百分位数。

### flame graph / 火焰图
可视化程序执行时间分布的工具，显示调用栈和时间占比。

### cache miss / 缓存未命中
CPU 在缓存中未找到需要的数据，必须从主内存加载。
缓存未命中是性能瓶颈的常见原因。

### throughput / 吞吐量
单位时间内处理的操作数或数据量。通常以 ops/sec、MB/s 表示。

### latency / 延迟
单个操作从开始到完成的时间。通常以纳秒、微秒表示。
