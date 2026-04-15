# 文档信息架构规范化（2026-03-13）

## 变更背景

- 继续推进仓库群 GitHub Pages 与文档入口标准化。
- 该仓库原先由 `README.md` / `README.zh-CN.md` 同时承担仓库入口和 HonKit 文档首页职责，导致仓库介绍、学习路径、示例索引与在线阅读入口混在一起。
- 本次调整目标是在保留 HonKit / GitBook 技术栈的前提下，拆分仓库入口与文档入口职责，并让 Pages workflow 与当前默认分支保持一致。

## 导航与目录调整

- `SUMMARY.md` 顶层目录改为以 `DOCS.md` 作为文档首页，再按英文文档、中文文档、示例模块、贡献指南分组。
- 新增 `DOCS.md` 作为 HonKit / GitBook 首页，集中承载项目定位、适合谁、从哪里开始、推荐阅读路径与核心文档入口。
- 保留 `docs/en/`、`docs/zh/` 与 `examples/*/README.md` 原有内容路径，不做大规模迁移，仅重组入口层级。

## 首页调整

- `README.md` / `README.zh-CN.md` 收敛为仓库入口，只保留项目定位、仓库概览、最短构建命令、文档入口与开发信息。
- `DOCS.md` 改为文档首页，负责引导读者进入学习路径、性能分析、站点同步说明与示例模块。
- `docs/en/gitbook-sync.md` 与 `docs/zh/gitbook-sync.md` 同步更新，明确 `DOCS.md` 是当前文档首页，且仓库默认同步分支为 `master`。

## Pages / Workflow 调整

- `.gitbook.yaml` 与 `book.json` 的 `readme` 入口统一改为 `DOCS.md`，保持 HonKit / GitBook 首页与信息架构一致。
- `.github/workflows/pages.yml` 的触发分支由 `main` 调整为 `master, main`，与仓库当前默认分支兼容。
- Pages workflow 的 `paths` 与 `sparse-checkout` 补充 `DOCS.md` 与 `changelog/**`，确保首页和变更记录更新都能触发文档部署。
- `sparse-checkout` 从仅检出 `docs/en`、`docs/zh` 调整为检出整个 `docs/` 目录，避免后续新增文档入口文件时漏检。

## 验证结果

- 已在仓库根目录执行 `npm ci` 与 `npm run docs:build`，HonKit 构建成功，产物输出到 `_site/`。
- 已检查 `_site/` 中生成的 HTML 页面，共输出 14 个 HTML 页面，首页与中英文核心文档页均已生成。
- 已检查生成首页内容，`Documentation Home`、`Learning Path`、`Profiling Guide` 等核心入口文案均存在。
- 已人工确认 `README.md` / `README.zh-CN.md` 与 `DOCS.md` 职责已分离：前者为仓库入口，后者为文档入口。

## 后续待办

- 评估是否为 HonKit 目录进一步加入中英文语言切换入口页，减少当前通过目录分组理解双语结构的认知成本。
- 后续如继续统一 HonKit 仓库群，可复用本仓库的 `DOCS.md + SUMMARY.md` 入口拆分方式。
