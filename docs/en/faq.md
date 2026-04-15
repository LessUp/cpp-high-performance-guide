# Frequently Asked Questions

## General Questions

### Q: What C++ knowledge do I need before starting?

You should have:
- Basic C++ syntax (classes, templates, STL containers)
- Understanding of pointers and memory management
- Familiarity with command-line tools
- Basic computer architecture concepts (CPU, memory, cache)

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

### Q: Build fails with "undefined reference to `pthread_create`"

Make sure CMake can find threads:

```bash
# Ubuntu/Debian
sudo apt-get install libpthread-stubs0-dev

# Reconfigure with fresh cache
cmake --preset=release --fresh
```

### Q: Sanitizer builds fail to link

Some sanitizers are mutually exclusive. Also ensure you have the sanitizer libraries:

```bash
# Ubuntu/Debian
sudo apt-get install libasan6 libtsan0 libubsan1
```

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

### Q: FlameGraph generation fails

Ensure you have the required tools:

```bash
# Install perf (linux-tools)
sudo apt-get install linux-tools-common linux-tools-generic

# The project includes FlameGraph scripts in tools/performance/
# No additional installation needed
```

---

## Test Issues

### Q: Tests fail on ThreadSanitizer build

ThreadSanitizer may report data races in third-party libraries. If the race is in your code:

1. Read the TSan report carefully
2. Check for missing atomic operations
3. Verify memory ordering is correct
4. Refer to [C++ Concurrency in Action](https://www.manning.com/books/c-plus-plus-concurrency-in-action)

### Q: AddressSanitizer reports memory leaks

ASan tracks allocations. If leaks are reported:

1. Check if they're in your code or dependencies
2. Use `ASAN_OPTIONS=detect_leaks=0` to disable leak checking temporarily
3. Fix actual leaks in your code

---

## Documentation Questions

### Q: Where can I find more learning resources?

- **Books**: "C++ Concurrency in Action", "Effective Modern C++"
- **Online**: [CppReference](https://en.cppreference.com/), [Compiler Explorer](https://godbolt.org/)
- **Papers**: [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)

### Q: How do I contribute a new example module?

See [Contributing Guide](../../CONTRIBUTING.md) for detailed instructions. Quick steps:

1. Create directory under `examples/XX-topic-name/`
2. Use the standard structure (src/, bench/, CMakeLists.txt, README.md)
3. Add tests in `tests/`
4. Submit a PR

### Q: Can I use this code in my project?

Yes! This project is MIT licensed. You can freely use, modify, and distribute the code. See [LICENSE](../../LICENSE) for details.

---

## Still Have Questions?

- 🐛 [Open an Issue](https://github.com/LessUp/cpp-high-performance-guide/issues)
- 💬 [Start a Discussion](https://github.com/LessUp/cpp-high-performance-guide/discussions)
- 📧 Contact maintainers
