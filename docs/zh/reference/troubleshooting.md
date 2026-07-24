# 故障排查

构建、运行或分析示例时遇到问题的快速解决方案。

---

## 构建问题

### CMake 找不到编译器

**症状：**

```
CMake Error: Could not find a package configuration file provided by "compiler"
```

**解决：**

```bash
# 验证编译器
g++ --version  # 需要 GCC 11+ 或 Clang 14+

# Ubuntu/Debian
sudo apt-get update && sudo apt-get install build-essential g++-11

# Fedora/RHEL
sudo dnf install gcc-c++ gcc

# macOS
brew install gcc
```

安装了多个编译器时，明确指定：

```bash
export CC=gcc-11 CXX=g++-11
cmake --preset=release --fresh
```

### FetchContent 下载失败

**症状：**

```
CMake Error: Failed to download ...
fatal: unable to access 'https://github.com/google/benchmark.git'
```

**解决：**

1. 检查网络连接和代理设置：

```bash
# 如果处于代理环境
export https_proxy=http://proxy.example.com:8080
export http_proxy=http://proxy.example.com:8080
```

2. 使用国内镜像或手动克隆依赖：

```bash
# 手动放置依赖到 _deps 目录
git clone https://gitee.com/mirrors/google-benchmark.git build/release/_deps/googlebenchmark-src
```

3. 使用 `FETCHCONTENT_FULLY_DISCONNECTED` 跳过下载（需提前准备好源码）：

```bash
cmake --preset=release -DFETCHCONTENT_FULLY_DISCONNECTED=ON
```

### 线程链接错误

**症状：**

```
undefined reference to `pthread_create'
```

**解决：**

```bash
# Ubuntu/Debian
sudo apt-get install libpthread-stubs0-dev

# 清除缓存重新配置
cmake --preset=release --fresh
```

### Sanitizer 链接错误

**症状：**

```
/usr/bin/ld: cannot find -lasan
```

**解决：**

```bash
# Ubuntu/Debian
sudo apt-get install libasan6 libtsan0 libubsan1

# Fedora
sudo dnf install libasan libtsan libubsan
```

> ASan 和 TSan 互斥，不要同时启用。

---

## 运行时问题

### perf 权限被拒绝

**症状：**

```
Error: Access to performance monitoring and observability operations is limited.
```

**方案 1**：调整内核参数（推荐）

```bash
# 临时生效
echo 1 | sudo tee /proc/sys/kernel/perf_event_paranoid

# 永久生效
echo 'kernel.perf_event_paranoid = 1' | sudo tee /etc/sysctl.d/99-perf.conf
sudo sysctl --system
```

**方案 2**：将用户加入 perf 组

```bash
sudo groupadd perf_users
sudo usermod -a -G perf_users $USER
# 注销并重新登录
```

**方案 3**：使用 sudo（不建议常规使用）

```bash
sudo perf record -g ./benchmark
```

### 基准测试结果太慢

**症状：** 结果比预期慢 10 倍以上。

**排查步骤：**

1. **确认 Release 构建**：

```bash
cmake --build build/release --verbose 2>&1 | grep -- -O
# 应看到 -O3 或 -O2
```

2. **检查 CPU 电源策略**：

```bash
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# 应为 "performance"，而非 "powersave"
```

3. **切换到性能模式**：

```bash
sudo apt-get install linux-tools-common
sudo cpupower frequency-set --governor performance
```

4. **减少后台负载**：关闭浏览器、IDE 索引等高 CPU 占用进程，避免在虚拟机或共享服务器上跑基准测试。

### macOS 无 perf

macOS 没有 Linux `perf` 工具。替代方案：

- **Instruments**：Xcode 自带的 GUI 性能分析工具（Time Profiler、Counters 等模板）
- **`sample` 命令**：`sample <pid> 3` 采样 3 秒的调用栈
- **Docker / Linux VM**：在容器或虚拟机中运行 `perf`

---

## 常见问题

### 需要哪些 C++ 知识？

- C++17/20 基础：模板、`constexpr`、`std::optional`、结构化绑定
- 基本了解 CMake 构建流程
- 不需要提前掌握 SIMD intrinsics 或并发原语——示例会从基础讲起

### 支持哪些操作系统和编译器？

| 平台 | 状态 |
|------|------|
| Linux (GCC 11+, Clang 14+) | 完整支持 |
| macOS (Homebrew GCC / AppleClang) | 大部分支持，`perf` 不可用 |
| Windows (MSVC 2022 / WSL2) | 尽力支持，推荐 WSL2 |

构建要求：CMake 3.20+、Ninja。

### 为什么基准测试结果与别人不同？

常见原因：

- **CPU 型号不同**：不同微架构的缓存大小、SIMD 宽度、分支预测器差异显著
- **电源策略**：`powersave` 模式下 CPU 频率被限制
- **后台负载**：其他进程争抢 CPU 和缓存
- **编译器版本和标志**：不同版本的自动向量化能力不同
- **构建类型**：Debug 构建（`-O0`）与 Release（`-O3`）可差 10-100 倍

建议在同一台机器上对比 baseline 和 optimized 版本，而非跨机器比较绝对数值。

---

## 获取帮助

1. **搜索现有问题**：[GitHub Issues](https://github.com/LessUp/cpp-high-performance-guide/issues)
2. **参与讨论**：[GitHub Discussions](https://github.com/LessUp/cpp-high-performance-guide/discussions)
3. **创建新 Issue**，请包含：
   - 操作系统和版本
   - 编译器版本（`g++ --version`）
   - CMake 版本（`cmake --version`）
   - 完整错误信息
   - 复现步骤
