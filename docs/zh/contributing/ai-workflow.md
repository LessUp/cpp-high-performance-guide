# AI 辅助开发流程

本仓库采用 **OpenSpec 优先** 的开发方式，并为主要 AI 工具保留少量但明确的说明文件。

## 权威说明文件

- `openspec/` —— 需求、变更意图与任务顺序
- `AGENTS.md` —— 终端 copilot / Codex 风格代理共享说明
- `CLAUDE.md` —— Claude Code 说明
- `.github/copilot-instructions.md` —— GitHub Copilot 仓库说明

如果这些文件之间出现漂移，优先修正漂移，而不是再引入新的流程文档。

## 推荐流程

1. 先阅读 `openspec/specs/` 中受影响的 capability spec。
2. 在 `openspec/changes/<change-id>/` 下创建或更新一个变更。
3. 按任务依赖顺序实施。
4. 使用仓库已有的 preset 驱动验证命令。
5. 在合并前或一个较大的清理阶段结束后使用 `/review`。
6. 完成后归档该 change。

## 工具选择

- **LSP**：`clangd`
- **构建 / 测试入口**：`cmake --preset=...` 与 `ctest --preset=...`
- **hooks**：仓库自带 `.githooks/`
- **autopilot 策略**：优先更长的单次会话，只有在任务天然并行时才考虑 `/fleet`

## 需要避免

- 重新引入 GitBook / HonKit / `.kiro` 假设
- 添加与现有说明重复的泛化 AI 文档
- 在没有项目特定收益时引入重型 MCP 或插件层
