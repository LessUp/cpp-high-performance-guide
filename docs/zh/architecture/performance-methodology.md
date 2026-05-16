# 性能方法论

本仓库偏好一条简单的证据链：复现问题、进行测量、一次只改变一个变量，再用合适的 preset 或 benchmark 验证结果。

## 工作规则

1. 先走 preset 驱动的默认验证路径。
2. 在宣称瓶颈之前先做 profiling。
3. 用 benchmark 比较方案，而不是为未经验证的代码背书。
4. 当改动影响仓库姿态时，在文档或 OpenSpec 中记录取舍。

## 操作入口

- [快速开始](/zh/getting-started/quickstart)
- [性能分析指南](/zh/guides/profiling-guide)
- [验证与 Sanitizer](/zh/guides/validation)
- [研究参考资料](/zh/research/references)
