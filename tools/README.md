# 工具

性能分析与剖析工具集。

## 结构

- `performance/` - 性能分析工具
  - `generate_flamegraph.sh` - 从 perf 数据生成火焰图
  - `benchmark_compare.py` - 对比两次基准测试结果（完整工作流见 `benchmarks/README.md`）

开发辅助脚本（格式化、环境准备等）位于仓库根的 `scripts/` 目录，不在本目录下。

## 用法

### 火焰图生成

```bash
./tools/performance/generate_flamegraph.sh ./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

### 基准对比

```bash
python3 tools/performance/benchmark_compare.py baseline.json current.json --threshold 0.1
```
