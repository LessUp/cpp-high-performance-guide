# 2026-03-10 GitHub Pages 优化

## 变更

### GitHub Pages 工作流优化 (`pages.yml`)
- **Sparse-checkout**：仅检出文档相关文件（docs/、examples/*/README.md、README.md 等），跳过 tests/、benchmarks/、cmake/、src/ 等大目录，显著减少 CI 检出时间和磁盘占用。
- **npm 缓存**：`actions/setup-node` 启用 `cache: npm`，后续构建复用 node_modules 缓存，加速 honkit 安装。
- **锁定 HonKit 版本**：新增 `package.json` + `package-lock.json`，将 honkit 固定为 `^3.7.1`，使用 `npm ci` 替代 `npm install`，确保可复现构建。
- **补全 paths 触发条件**：新增 `.gitbook.yaml`、`package.json`、`package-lock.json` 触发路径。

### Docs 徽章修复
- 修复 `README.md` 和 `README.zh-CN.md` 中 Docs 徽章链接：`docs.yml` → `pages.yml`，与实际工作流文件名一致。

### 新增文件
- `package.json` — 声明 honkit 开发依赖，提供 `docs:build` / `docs:serve` 脚本。
- `package-lock.json` — 锁定依赖版本，支持 npm 缓存和可复现安装。

## 原因

- 原工作流每次全量检出整个仓库（含 C++ 源码、测试、基准测试），仅为构建 Markdown 文档，浪费 CI 资源。
- `npm install honkit` 无版本锁定，每次构建可能安装不同版本，存在不可复现风险。
- Docs 徽章引用了不存在的 `docs.yml`，导致徽章状态始终显示异常。
