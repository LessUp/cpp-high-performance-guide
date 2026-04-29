# Validation & Sanitizers

Use the preset-driven validation path first, then pick the sanitizer that
matches the failure mode you are investigating.

---

## Quick reference

| Preset | Best for | Notes |
| --- | --- | --- |
| `asan` | heap/stack overflows, use-after-free, double free | Benchmarks are disabled in this preset |
| `tsan` | data races, unsafe synchronization | This preset switches to `clang` / `clang++` |
| `ubsan` | undefined behavior, invalid shifts, signed overflow | Good follow-up after functional fixes |

---

## AddressSanitizer

```bash
cmake --preset=asan
cmake --build build/asan
ctest --preset=asan
```

Use `asan` when you suspect invalid memory access, lifetime bugs, or accidental
buffer overruns.

## ThreadSanitizer

```bash
cmake --preset=tsan
cmake --build build/tsan
ctest --preset=tsan
```

Use `tsan` for concurrent code paths. The preset already selects `clang` /
`clang++`, which is the supported toolchain in this repository.

## UndefinedBehaviorSanitizer

```bash
cmake --preset=ubsan
cmake --build build/ubsan
ctest --preset=ubsan
```

Use `ubsan` to surface undefined behavior that may stay invisible in normal
debug or release builds.

---

## Suggested workflow

1. Start with `debug` or `release` to reproduce the issue normally.
2. Run `asan` for memory-safety problems.
3. Run `tsan` for concurrency changes or flaky parallel tests.
4. Run `ubsan` before closing work that touches low-level arithmetic, casts, or
   layout assumptions.

The repository keeps these as separate presets on purpose: they stay easy to
discover, easy to automate, and do not hide compiler-specific sanitizer
constraints behind extra wrapper scripts.
