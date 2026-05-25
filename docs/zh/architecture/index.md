# 架构

架构部分说明仓库是如何组装起来的、文档如何映射到这套结构，以及验证方法如何把实现工作连接到性能结论。

## 架构论点

这个仓库围绕一个简单承诺组织：**每一条重要的性能经验都应同时以代码、证据与解释三种形式存在。**

这一定义塑造了架构：

- 示例模块是主要教学载体
- 可复用头文件保持小而明确
- preset 提供构建与测试的统一入口
- 基准与 profiler 构成测量层
- 文档保留解释与意图

## 架构入口

| 页面 | 主要问题 | 它连接的关键表面 |
| --- | --- | --- |
| [仓库拓扑](/zh/architecture/repository-topology) | 每类关注点分别位于哪里？ | 顶层目录、构建系统、文档与治理文件 |
| [性能方法论](/zh/architecture/performance-methodology) | 性能结论如何被建立？ | preset、基准、性能分析工具与证据规则 |
| [实践手册](/zh/playbook/) | 我接下来应该运行哪些命令？ | getting-started、profiling、validation 与 best-practices |
| [模块总览](/zh/academy/module-atlas) | 哪个模块教授哪个子系统？ | examples、tests、辅助头文件与文档入口 |

## Preset 驱动的验证回路

仓库刻意通过公开的 CMake preset 来教授验证，而不是依赖临时脚本。

```bash
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```

这条回路不仅是流程，更是架构。它为本地开发、CI、文档与后续维护提供了稳定词汇表。

## 谁应该先读什么

| 如果你是…… | 先读 | 再转到 |
| --- | --- | --- |
| 检查仓库是否整体一致 | [仓库拓扑](/zh/architecture/repository-topology) | [实践手册](/zh/playbook/) |
| 审视性能结论是否严谨 | [性能方法论](/zh/architecture/performance-methodology) | [研究参考资料](/zh/research/references) |
| 想快速理解某个模块 | [模块总览](/zh/academy/module-atlas) | 对应的示例目录 |
| 维护文档与仓库策略 | [研究演进](/zh/research/evolution) | GitHub 上的贡献者指导 |

## 本节不做什么

架构页不试图取代源码阅读。它的职责更窄，也更耐久：

- 让仓库边界清晰可读
- 解释为什么验证要从 preset 开始
- 定义基准背后的测量纪律
- 减少未来维护者必须从零重建的上下文
