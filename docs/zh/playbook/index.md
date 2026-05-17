# 实践手册

实践手册是站点的操作层。它把可执行指南重新聚合到一起，使读者能够从第一次构建一路走到 profiling 与加固，而不必重新学习仓库布局。

## 操作原则

当你已经知道自己要做什么，但还不知道命令在哪一页时，就使用实践手册。它只是重新组织现有指南，而不物理移动文件，因此能在保持链接稳定的同时，为站点提供更清晰的专家叙事。

## 运行矩阵

| 需求 | 从这里开始 | 结果 |
| --- | --- | --- |
| 确认工具链与平台假设 | [先决条件](/zh/getting-started/prerequisites) | 在构建开始前获得正确的本地环境 |
| 安装依赖并产出二进制 | [安装指南](/zh/getting-started/installation) | 获得已配置且依赖齐全的仓库 |
| 完成第一次已验证运行 | [快速开始](/zh/getting-started/quickstart) | 得到一次基线构建与测试通过 |
| 按顺序学习各模块 | [学习路径](/zh/guides/learning-path) | 获得跨主题的导读顺序 |
| 诊断热点路径 | [性能分析指南](/zh/guides/profiling-guide) | 为下一次改动建立以 profiler 为先的证据 |
| 选择优化策略 | [优化决策树](/zh/guides/optimization-decision-tree) | 收敛出一组更具体的干预候选 |
| 在收尾前加固低层代码 | [验证与 Sanitizer](/zh/guides/validation) | 完成内存、竞态与 UB 检查 |
| 复核全仓库工作习惯 | [最佳实践](/zh/guides/best-practices) | 回到与仓库姿态一致的长期规则 |

## 推荐操作顺序

1. 先各执行一次[先决条件](/zh/getting-started/prerequisites)与[安装指南](/zh/getting-started/installation)。
2. 用[快速开始](/zh/getting-started/quickstart)证明仓库可以干净地构建。
3. 通过[学院](/zh/academy/)或[学习路径](/zh/guides/learning-path)选择一个主题。
4. 当你开始优化时，转入[性能分析指南](/zh/guides/profiling-guide)与[优化决策树](/zh/guides/optimization-decision-tree)。
5. 在宣称完成之前，运行[验证与 Sanitizer](/zh/guides/validation)。

## 次级支持表面

- [练习](/en/exercises/) 仍然可用，尤其适合在隔离环境中刻意复现某个概念。
- [AI 开发流程](/zh/contributing/ai-workflow) 仍是面向贡献者的入口，而不是核心读者旅程的一部分。
- [参考](/zh/reference/) 适合在你已经知道主题、但需要精确命令或 API 细节时进入。

## 为什么白皮书仍需要实践手册

如果一份白皮书没有可运行路径，它就很容易沦为装饰性文本。实践手册的作用，就是防止这种情况发生。它证明架构页与研究页仍然连接着一个活的仓库，而不是只服务于一套经过修饰的叙事。
