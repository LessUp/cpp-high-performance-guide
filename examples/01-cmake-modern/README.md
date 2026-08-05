# 01 - 现代 CMake

以"反模式 vs 最佳实践"对照的方式演示目标驱动（target-based）的现代 CMake。
构建系统的深入分析见文档站：[CMake 构建系统专题](../../docs/zh/deep-dives/cmake-build-system.md)。

## 内容

- `best-practices/CMakeLists.txt` —— 带注释的最佳实践范本（仅作文档，不参与构建）
- `best-practices/src/` + `best-practices/include/` —— 可运行的演示代码，由 `cmake_demo` 目标构建

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release --target cmake_demo
./build/release/examples/01-cmake-modern/cmake_demo
```

## 要点速览

| 实践 | 收益 |
|------|------|
| `target_*` 目标级命令（而非 `include_directories` 等目录级命令） | 依赖不泄漏，PUBLIC/PRIVATE/INTERFACE 精确传播 |
| FetchContent + 钉死版本 | 依赖自动管理，无需手工 submodule |
| CMakePresets.json | 跨环境可复现构建（本仓库根目录即标准入口） |
| 生成器表达式按配置/编译器分流选项 | 多配置生成器兼容（注意：一个 genex 内多个 flag 必须用分号分隔成 CMake 列表，空格分隔会被当成单个参数） |
