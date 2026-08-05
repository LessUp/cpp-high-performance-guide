# 基准测试

本目录存放共享的基准工具，以及捕获/对比 Google Benchmark JSON 输出的本地工作流。
各模块的基准源码位于 `examples/*/bench/`。

## 回归对比工作流

1. 构建基准目标：

```bash
cmake --preset=release
cmake --build build/release
```

2. 捕获基线：

```bash
./build/release/examples/04-simd-vectorization/simd_bench \
  --benchmark_format=json \
  --benchmark_out=simd-baseline.json
```

3. 改动后捕获候选：

```bash
./build/release/examples/04-simd-vectorization/simd_bench \
  --benchmark_format=json \
  --benchmark_out=simd-candidate.json
```

4. 对比两次结果：

```bash
python3 tools/performance/benchmark_compare.py \
  simd-baseline.json simd-candidate.json \
  --threshold 0.1 --fail-on-regression
```

`--threshold` 是**比例**而非百分比（`0.1` 表示 10%）。加上 `--fail-on-regression`
后，任一基准回归超过阈值时脚本以退出码 1 结束，可用于本地冒烟或未来的 CI 门禁
（不传该参数时脚本只打印对比表，始终以 0 退出）。
