# 验证与 Sanitizer

先走 preset 驱动的常规验证路径，再根据你正在排查的问题选择对应的
sanitizer。

---

## 快速参考

| Preset | 适合发现的问题 | 备注 |
| --- | --- | --- |
| `asan` | 堆/栈越界、use-after-free、double free | 该 preset 会关闭 benchmark |
| `tsan` | 数据竞争、同步错误 | 该 preset 会切换到 `clang` / `clang++` |
| `ubsan` | 未定义行为、非法移位、有符号溢出 | 很适合作为功能修复后的补充验证 |

---

## AddressSanitizer

```bash
cmake --preset=asan
cmake --build build/asan
ctest --preset=asan
```

当你怀疑存在非法内存访问、对象生命周期错误或缓冲区越界时，优先使用
`asan`。

## ThreadSanitizer

```bash
cmake --preset=tsan
cmake --build build/tsan
ctest --preset=tsan
```

当修改涉及并发路径时使用 `tsan`。该 preset 已经为仓库切换到了受支持的
`clang` / `clang++` 工具链。

## UndefinedBehaviorSanitizer

```bash
cmake --preset=ubsan
cmake --build build/ubsan
ctest --preset=ubsan
```

当你想发现常规 debug/release 构建中不易显现的未定义行为时，使用
`ubsan`。

---

## 建议工作流

1. 先用 `debug` 或 `release` 复现问题。
2. 内存安全问题优先跑 `asan`。
3. 并发变更或偶发并行失败优先跑 `tsan`。
4. 涉及底层算术、类型转换、布局假设的修改，在收尾前补跑 `ubsan`。

仓库刻意把这些能力保留为独立 preset：更容易发现、更容易接入自动化，也
不会通过额外脚本把编译器相关的 sanitizer 约束隐藏起来。
