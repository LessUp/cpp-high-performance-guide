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

### FetchContent 下载失败

**症状：**
```
FetchContent failed to populate benchmark
```

**解决方案：**

1. **网络问题**: 检查网络连接和代理设置
```bash
# 测试连通性
curl -I https://github.com

# 如果使用代理
export HTTP_PROXY=http://proxy:port
export HTTPS_PROXY=http://proxy:port
```

2. **Git 未找到**: 确保安装了 Git
```bash
sudo apt-get install git  # Ubuntu/Debian
```

3. **SSL 证书问题**：
```bash
# 更新证书
sudo apt-get install ca-certificates
# 或临时禁用 SSL 验证（临时解决方案）
git config --global http.sslVerify false
```

4. **改用系统包**（如果可用）：
```bash
# Ubuntu/Debian
sudo apt-get install libbenchmark-dev libgtest-dev
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

---

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

4. **禁用 CPU boost**（为了一致的结果）：
```bash
echo 0 | sudo tee /sys/devices/system/cpu/cpufreq/boost
```

5. **绑定到特定 CPU**：
```bash
taskset -c 0 ./benchmark
```

6. **检查是否热节流**：
```bash
# 基准测试期间监控温度
watch -n1 sensors
```

---

### 结果方差大

**症状：** 基准测试结果在各次运行间差异显著。

**解决方案：**

1. **禁用 ASLR**：
```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
# 运行基准测试
# 完成后重新启用：echo 2 | sudo tee /proc/sys/kernel/randomize_va_space
```

2. **关闭其他应用程序**

3. **使用基准重复**：
```bash
./benchmark --benchmark_repetitions=10 --benchmark_report_aggregates_only=true
```

4. **禁用超线程**（最一致的结果）：
```bash
# 禁用逻辑 CPU
echo 0 | sudo tee /sys/devices/system/cpu/cpuX/online
```

---

## 测试失败

### ThreadSanitizer 误报

**症状：** TSan 报告系统库或第三方代码中的数据竞争。

**解决方案：** 使用抑制文件：
```bash
# 创建 suppressions.txt
race:libpthread
race:libstdc++

# 使用抑制运行
TSAN_OPTIONS="suppressions=suppressions.txt" ./test
```

---

### AddressSanitizer 内存不足

**症状：** ASan 测试因 OOM 失败。

**解决方案：** ASan 增加内存使用。可以：
1. 在内存更大的机器上运行
2. 减少测试数据大小
3. 对那些测试改用 debug 构建

---

## 分析工具问题

### FlameGraph 为空或不完整

**症状：** 生成的 SVG 没有数据或非常小。

**解决方案：**

1. **确保存在调试符号**：
```bash
# 使用调试信息构建
cmake --preset=relwithdebinfo
```

2. **让基准测试运行足够长时间**：
```bash
./benchmark --benchmark_min_time=5
```

3. **检查 perf record 输出**：
```bash
perf record -g ./benchmark
perf report  # 查看是否捕获了样本
```

4. **增加采样频率**：
```bash
perf record -F 999 -g ./benchmark
```

---

### Valgrind 太慢

**症状：** Cachegrind/Callgrind 运行时间过长。

**解释：** Valgrind 插桩比原生慢 10-50 倍。

**解决方案：**

1. Valgrind 运行使用较小数据集
2. 快速分析改用 perf
3. Valgrind 只运行在特定测试上，非完整基准测试套件

---

## 平台特定问题

### macOS: perf 不可用

**解释：** macOS 没有 Linux perf 工具。

**替代方案：**
- 使用 Instruments（Xcode 的 GUI 工具）
- 使用 `sample` 命令行工具
- 在 Docker/Linux VM 中运行分析

---

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

## 调试清单

报告问题前，请验证：

- [ ] 使用支持的编译器（GCC 11+、Clang 14+）
- [ ] CMake 3.22 或更新版本
- [ ] 使用全新构建目录（使用了 `--fresh` 标志）
- [ ] 所有依赖已安装
- [ ] 网络连通性（用于 FetchContent）
- [ ] 足够的磁盘空间
- [ ] 足够的内存（特别是 sanitizer）
