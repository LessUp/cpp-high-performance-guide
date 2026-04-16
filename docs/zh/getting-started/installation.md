# 安装指南

所有支持平台的完整安装说明。

---

## 目录

- [Linux (Ubuntu/Debian)](#linux-ubuntudebian)
- [Linux (Fedora/RHEL)](#linux-fedorarhel)
- [macOS](#macos)
- [Windows (WSL2)](#windows-wsl2)
- [Windows (原生)](#windows-原生)

---

## Linux (Ubuntu/Debian)

### 系统依赖

```bash
# 更新包列表
sudo apt-get update

# 安装构建工具
sudo apt-get install -y build-essential cmake git

# 安装 C++20 编译器
sudo apt-get install -y g++-11 clang-14

# 安装性能分析工具（可选）
sudo apt-get install -y linux-tools-common linux-tools-generic
sudo apt-get install -y valgrind perf-tools-unstable

# 安装 sanitizer 库（可选）
sudo apt-get install -y libasan6 libtsan0 libubsan1
```

### 从源码构建

```bash
# 克隆仓库
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# 配置并构建
cmake --preset=release
cmake --build build/release

# 运行测试
ctest --preset=release
```

---

## Linux (Fedora/RHEL)

### 系统依赖

```bash
# 安装开发工具
sudo dnf groupinstall "Development Tools"

# 安装 C++20 编译器
sudo dnf install gcc-c++ clang cmake git

# 安装性能分析工具
sudo dnf install perf valgrind

# 安装 sanitizer 库
sudo dnf install libasan libtsan libubsan
```

### 从源码构建

与 Ubuntu/Debian 相同。

---

## macOS

### 使用 Homebrew

```bash
# 如未安装 Homebrew，先安装
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install cmake git
brew install gcc  # 或使用 Xcode Clang
```

### 使用 Xcode

从 App Store 安装 Xcode，然后：

```bash
xcode-select --install
```

### 配置说明

macOS 默认使用 `clang++`。某些 Linux 专用性能工具（如 `perf`）在 macOS 上不可用。可以使用 Xcode 自带的 Instruments 作为替代。

```bash
# 使用默认编译器（Clang）构建
cmake --preset=release
cmake --build build/release
```

---

## Windows (WSL2)

WSL2 为 Windows 用户提供最佳体验。

### 安装 WSL2

```powershell
# 在 PowerShell（管理员）中
wsl --install
```

然后在 WSL2 内遵循上面的 Linux (Ubuntu) 说明。

---

## Windows (原生)

### 要求

- Visual Studio 2022 带 C++ 工作负载
- CMake 3.20+（VS2022 已包含）
- Git for Windows

### 使用 Visual Studio

1. 在 Visual Studio 中打开项目文件夹
2. CMake 将自动配置
3. 选择预设（推荐 x64-Release）
4. 生成 → 生成全部

### 使用命令行

```powershell
# 在 VS 2022 开发人员 PowerShell 中
cmake --preset=msvc-release
cmake --build build/msvc-release
```

### Windows 注意事项

- 某些示例（OpenMP 专用特性、perf）可能在原生 Windows 上无法工作
- 建议使用 WSL2 获得完整功能
- MSVC 支持为尽力而为

---

## 验证安装

安装完成后，验证一切正常：

```bash
# 检查编译器
g++ --version
cmake --version

# 构建并测试
cmake --preset=release
cmake --build build/release
ctest --preset=release

# 运行基准测试
./build/release/examples/02-memory-cache/bench/aos_soa_bench
```

---

## 另请参阅

- [先决条件](prerequisites.md) - 详细先决条件
- [故障排查](../reference/troubleshooting.md) - 常见问题
- [学习路径](../guides/learning-path.md) - 开始学习
