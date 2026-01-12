# CLAUDE.md

本文档为Claude Code (claude.ai/code) 在此代码库中工作提供指导。

## 项目概述

这是一个现代C++20高性能计算优化示例集合项目，包含以下核心特性：
- 现代CMake构建系统（CMake 3.20+，支持预设和FetchContent）
- 内存与缓存优化示例（AOS/SOA、伪共享、内存对齐、预取）
- 现代C++特性优化（constexpr、移动语义、vector预留、C++20 ranges）
- SIMD向量化（自动向量化、SSE/AVX2/AVX-512 intrinsic）
- 并发编程（原子操作、无锁队列、OpenMP）
- 基于Google Benchmark的基准测试框架
- FlameGraph性能可视化

## 常用开发命令

### 构建系统

```bash
# 配置项目（Debug模式）
cmake --preset=debug

# 配置项目（Release模式，优化级别 -O3 -march=native）
cmake --preset=release

# 配置项目（带调试信息的发布版）
cmake --preset=relwithdebinfo

# 配置项目（AddressSanitizer）
cmake --preset=asan

# 配置项目（ThreadSanitizer）
cmake --preset=tsan

# 配置项目（UndefinedBehaviorSanitizer）
cmake --preset=ubsan

# 配置项目（代码覆盖率）
cmake --preset=coverage

# 构建项目
cmake --build build/release

# 或使用ninja构建
ninja -C build/release
```

### 运行测试和基准测试

```bash
# 运行所有测试（Debug模式）
cmake --build build/debug
ctest --preset=debug

# 运行所有测试（Release模式）
cmake --build build/release
ctest --preset=release

# 运行特定测试
ctest -R memory --output-on-failure

# 运行所有基准测试
cd build/release && ctest --output-on-failure

# 运行特定基准测试
./build/release/examples/02-memory-cache/bench/aos_soa_bench

# 以JSON格式导出基准测试结果
./build/release/examples/02-memory-cache/bench/aos_soa_bench --benchmark_format=json > results.json

# 运行带统计的基准测试
./bench --benchmark_repetitions=10 --benchmark_report_aggregates_only=true
```

### 性能分析

```bash
# 使用perf进行CPU分析
perf record -g ./build/release/examples/02-memory-cache/bench/aos_soa_bench
perf report

# 缓存miss分析
perf stat -e cache-references,cache-misses,L1-dcache-load-misses ./bench

# 分支预测分析
perf stat -e branches,branch-misses ./bench

# 生成FlameGraph可视化
./tools/flamegraph/generate_flamegraph.sh ./build/release/examples/02-memory-cache/bench/aos_soa_bench

# 使用Valgrind Cachegrind分析缓存
valgrind --tool=cachegrind ./build/release/examples/02-memory-cache/bench/aos_soa_bench

# 使用Valgrind Callgrind分析调用图
valgrind --tool=callgrind ./build/release/examples/02-memory-cache/bench/aos_soa_bench
kcachegrind callgrind.out.*
```

### 编译向量化报告

```bash
# GCC向量化报告
g++ -O3 -march=native -fopt-info-vec-optimized your_code.cpp

# Clang向量化报告
clang++ -O3 -march=native -Rpass=loop-vectorize your_code.cpp
```

### 清理和重新构建

```bash
# 清理构建目录
rm -rf build/*

# 完全重新配置和构建
cmake --preset=release --fresh
cmake --build build/release
```

## 项目架构

### 目录结构

```
cpp-high-performance-guide/
├── cmake/                      # CMake模块
│   ├── CompilerOptions.cmake   # 编译器选项管理
│   ├── Dependencies.cmake      # FetchContent依赖管理
│   ├── Sanitizers.cmake        #  sanitizer配置
│   └── ExampleTemplate.cmake    # 示例模块模板
├── examples/                   # 优化示例（5个模块）
│   ├── 01-cmake-modern/       # CMake最佳实践 vs 反模式
│   ├── 02-memory-cache/       # 内存和缓存优化
│   ├── 03-modern-cpp/         # 现代C++特性
│   ├── 04-simd-vectorization/ # SIMD和向量化
│   └── 05-concurrency/         # 并发编程
├── benchmarks/                 # 基准测试工具
├── tests/                     # 单元测试和属性测试
├── tools/                     # 性能分析工具
│   ├── flamegraph/           # FlameGraph生成脚本
│   └── analysis/              # 分析工具
├── docs/                      # 文档
└── .kiro/specs/               # 项目规格文档
```

