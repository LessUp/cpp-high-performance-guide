# GitBook 接入指南

当前仓库已经为通过 Git Sync 接入 GitBook 做好了在线阅读准备。

## 推荐接入方式

1. 创建或打开一个 GitBook Space。
2. 在 Space 顶部选择 `Configure`。
3. 选择 `GitHub Sync`。
4. 完成 GitHub 账户授权。
5. 为当前仓库所属账号或组织安装 GitBook GitHub App。
6. 选择仓库 `LessUp/cpp-high-performance-guide`。
7. 选择需要同步的分支，本仓库通常使用 `master`。
8. 如果仓库里已经有 Markdown 文档，首次同步建议选择 `GitHub -> GitBook`。

## 仓库内已准备的文件

当前仓库已经包含以下 GitBook 需要识别的文件：

- `.gitbook.yaml`
- `SUMMARY.md`
- `README.md`（首页）
- `docs/` 下的 Markdown 文档

## 内容结构

GitBook 将使用：

- `README.md` 作为首页（在 `.gitbook.yaml` 中配置）
- `SUMMARY.md` 作为目录导航

这些入口已经在 `.gitbook.yaml` 中显式声明。

## 注意事项

- 入口页面通过仓库中的 `README.md` 管理。
- 如果你没有在 GitBook 中手工调整目录，`SUMMARY.md` 会直接作为目录来源。
- 如果你后续把文档迁移到其他根目录，需要同步更新 `.gitbook.yaml`。
