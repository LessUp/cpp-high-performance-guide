# 故障排查指南

本指南帮助你解决构建、运行或分析 C++ 高性能指南示例时遇到的常见问题。

---

## 构建失败

### CMake 找不到编译器

**症状：**
```
CMake Error: Could not find a package configuration file provided by "compiler"
```

**解决方案：**

1. 验证编译器安装：
```bash
g++ --version  # 应显示 GCC 11+ 或 Clang 14+
```

2. 如果未找到，安装编译器：
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential g++-11

# Fedora/RHEL
sudo dnf install gcc-c++ gcc

# macOS with Homebrew
brew install gcc
```

3. 如果安装了多个编译器，明确指定：
```bash
export CC=gcc-11
export CXX=g++-11
cmake --preset=release --fresh
```

---

### 线程链接错误

**症状：**
```
undefined reference to `pthread_create'
```

**解决方案：**

1. 安装 pthread 开发文件：
```bash
sudo apt-get install libpthread-stubs0-dev
```

2. 使用全新缓存重新配置：
```bash
cmake --preset=release --fresh
```

### Sanitizer 链接错误

**症状：**
```
/usr/bin/ld: cannot find -lasan
```

**解决方案：**

安装 sanitizer 库：
```bash
# Ubuntu/Debian
sudo apt-get install libasan6 libtsan0 libubsan1

# Fedora
sudo dnf install libasan libtsan libubsan
```

注意：ASan 和 TSan 互斥，不要同时使用。

---

## 运行时问题

### perf 权限被拒绝

**症状：**
```
Error:
Access to performance monitoring and observability operations is limited.
```

**解决方案：**

**选项 1**：使用 sudo 运行（不建议常规使用）
```bash
sudo perf record -g ./benchmark
```

**选项 2**：调整内核参数（临时）
```bash
# 允许用户使用 perf
echo 1 | sudo tee /proc/sys/kernel/perf_event_paranoid

# 永久生效
echo 'kernel.perf_event_paranoid = 1' | sudo tee /etc/sysctl.d/99-perf.conf
sudo sysctl --system
```

**选项 3**：将用户添加到 perf 组（推荐）
```bash
sudo groupadd perf_users
sudo usermod -a -G perf_users $USER
# 注销并重新登录
```

---

### 基准测试结果太慢

**症状：** 基准测试结果比预期慢 10 倍或更多。

**诊断步骤：**

1. **检查构建类型**：
```bash
# Release 模式应显示 -O3
cmake --build build/release --verbose | grep -- -O
```

2. **检查 CPU 电源策略**：
```bash
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# 应为 "performance"，而非 "powersave"
```

3. **设置为性能模式**：
```bash
sudo apt-get install linux-tools-common
sudo cpupower frequency-set --governor performance
```

---

## 平台特定问题

### macOS: perf 不可用

**解释：** macOS 没有 Linux perf 工具。

**替代方案：**
- 使用 Instruments（Xcode 的 GUI 工具）
- 使用 `sample` 命令行工具
- 在 Docker/Linux VM 中运行分析

### Windows: 构建问题

**解释：** Windows 支持是尽力而为。

**建议：**
1. 使用 WSL2 获得完整 Linux 兼容性
2. 或使用支持 C++20 的 Visual Studio 2022
3. 某些示例（OpenMP、perf）可能在原生 Windows 上无法工作

---

## 获取帮助

如果这里未能解决你的问题：

1. **搜索现有问题**：[GitHub Issues](https://github.com/LessUp/cpp-high-performance-guide/issues)
2. **查看讨论区**：[GitHub Discussions](https://github.com/LessUp/cpp-high-performance-guide/discussions)
3. **创建新问题**，包含：
   - 你的操作系统和版本
   - 编译器版本（`g++ --version`）
   - CMake 版本（`cmake --version`）
   - 完整错误信息
   - 复现步骤

---

## 相关文档

- [常见问题](faq.md) - 快速解答
- [安装指南](../getting-started/installation.md) - 安装帮助
- [性能分析指南](../guides/profiling-guide.md) - 性能分析
