# 参考

当你需要精确接口、命令落点或故障路径指引，而不是叙事时，请进入参考部分。它是整站的精度层。

## 参考矩阵

| 如果你需要…… | 从这里开始 | 范围 |
| --- | --- | --- |
| 可复用辅助 API 与模块级头文件 | [API 入口](/zh/reference/api-reference) | 已文档化的 API 入口页与底层细节页 |
| 内存工具 API 细节 | [内存工具](/zh/reference/api/memory-utils) | `hpc::memory` 命名空间的对齐分配器与缓存行填充工具 |
| SIMD Wrapper API 细节 | [SIMD Wrapper](/zh/reference/api/simd-wrapper) | `hpc::simd` 命名空间的运行时分发与向量类型 |
| Benchmark 工具 API 细节 | [Benchmark 工具](/zh/reference/api/benchmark-utils) | `hpc::bench` 命名空间的辅助函数与结果比较工具 |
| 对高频安装与工作流问题的快速回答 | [常见问题](/zh/reference/faq) | 高频问题与简明答案 |
| 构建、运行时或基准失败后的恢复路径 | [故障排查](/zh/reference/troubleshooting) | 操作层诊断路线 |
| 方法论背后的来源材料 | [研究参考资料](/zh/research/references) | 标准、手册、仓库与文章 |
| 从模块回到代码的映射 | [模块总览](/zh/academy/module-atlas) | 偏架构定向，而不是 API 细节 |

## 命令速查

| 任务 | 规范入口 |
| --- | --- |
| 基线验证 | `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` |
| 优化配置验证 | `cmake --preset=release && cmake --build build/release && ctest --preset=release` |
| 内存安全检查 | `cmake --preset=asan && cmake --build build/asan && ctest --preset=asan` |
| 竞态检测 | `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan` |
| 未定义行为检查 | `cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan` |

## 与其他部分的关系

- 当你需要一条有顺序的操作路线时，请转到[实践手册](/zh/playbook/)
- 当你需要系统上下文时，请转到[架构](/zh/architecture/)
- 当你需要外部引文或历史背景时，请转到[研究](/zh/research/)

参考入口刻意保持紧凑。即使其他叙事页面继续加深，它也应是一块稳定的落脚点。
