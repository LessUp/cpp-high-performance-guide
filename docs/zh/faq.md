# 常见问题解答

## 一般问题

### Q: 开始之前需要哪些 C++ 知识？

建议具备：
- 基础 C++ 语法（类、模板、STL 容器）
- 指针和内存管理理解
- 命令行工具使用经验
- 计算机体系结构基础概念（CPU、内存、缓存）

### Q: 支持哪些操作系统？

项目主要在 **Linux** 上测试。macOS 和 Windows 尽力支持：
- Linux：完整支持（推荐使用性能分析工具）
- macOS：构建和测试正常；部分分析工具可能有差异
- Windows：支持 MSVC 或 WSL；部分 Unix 专用工具不可用

### Q: 支持哪些编译器？

- **GCC 11+**（完全支持）
- **Clang 14+**（完全支持）
- **MSVC**（尽力支持，部分 C++20 特性可能有差异）

### Q: 为什么我的基准测试结果与文档中的不同？

性能取决于多个因素：
- **CPU 架构**：不同代际有不同优化
- **内存速度**：DDR4 vs DDR5，单通道 vs 双通道
- **编译器版本**：新版本可能优化更好
- **后台进程**：关闭其他应用以获得一致结果
- **电源设置**：Linux 上使用 "performance" 电源策略

建议多次运行基准测试并报告统计汇总结果。

---

## 构建问题

### Q: CMake 配置失败，提示 "Could not find compiler"

确保安装了 C++20 兼容的编译器：

```bash
# Ubuntu/Debian
sudo apt-get install build-essential g++-11

# Fedora
sudo dnf install gcc-c++

# macOS
xcode-select --install
# 或
brew install gcc
```

### Q: "FetchContent failed to download dependencies"

检查网络连接。如果使用代理：

```bash
export HTTP_PROXY=http://proxy.example.com:8080
export HTTPS_PROXY=http://proxy.example.com:8080
cmake --preset=release
```

或使用预下载的依赖离线构建。

### Q: 构建失败，提示 "undefined reference to `pthread_create`"

确保 CMake 能找到线程库：

```bash
# Ubuntu/Debian
sudo apt-get install libpthread-stubs0-dev

# 使用全新缓存重新配置
cmake --preset=release --fresh
```

### Q: Sanitizer 构建链接失败

某些 sanitizer 互斥。同时确保安装了 sanitizer 库：

```bash
# Ubuntu/Debian
sudo apt-get install libasan6 libtsan0 libubsan1
```

---

## 运行时问题

### Q: 基准测试运行很慢

可能原因：
1. **Debug 构建**：性能测量使用 `release` 预设
2. **CPU 降频**：设置电源策略为 performance：
   ```bash
   sudo cpupower frequency-set --governor performance
   ```
3. **后台负载**：关闭其他应用程序
4. **虚拟机**：VM 可能性能不稳定

### Q: 运行 perf 时提示 "Permission denied"

`perf` 需要适当的权限：

```bash
# 选项 1：使用 sudo 运行
sudo perf record ./your_benchmark

# 选项 2：调整 perf_event_paranoid
echo 1 | sudo tee /proc/sys/kernel/perf_event_paranoid

# 选项 3：将用户添加到 perf 组
sudo usermod -a -G perf $USER
# 注销并重新登录使更改生效
```

### Q: FlameGraph 生成失败

确保安装了所需工具：

```bash
# 安装 perf (linux-tools)
sudo apt-get install linux-tools-common linux-tools-generic

# 项目已在 tools/performance/ 包含 FlameGraph 脚本
# 无需额外安装
```

---

## 测试问题

### Q: ThreadSanitizer 构建的测试失败

ThreadSanitizer 可能报告第三方库的数据竞争。如果是你的代码中的竞争：

1. 仔细阅读 TSan 报告
2. 检查是否缺少原子操作
3. 验证内存序是否正确
4. 参考 [C++ Concurrency in Action](https://www.manning.com/books/c-plus-plus-concurrency-in-action)

### Q: AddressSanitizer 报告内存泄漏

ASan 会跟踪分配。如果报告泄漏：

1. 检查是在你的代码还是依赖库中
2. 使用 `ASAN_OPTIONS=detect_leaks=0` 临时禁用泄漏检查
3. 修复你代码中的实际泄漏

---

## 文档问题

### Q: 哪里可以找到更多学习资源？

- **书籍**："C++ Concurrency in Action"、"Effective Modern C++"
- **在线资源**：[CppReference](https://zh.cppreference.com/)、[Compiler Explorer](https://godbolt.org/)
- **论文**：[What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)

### Q: 如何贡献一个新的示例模块？

详见 [贡献指南](../../CONTRIBUTING.zh.md)。快速步骤：

1. 在 `examples/XX-topic-name/` 下创建目录
2. 使用标准结构（src/、bench/、CMakeLists.txt、README.md）
3. 在 `tests/` 中添加测试
4. 提交 PR

### Q: 我可以在我的项目中使用这些代码吗？

可以！本项目采用 MIT 许可证。你可以自由使用、修改和分发代码。详见 [LICENSE](../../LICENSE)。

---

## 还有其他问题？

- 🐛 [提交 Issue](https://github.com/LessUp/cpp-high-performance-guide/issues)
- 💬 [发起 Discussion](https://github.com/LessUp/cpp-high-performance-guide/discussions)
- 📧 联系维护者
