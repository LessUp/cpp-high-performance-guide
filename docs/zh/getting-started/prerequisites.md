# 先决条件

深入学习本指南前，请确保具备必要的背景知识和工具。

---

## 知识先决条件

### 必需

| 主题 | 水平 | 重要性 |
|------|------|--------|
| **C++ 基础** | 中级 | 类、模板、STL 容器 |
| **指针与内存** | 中级 | 理解内存布局至关重要 |
| **命令行** | 基础 | 构建和运行示例 |
| **计算机体系结构** | 基础 | CPU、内存层次结构、缓存 |

### 有帮助但非必需

| 主题 | 好处 |
|------|------|
| **CMake** | 理解构建系统 |
| **汇编基础** | 阅读编译器输出 |
| **并行编程** | 并发章节 |

---

## 系统要求

### 最低要求

- **操作系统**: Linux (Ubuntu 20.04+, Fedora 35+)、macOS 12+ 或 Windows 10/11
- **CPU**: x86-64 支持 AVX（Intel Haswell/AMD Zen 或更新）
- **内存**: 最低 4 GB，推荐 8 GB
- **磁盘**: 2 GB 可用空间

### 推荐配置（获得最佳体验）

- **操作系统**: Linux（完整性能分析工具支持）
- **CPU**: 支持 AVX-512 的 Intel 或支持 AVX2 的 AMD
- **内存**: 16 GB 或更多
- **编译器**: GCC 12+ 或 Clang 15+

### 检查 CPU 特性

```bash
# Linux - 检查 CPU 特性
grep flags /proc/cpuinfo | head -1

# 查找: sse4_2, avx, avx2, avx512f
```

---

## 工具要求

### 必需工具

| 工具 | 最低版本 | 用途 |
|------|----------|------|
| **CMake** | 3.20 | 构建系统 |
| **C++ 编译器** | GCC 11+ / Clang 14+ / MSVC 2022+ | 编译代码 |
| **Git** | 2.30 | 版本控制 |

### 可选工具（用于性能分析）

| 工具 | 用途 |
|------|------|
| **perf** | Linux 系统分析器 |
| **FlameGraph** | 性能可视化 |
| **Valgrind** | 内存和缓存分析 |
| **Intel VTune** | 高级分析（Intel CPU） |

---

## IDE 和编辑器

### 推荐

- **CLion** - 完整 CMake 集成，优秀的 C++ 支持
- **VS Code** 带 C++ 扩展 - 轻量级、可扩展
- **Visual Studio 2022** - Windows 开发最佳选择

### 包含的配置文件

项目包含以下配置：
- `.vscode/` - VS Code 设置和任务
- `.clang-format` - 代码格式化规则
- `.editorconfig` - 编辑器一致性配置

---

## 学前检查清单

开始学习路径前：

- [ ] 你能编写和编译基础 C++ 程序
- [ ] 你理解 `std::vector`、`std::map` 和基础 STL
- [ ] 你知道什么是指针和引用
- [ ] 你能使用命令行（cd、ls/dir、基础命令）
- [ ] 你安装了支持 C++20 的编译器
- [ ] 你能克隆 Git 仓库

---

## 下一步

准备好了吗？前往[快速开始](quickstart.md)或深入学习[学习路径](../guides/learning-path.md)。

还没准备好？先学习：
- [Learn C++](https://learncpp.com/) - 免费 C++ 教程（英文）
- [CMake 教程](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) - 官方 CMake 指南（英文）
