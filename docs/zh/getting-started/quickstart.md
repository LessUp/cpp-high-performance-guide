# 快速开始指南

在5分钟内启动并运行 C++ 高性能指南。

---

## 先决条件

开始前请确保具备：

- **编译器**: GCC 11+、Clang 14+ 或 MSVC 2022+
- **CMake**: 3.20 或更高版本
- **Git**: 用于克隆仓库

验证你的编译器：
```bash
g++ --version  # 应显示 11.0 或更高版本
clang++ --version  # 应显示 14.0 或更高版本
```

## 安装步骤

### 1. 克隆仓库

```bash
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide
```

### 2. 配置项目

使用 CMake 预设实现可复现构建：

```bash
# Release 构建（性能优化）
cmake --preset=release

# 或 Debug 构建（开发调试用）
cmake --preset=debug
```

### 3. 构建

```bash
cmake --build build/release
```

### 4. 运行测试

```bash
ctest --preset=release
```

### 5. 运行基准测试

```bash
./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

---

## 可用构建预设

| 预设 | 描述 | 用途 |
|------|------|------|
| `release` | -O3, march=native | 性能测量 |
| `debug` | -O0, -g | 开发和调试 |
| `relwithdebinfo` | -O2, -g | 带符号的性能分析 |
| `asan` | AddressSanitizer | 内存错误检测 |
| `tsan` | ThreadSanitizer | 竞争条件检测 |
| `ubsan` | UndefinedBehaviorSanitizer | 未定义行为检测 |
| `coverage` | gcov | 代码覆盖率分析 |

---

## 下一步

1. **[学习路径](../guides/learning-path.md)** - 跟随 6 周结构化课程
2. **[第一个示例](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/01-cmake-modern)** - 探索现代 CMake
3. **[性能分析指南](../guides/profiling-guide.md)** - 学习性能测量

---

## 故障排查

### "CMake 找不到编译器"

安装 C++20 兼容的编译器：

```bash
# Ubuntu/Debian
sudo apt-get install build-essential g++-11

# Fedora
sudo dnf install gcc-c++

# macOS
brew install gcc
```

### "FetchContent 失败"

检查网络连接或使用系统包：

```bash
# Ubuntu/Debian
sudo apt-get install libbenchmark-dev libgtest-dev
```

更多问题请参阅[故障排查指南](../reference/troubleshooting.md)。
