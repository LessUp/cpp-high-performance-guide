# 参与贡献

感谢你的贡献。这个仓库更偏好 **规格驱动、低噪声** 的修改，而不是范围松散的大改。

## 首次配置

```bash
git clone https://github.com/<your-username>/cpp-high-performance-guide.git
cd cpp-high-performance-guide

./scripts/setup.sh
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug
```

`./scripts/setup.sh` 会检查本地工具链，并通过 `.githooks/` 配置仓库自带的 Git hooks。

## 默认开发流程

1. 保持文档与代码同步。
2. 运行要求的验证命令。
3. 对非 trivial 修改在合并前使用 `/review`。

## 验证命令

```bash
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release

cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```

`tsan` preset 默认走 **Clang**，这是当前仓库里最稳定的 ThreadSanitizer 路径。

## 格式化与 hooks

```bash
./scripts/format.sh
./scripts/format.sh --check
./scripts/setup-hooks.sh
```

当前 hooks 会做这些事情：

- 拒绝提交生成的构建产物或 docs 产物
- 对 staged C++ 文件做格式检查
- push 前执行一次 debug build/test

## 文档规则

- 根 README 保持简洁，专注仓库入口。
- `docs/` 承担更完整的叙事与学习路径。
- 英文与中文的对外入口保持一致。
- 不要重新引入 GitBook / HonKit / `.kiro` 引用。

## 流程规则

- 对过时内容，优先删除或归档，而不是继续留在主路径。
- 新增 workflow 或工程化配置前，要能说明其项目特定价值。
- 避免同时维护大量长期分支；一个 change 对应一个聚焦分支或 PR 更合适。
- 非 trivial 的结构性修改不要跳过 review。

## 如果你需要 AI 开发流程

参见 `CLAUDE.md`。
