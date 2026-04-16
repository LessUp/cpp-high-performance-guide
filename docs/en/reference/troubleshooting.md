# Troubleshooting Guide

This guide helps you resolve common issues when building, running, or profiling the C++ High Performance Guide examples.

---

## Build Failures

### CMake Cannot Find Compiler

**Symptom:**
```
CMake Error: Could not find a package configuration file provided by "compiler"
```

**Solution:**

1. Verify compiler installation:
```bash
g++ --version  # Should show GCC 11+ or Clang 14+
```

2. If not found, install:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential g++-11

# Fedora/RHEL
sudo dnf install gcc-c++ gcc

# macOS with Homebrew
brew install gcc
```

3. If multiple compilers are installed, specify explicitly:
```bash
export CC=gcc-11
export CXX=g++-11
cmake --preset=release --fresh
```

---

### Link Errors with Threads

**Symptom:**
```
undefined reference to `pthread_create'
```

**Solution:**

1. Install pthread development files:
```bash
sudo apt-get install libpthread-stubs0-dev
```

2. Reconfigure with fresh cache:
```bash
cmake --preset=release --fresh
```

### Sanitizer Link Errors

**Symptom:**
```
/usr/bin/ld: cannot find -lasan
```

**Solution:**

Install sanitizer libraries:
```bash
# Ubuntu/Debian
sudo apt-get install libasan6 libtsan0 libubsan1

# Fedora
sudo dnf install libasan libtsan libubsan
```

Note: ASan and TSan are mutually exclusive. Don't use them together.

---

## Runtime Issues

### Permission Denied with perf

**Symptom:**
```
Error:
Access to performance monitoring and observability operations is limited.
```

**Solutions:**

**Option 1**: Run with sudo (not recommended for regular use)
```bash
sudo perf record -g ./benchmark
```

**Option 2**: Adjust kernel parameter (temporary)
```bash
# Allow users to use perf
echo 1 | sudo tee /proc/sys/kernel/perf_event_paranoid

# Make permanent
echo 'kernel.perf_event_paranoid = 1' | sudo tee /etc/sysctl.d/99-perf.conf
sudo sysctl --system
```

**Option 3**: Add user to perf group (recommended)
```bash
sudo groupadd perf_users
sudo usermod -a -G perf_users $USER
# Log out and back in
```

---

### Benchmark Results Too Slow

**Symptom:** Benchmark results are 10x or more slower than expected.

**Diagnosis Steps:**

1. **Check build type**:
```bash
# Should see -O3 for Release
cmake --build build/release --verbose | grep -- -O
```

2. **Check CPU governor**:
```bash
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Should be "performance", not "powersave"
```

3. **Set to performance mode**:
```bash
sudo apt-get install linux-tools-common
sudo cpupower frequency-set --governor performance
```

---

## Platform-Specific Issues

### macOS: perf Not Available

**Explanation:** macOS doesn't have the Linux perf tool.

**Alternatives:**
- Use Instruments (GUI tool from Xcode)
- Use `sample` command-line tool
- Run profiling in Docker/Linux VM

### Windows: Build Issues

**Explanation:** Windows support is best-effort.

**Recommendations:**
1. Use WSL2 for full Linux compatibility
2. Or use Visual Studio 2022 with C++20 support
3. Some examples (OpenMP, perf) may not work on native Windows

---

## Getting Help

If your issue isn't resolved here:

1. **Search existing issues**: [GitHub Issues](https://github.com/LessUp/cpp-high-performance-guide/issues)
2. **Check discussions**: [GitHub Discussions](https://github.com/LessUp/cpp-high-performance-guide/discussions)
3. **Create a new issue** with:
   - Your OS and version
   - Compiler version (`g++ --version`)
   - CMake version (`cmake --version`)
   - Full error message
   - Steps to reproduce

---

## Related Documentation

- [FAQ](faq.md) - Quick answers
- [Installation Guide](../getting-started/installation.md) - Setup help
- [Profiling Guide](../guides/profiling-guide.md) - Performance analysis
