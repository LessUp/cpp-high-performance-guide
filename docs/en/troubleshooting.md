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

### FetchContent Download Failures

**Symptom:**
```
FetchContent failed to populate benchmark
```

**Solutions:**

1. **Network issues**: Check internet connection and proxy settings
```bash
# Test connectivity
curl -I https://github.com

# If behind proxy
export HTTP_PROXY=http://proxy:port
export HTTPS_PROXY=http://proxy:port
```

2. **Git not found**: Ensure Git is installed
```bash
sudo apt-get install git  # Ubuntu/Debian
```

3. **SSL certificate issues**:
```bash
# Update certificates
sudo apt-get install ca-certificates
# Or disable SSL verify (temporary workaround)
git config --global http.sslVerify false
```

4. **Use system packages instead** (if available):
```bash
# Ubuntu/Debian
sudo apt-get install libbenchmark-dev libgtest-dev
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

---

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

4. **Disable CPU boost** (for consistent results):
```bash
echo 0 | sudo tee /sys/devices/system/cpu/cpufreq/boost
```

5. **Pin to specific CPU**:
```bash
taskset -c 0 ./benchmark
```

6. **Check for thermal throttling**:
```bash
# Monitor temperatures during benchmark
watch -n1 sensors
```

---

### High Result Variance

**Symptom:** Benchmark results vary significantly between runs.

**Solutions:**

1. **Disable ASLR**:
```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
# Run benchmarks
# Re-enable when done: echo 2 | sudo tee /proc/sys/kernel/randomize_va_space
```

2. **Close other applications**

3. **Use benchmark repetitions**:
```bash
./benchmark --benchmark_repetitions=10 --benchmark_report_aggregates_only=true
```

4. **Disable hyperthreading** (for most consistent results):
```bash
# Disable logical CPUs
echo 0 | sudo tee /sys/devices/system/cpu/cpuX/online
```

---

## Test Failures

### ThreadSanitizer False Positives

**Symptom:** TSan reports data races in system libraries or third-party code.

**Solution:** Use suppression file:
```bash
# Create suppressions.txt
race:libpthread
race:libstdc++

# Run with suppression
TSAN_OPTIONS="suppressions=suppressions.txt" ./test
```

---

### AddressSanitizer Out of Memory

**Symptom:** ASan tests fail with OOM.

**Solution:** ASan increases memory usage. Either:
1. Run on machine with more RAM
2. Reduce test data sizes
3. Use debug build instead for those tests

---

## Profiling Issues

### FlameGraph Empty or Incomplete

**Symptom:** Generated SVG has no data or is very small.

**Solutions:**

1. **Ensure debug symbols are present**:
```bash
# Build with debug info
cmake --preset=relwithdebinfo
```

2. **Run benchmark long enough**:
```bash
./benchmark --benchmark_min_time=5
```

3. **Check perf record output**:
```bash
perf record -g ./benchmark
perf report  # See if samples were captured
```

4. **Increase sampling frequency**:
```bash
perf record -F 999 -g ./benchmark
```

---

### Valgrind Too Slow

**Symptom:** Cachegrind/Callgrind takes forever to run.

**Explanation:** Valgrind instrumentation is 10-50x slower than native.

**Solutions:**

1. Use smaller data sets for Valgrind runs
2. Use perf instead for quick profiling
3. Run Valgrind only on specific tests, not full benchmark suite

---

## Platform-Specific Issues

### macOS: perf Not Available

**Explanation:** macOS doesn't have the Linux perf tool.

**Alternatives:**
- Use Instruments (GUI tool from Xcode)
- Use `sample` command-line tool
- Run profiling in Docker/Linux VM

---

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

## Debugging Checklist

Before reporting an issue, please verify:

- [ ] Using supported compiler (GCC 11+, Clang 14+)
- [ ] CMake 3.22 or newer
- [ ] Fresh build directory (`--fresh` flag used)
- [ ] All dependencies installed
- [ ] Network connectivity (for FetchContent)
- [ ] Sufficient disk space
- [ ] Sufficient memory (especially for sanitizers)
