# 验证原则

这个仓库中的性能结论应由 preset 驱动的构建、测试，以及在合适场景下的 Sanitizer 或基准测试证据支撑。

## 默认路径

```bash
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug
```

## 升级路径

- 需要接近真实性能数据时使用 `release`。
- 调查内存、数据竞争或未定义行为时使用 `asan`、`tsan`、`ubsan`。
- 性能分析与基准测试是补充证据，不能替代功能正确性验证。

## 配套页面

- [验证与 Sanitizer](/zh/guides/validation)
- [性能方法论](/zh/architecture/performance-methodology)
- [性能分析指南](/zh/guides/profiling-guide)
