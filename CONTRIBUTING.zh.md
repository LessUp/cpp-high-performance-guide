# 参与贡献到 C++ 高性能指南

感谢你对本项目的关注！本文档介绍如何为本项目贡献内容。

## 开始之前

1. Fork 本仓库
2. 克隆你的 Fork：
   ```bash
   git clone https://github.com/<your-username>/cpp-high-performance-guide.git
   ```
3. 创建功能分支：
   ```bash
   git checkout -b feature/your-feature-name
   ```

## 构建与测试

```bash
# Debug 构建并启用测试
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug

# Release 构建并运行基准测试
cmake --preset=release
cmake --build build/release
ctest --preset=release
```

## 代码风格

- 遵循项目根目录中的 `.clang-format` 配置
- 提交前格式化代码：
  ```bash
  find examples tests benchmarks -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
  ```
- 头文件统一使用 `#pragma once`
- 代码放在 `hpc::` 命名空间层级下

## 添加新的示例模块

1. 在 `examples/` 下创建新目录，遵循命名规则：`XX-topic-name/`
2. 使用统一结构：
   ```
   examples/XX-topic-name/
   ├── src/           # 带 demo main() 的源码文件
   ├── bench/         # Google Benchmark 文件
   ├── include/       # 可复用头文件（可选）
   ├── CMakeLists.txt # 使用 ExampleTemplate.cmake 中的 hpc_add_example()
   └── README.md      # 模块文档
   ```
3. 在 `examples/CMakeLists.txt` 中注册子目录
4. 在 `tests/` 下补充对应测试

## 测试要求

提交 Pull Request 之前，请确保：

1. **所有测试通过：**
   ```bash
   cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
   ```

2. **没有 AddressSanitizer 错误：**
   ```bash
   cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
   ```

3. **没有 ThreadSanitizer 错误**（并发代码尤其重要）：
   ```bash
   cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
   ```

4. **没有 UndefinedBehaviorSanitizer 错误：**
   ```bash
   cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
   ```

## Commit Message 建议

请使用清晰、可描述的提交信息：
- `feat: add matrix multiplication SIMD example`
- `fix: resolve false sharing in concurrent counter`
- `docs: update learning path with new module`
- `bench: add parameterized benchmark for prefetch distance`
- `test: add property tests for lock-free queue`

## Pull Request 流程

1. 确保你的分支与 `main` 保持同步
2. 确保所有 CI 检查通过
3. 在 PR 描述中清楚说明本次修改的内容和原因
4. 如有相关 issue，请在 PR 中引用

## License

通过提交贡献，你同意你的贡献将以 MIT License 进行许可。
