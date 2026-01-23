# C++ 高性能计算优化指南

[![Build](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/build.yml/badge.svg)](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/build.yml)
[![Benchmarks](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/benchmark.yml/badge.svg)](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/benchmark.yml)
[![Sanitizers](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/sanitizers.yml)

面向现代 C++20 的高性能计算优化示例与最佳实践合集。

## 特性

- **现代 CMake 构建系统** - 目标驱动的 CMake、预设、FetchContent 依赖
- **内存与缓存优化** - AOS vs SOA、伪共享、对齐、预取
- **现代 C++ 特性** - constexpr、移动语义、vector reserve、C++20 ranges
- **SIMD 向量化** - 自动向量化、SSE/AVX2/AVX-512 内在函数、封装库
- **并发** - 原子操作、无锁队列、OpenMP 集成
- **基准测试框架** - Google Benchmark 集成、FlameGraph 生成

## 快速开始

### 先决条件

- 支持 C++20 的编译器（GCC 11+、Clang 14+）
- CMake 3.20+
- Ninja（推荐）或 Make

### 构建

```bash
# 克隆仓库
git clone https://github.com/YOUR_USERNAME/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# 配置并构建（Release 模式）
cmake --preset=release
cmake --build build/release

# 运行所有基准测试
cd build/release && ctest --output-on-failure
```

### 可用预设

| 预设 | 说明 |
|------|------|
| `debug` | 带符号信息的调试构建 |
| `release` | 优化构建（-O3, -march=native） |
| `asan` | 启用 AddressSanitizer |
| `tsan` | 启用 ThreadSanitizer |

```bash
# 使用 Sanitizer 构建
cmake --preset=asan
cmake --build build/asan
```

## 项目结构

```
cpp-high-performance-guide/
├── cmake/                      # CMake 模块
│   ├── CompilerOptions.cmake   # 编译选项管理
│   ├── Dependencies.cmake      # FetchContent 依赖
│   ├── Sanitizers.cmake        # Sanitizer 配置
│   └── ExampleTemplate.cmake   # 示例模板
├── examples/
│   ├── 01-cmake-modern/        # CMake 最佳实践 vs 反模式
│   ├── 02-memory-cache/        # 内存与缓存优化
│   ├── 03-modern-cpp/          # 现代 C++ 特性
│   ├── 04-simd-vectorization/  # SIMD 与向量化
│   └── 05-concurrency/         # 并发编程
├── benchmarks/                 # 基准测试工具
├── tests/                      # 单元与性质测试
├── tools/                      # 分析与性能工具
└── docs/                       # 文档
```

## 示例模块

### 01 - 现代 CMake
通过反模式 vs 最佳实践对比学习 CMake。
- 目标驱动 vs 目录驱动的命令
- FetchContent 依赖管理
- 编译选项管理

### 02 - 内存与缓存优化
掌握缓存友好的编程技巧。
- **AOS vs SOA**：数据布局对性能的影响
- **伪共享**：多线程缓存行竞争
- **对齐**：面向 SIMD 的内存对齐
- **预取**：手动预取提示

### 03 - 现代 C++ 特性
用现代 C++ 提升性能。
- **constexpr/consteval**：编译期计算
- **移动语义**：避免不必要的拷贝
- **Vector Reserve**：减少内存分配
- **C++20 Ranges**：现代迭代模式

### 04 - SIMD 向量化
通过 SIMD 最大化吞吐。
- **自动向量化**：编译器友好模式
- **内在函数**：SSE、AVX2、AVX-512 示例
- **SIMD 封装**：可读的 SIMD 抽象

### 05 - 并发
编写高效多线程代码。
- **原子操作**：内存序讲解
- **无锁队列**：SPSC 队列实现
- **OpenMP**：简单并行化模式

## 运行基准测试

```bash
# 运行全部基准测试
cd build/release
ctest --output-on-failure

# 运行指定基准
./examples/02-memory-cache/bench/aos_soa_bench

# 导出 JSON 结果
./examples/02-memory-cache/bench/aos_soa_bench --benchmark_format=json > results.json
```

## 性能分析

生成 FlameGraph 可视化：

```bash
# 录制性能数据
./tools/flamegraph/generate_flamegraph.sh ./build/release/examples/02-memory-cache/bench/aos_soa_bench

# 查看生成的 SVG
firefox flamegraph.svg
```

## 文档

- [学习路径](docs/zh/learning-path.md) - 推荐的学习顺序
- [性能分析指南](docs/zh/profiling-guide.md) - 如何进行性能剖析与分析

## 贡献

1. Fork 仓库
2. 创建功能分支
3. 确保 Sanitizer 下测试通过
4. 提交 Pull Request

## 许可证

MIT License - 详见 [LICENSE](LICENSE)。

## 致谢

- [Google Benchmark](https://github.com/google/benchmark)
- [Google Test](https://github.com/google/googletest)
- [RapidCheck](https://github.com/emil-e/rapidcheck)
- [FlameGraph](https://github.com/brendangregg/FlameGraph)
