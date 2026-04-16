# Installation Guide

Complete installation instructions for all supported platforms.

---

## Table of Contents

- [Linux (Ubuntu/Debian)](#linux-ubuntudebian)
- [Linux (Fedora/RHEL)](#linux-fedorarhel)
- [macOS](#macos)
- [Windows (WSL2)](#windows-wsl2)
- [Windows (Native)](#windows-native)

---

## Linux (Ubuntu/Debian)

### System Dependencies

```bash
# Update package list
sudo apt-get update

# Install build essentials
sudo apt-get install -y build-essential cmake git

# Install C++20 compilers
sudo apt-get install -y g++-11 clang-14

# Install optional tools for profiling
sudo apt-get install -y linux-tools-common linux-tools-generic
sudo apt-get install -y valgrind perf-tools-unstable

# Install sanitizer libraries (optional)
sudo apt-get install -y libasan6 libtsan0 libubsan1
```

### Build from Source

```bash
# Clone repository
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# Configure and build
cmake --preset=release
cmake --build build/release

# Run tests
ctest --preset=release
```

---

## Linux (Fedora/RHEL)

### System Dependencies

```bash
# Install development tools
sudo dnf groupinstall "Development Tools"

# Install C++20 compilers
sudo dnf install gcc-c++ clang cmake git

# Install profiling tools
sudo dnf install perf valgrind

# Install sanitizer libraries
sudo dnf install libasan libtsan libubsan
```

### Build from Source

Same as Ubuntu/Debian above.

---

## macOS

### Using Homebrew

```bash
# Install Homebrew first if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git
brew install gcc  # Or use Xcode Clang
```

### Using Xcode

Install Xcode from the App Store, then:

```bash
xcode-select --install
```

### Configuration Notes

macOS uses `clang++` by default. Some Linux-specific profiling tools (like `perf`) are not available on macOS. Use Instruments (included with Xcode) as an alternative.

```bash
# Build with default compiler (Clang)
cmake --preset=release
cmake --build build/release
```

---

## Windows (WSL2)

WSL2 provides the best experience for Windows users.

### Install WSL2

```powershell
# In PowerShell (Administrator)
wsl --install
```

Then follow the Linux (Ubuntu) instructions above inside WSL2.

---

## Windows (Native)

### Requirements

- Visual Studio 2022 with C++ workload
- CMake 3.20+ (included with VS2022)
- Git for Windows

### Using Visual Studio

1. Open the project folder in Visual Studio
2. CMake will auto-configure
3. Select a preset (x64-Release recommended)
4. Build → Build All

### Using Command Line

```powershell
# In Developer PowerShell for VS 2022
cmake --preset=msvc-release
cmake --build build/msvc-release
```

### Notes for Windows

- Some examples (OpenMP specifics, perf) may not work on native Windows
- WSL2 is recommended for full functionality
- MSVC support is best-effort

---

## Verification

After installation, verify everything works:

```bash
# Check compiler
g++ --version
cmake --version

# Build and test
cmake --preset=release
cmake --build build/release
ctest --preset=release

# Run a benchmark
./build/release/examples/02-memory-cache/bench/aos_soa_bench
```

---

## See Also

- [Prerequisites](prerequisites.md) - Detailed prerequisites
- [Troubleshooting](../reference/troubleshooting.md) - Common issues
- [Learning Path](../guides/learning-path.md) - Start learning
