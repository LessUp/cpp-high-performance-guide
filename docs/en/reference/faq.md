# Frequently Asked Questions

## General Questions

### Q: What C++ knowledge do I need before starting?

You should have:
- Basic C++ syntax (classes, templates, STL containers)
- Understanding of pointers and memory management
- Familiarity with command-line tools
- Basic computer architecture concepts (CPU, memory, cache)

See [Prerequisites](../getting-started/prerequisites.md) for details.

### Q: Which operating systems are supported?

The project is primarily tested on **Linux**. macOS and Windows are supported on a best-effort basis:
- Linux: Full support (recommended for profiling tools)
- macOS: Build and tests work; some profiling tools may differ
- Windows: Build works with MSVC or WSL; some Unix-specific tools unavailable

### Q: What compilers are supported?

- **GCC 11+** (fully supported)
- **Clang 14+** (fully supported)
- **MSVC** (best-effort support, some C++20 features may vary)

### Q: Why are my benchmark results different from the documented ones?

Performance depends on multiple factors:
- **CPU architecture**: Different generations have different optimizations
- **Memory speed**: DDR4 vs DDR5, single vs dual channel
- **Compiler version**: Newer compilers may optimize better
- **Background processes**: Close other applications for consistent results
- **Power settings**: Use "performance" governor on Linux

Always run benchmarks multiple times and report statistical aggregates.

---

## Build Issues

### Q: CMake configuration fails with "Could not find compiler"

Ensure you have a C++20 compatible compiler installed:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential g++-11

# Fedora
sudo dnf install gcc-c++

# macOS
xcode-select --install
# or
brew install gcc
```

### Q: "FetchContent failed to download dependencies"

Check your network connection. If behind a proxy:

```bash
export HTTP_PROXY=http://proxy.example.com:8080
export HTTPS_PROXY=http://proxy.example.com:8080
cmake --preset=release
```

Or use offline mode with pre-downloaded dependencies.

---

## Runtime Issues

### Q: Benchmarks run very slowly

Possible causes:
1. **Debug build**: Use `release` preset for performance measurements
2. **CPU throttling**: Set governor to performance:
   ```bash
   sudo cpupower frequency-set --governor performance
   ```
3. **Background load**: Close other applications
4. **Virtual machine**: VMs may have inconsistent performance

### Q: "Permission denied" when running perf

`perf` requires appropriate permissions:

```bash
# Option 1: Run with sudo
sudo perf record ./your_benchmark

# Option 2: Adjust perf_event_paranoid
echo 1 | sudo tee /proc/sys/kernel/perf_event_paranoid

# Option 3: Add user to perf group
sudo usermod -a -G perf $USER
# Log out and back in for changes to take effect
```

---

## Still Have Questions?

- 🐛 [Open an Issue](https://github.com/LessUp/cpp-high-performance-guide/issues)
- 💬 [Start a Discussion](https://github.com/LessUp/cpp-high-performance-guide/discussions)
- 📧 Contact maintainers

---

## Related Documentation

- [Troubleshooting](troubleshooting.md) - Detailed problem solving
- [Learning Path](../guides/learning-path.md) - Study curriculum
- [Profiling Guide](../guides/profiling-guide.md) - Performance analysis
