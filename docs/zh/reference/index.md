# 参考

当你需要精确接口、命令落点或故障路径指引，而不是叙事时，请进入参考部分。它是整站的精度层。

## 参考矩阵

| 如果你需要…… | 从这里开始 | 范围 |
| --- | --- | --- |
| 可复用辅助 API 与模块级头文件 | [API 入口](/zh/reference/api-reference) | 已文档化、但更深入细节页仍以英文为主的 API 入口页 |
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

## 英文优先的细节页

`reference/api/` 下的低层细节页按设计继续保持英文优先。它们记录诸如内存工具、SIMD wrapper 与 benchmark helper 之类的代码表面，此处更强调精确性，而不是完整翻译覆盖。

## 与其他部分的关系

- 当你需要一条有顺序的操作路线时，请转到[实践手册](/zh/playbook/)
- 当你需要系统上下文时，请转到[架构](/zh/architecture/)
- 当你需要外部引文或历史背景时，请转到[研究](/zh/research/)

参考入口刻意保持紧凑。即使其他叙事页面继续加深，它也应是一块稳定的落脚点。