### 构建系统架构

项目使用现代CMake，采用以下设计原则：
- **基于目标的命令**：使用`target_include_directories`、`target_link_libraries`而非目录级命令
- **FetchContent依赖管理**：自动下载Google Benchmark、Google Test、RapidCheck
- **CMake预设**：支持多种构建配置（debug、release、asan、tsan等）
- **C++20标准**：强制要求C++20，支持编译器自动检测

核心CMake模块：
1. **Dependencies.cmake**：管理外部依赖，使用FetchContent
2. **CompilerOptions.cmake**：统一编译器选项管理
3. **Sanitizers.cmake**：集成AddressSanitizer、ThreadSanitizer、UBSan
4. **ExampleTemplate.cmake**：标准化新示例模块的CMakeLists.txt模板

### 示例模块架构

每个示例模块遵循统一结构：
```
examples/XX-category/
├── src/                       # 源文件
│   ├── *.cpp                 # 优化示例
├── bench/                     # 基准测试
│   └── *_bench.cpp          # Google Benchmark基准测试
├── CMakeLists.txt            # 模块级构建配置
└── README.md                # 模块说明（可选）
```

示例模块分类：
1. **01-cmake-modern**：CMake最佳实践教育
2. **02-memory-cache**：AOS vs SOA、伪共享、内存对齐、预取
3. **03-modern-cpp**：constexpr、移动语义、vector预留、C++20 ranges
4. **04-simd-vectorization**：自动向量化、SSE/AVX2/AVX-512 intrinsic
5. **05-concurrency**：原子操作、无锁队列、OpenMP

### 依赖管理

项目自动下载以下依赖（通过FetchContent）：
- **Google Benchmark v1.8.3**：性能基准测试
- **Google Test v1.14.0**：单元测试框架
- **RapidCheck**：基于属性的测试

### 测试架构

- **单元测试**：使用Google Test
- **属性测试**：使用RapidCheck进行随机属性验证
- **基准测试**：所有示例都有对应的Google Benchmark测试
- **代码覆盖率**：支持gcov代码覆盖率分析

### 性能分析工具集成

- **FlameGraph**：`./tools/flamegraph/generate_flamegraph.sh`脚本自动生成火焰图
- **benchmark_compare.py**：自动比较基准测试结果
- **perf支持**：文档包含完整的perf使用指南
- **Valgrind集成**：支持Cachegrind和Callgrind

## 关键开发信息

### 学习路径

参考`docs/learning-path.md`，推荐学习顺序：
1. Phase 1: 构建系统基础（CMake最佳实践）
2. Phase 2: 内存基础（AOS vs SOA、内存对齐）
3. Phase 3: 现代C++性能特性
4. Phase 4: SIMD向量化
5. Phase 5: 并发编程
6. Phase 6: 性能分析与基准测试

### 添加新示例模块

使用`cmake/ExampleTemplate.cmake`作为模板：
1. 复制模板目录结构
2. 修改CMakeLists.txt中的模块名称
3. 添加源代码和基准测试
4. 在主CMakeLists.txt中添加新子目录

### 性能优化原则

1. **测量优先**：使用基准测试验证优化效果
2. **编译器友好**：编写利于编译器优化的代码
3. **缓存友好**：优化数据布局和访问模式
4. **向量化友好**：确保循环可向量化
5. **并发安全**：避免伪共享，使用原子操作

### 文档资源

- **README.md**：项目概述和快速开始
- **docs/learning-path.md**：推荐学习顺序
- **docs/profiling-guide.md**：详细的性能分析指南
- **.kiro/specs/requirements.md**：项目需求规格说明

### 编译器要求

- C++20兼容编译器（GCC 11+、Clang 14+）
- CMake 3.20+
- Ninja（推荐）或Make
- 支持OpenMP（可选）

### 重要配置选项

CMake变量：
- `HPC_BUILD_TESTS`：构建测试（默认ON）
- `HPC_BUILD_BENCHMARKS`：构建基准测试（默认ON）
- `HPC_ENABLE_OPENMP`：启用OpenMP支持（默认ON）

编译器优化标志（Release模式）：
- `-O3`：最高优化级别
- `-march=native`：使用当前CPU的所有指令集
- `-ffast-math`：快速数学运算（适当场景）
