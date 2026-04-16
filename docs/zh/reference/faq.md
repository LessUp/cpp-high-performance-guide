# 常见问题解答

## 一般问题

### Q: 开始之前需要哪些 C++ 知识？

建议具备：
- 基础 C++ 语法（类、模板、STL 容器）
- 指针和内存管理理解
- 命令行工具使用经验
- 计算机体系结构基础概念（CPU、内存、缓存）

详见 [先决条件](../getting-started/prerequisites.md)。

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

---

## 还有其他问题？

- 🐛 [提交 Issue](https://github.com/LessUp/cpp-high-performance-guide/issues)
- 💬 [发起 Discussion](https://github.com/LessUp/cpp-high-performance-guide/discussions)
- 📧 联系维护者

---

## 相关文档

- [故障排查](troubleshooting.md) - 详细问题解决
- [学习路径](../guides/learning-path.md) - 学习课程
- [性能分析指南](../guides/profiling-guide.md) - 性能分析
