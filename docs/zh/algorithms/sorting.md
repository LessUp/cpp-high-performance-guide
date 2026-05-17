# 排序算法

排序是经典的算法问题。在高性能 C++ 中，有趣的问题通常不是"哪种排序理论最优"，而是"哪种排序对*这种数据形状、在这种微架构上*最优"。

---

## 概览

| 算法 | 最优时间 | 平均时间 | 最坏时间 | 空间 | 稳定 | 说明 |
| --- | --- | --- | --- | --- | --- | --- |
| 快速排序（Hoare） | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n²)" /> | <ComplexityBadge value="O(log n)" /> | 否 | 实践中缓存友好；枢轴选择关键 |
| 内省排序 | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(log n)" /> | 否 | 快速排序 + 堆排序回退；`std::sort` 使用此算法 |
| 归并排序 | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n)" /> | 是 | 可预测、缓存友好；适合链接数据 |
| Timsort | <ComplexityBadge value="O(n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n)" /> | 是 | 利用真实数据中的有序段 |
| 基数排序（LSD） | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(n + r)" /> | 是 | k = 键宽，r = 基数；定宽键时占绝对优势 |
| 双调排序 | <ComplexityBadge value="O(log² n)" /> | <ComplexityBadge value="O(log² n)" /> | <ComplexityBadge value="O(log² n)" /> | <ComplexityBadge value="O(1)" /> | 否 | 数据并行；在 SIMD/GPU 上表现优异 |

## 快速排序及其工程实现

### 为什么快速排序在实践中占主导

尽管最坏情况是 <ComplexityBadge value="O(n²)" />，快速排序的平均缓存行为和低指令数使其成为 C++ 通用排序的默认选择（`std::sort` 通常是内省排序的变体）。

**关键工程决策：**

1. **枢轴选择。** 三数取中或中位数的中位数减少病态输入。部分实现采样更多枢轴以规避对抗性输入。
2. **分区方案。** Hoare 分区每个元素只访问一次；Lomuto 更简单但交换更多。现代实现偏向 Hoare 或混合方案。
3. **小数组回退。** 当子数组低于阈值（通常 16–32 个元素）时，插入排序因开销更低而更快。
4. **尾递归优化。** 先递归较小分区、循环处理较大分区，保证栈深度为 <ComplexityBadge value="O(log n)" />。

### 缓存行为

快速排序*并非*天生缓存最优，但其分治结构在中等输入规模下通常能保持工作集在缓存内。关键指标是每个元素的缓存缺失次数：

```cpp
// 缓存友好的快速排序草图
void quicksort(int* first, int* last) {
  while (last - first > 32) {
    auto* pivot = hoare_partition(first, last);
    if (pivot - first < last - pivot) {
      quicksort(first, pivot);
      first = pivot + 1;
    } else {
      quicksort(pivot + 1, last);
      last = pivot;
    }
  }
  insertion_sort(first, last);
}
```

## 归并排序与稳定性

归并排序保证 <ComplexityBadge value="O(n log n)" /> 和稳定性，代价是 <ComplexityBadge value="O(n)" /> 辅助内存。在 C++ 中，`std::stable_sort` 使用归并排序的变体。

### 内存布局优化

朴素的归并排序每次合并都分配临时缓冲区。更好的做法：

- 在顶层分配一个大小为 `n` 的临时数组。
- 交替合并方向（A→B，然后 B→A）以避免拷贝。
- 这样将分配开销从 <ComplexityBadge value="O(n log n)" /> 降到 <ComplexityBadge value="O(n)" />。

## 基数排序：当大-O 失效时

对于定宽整数键，基数排序可以达到线性时间，并以 3–10 倍优势击败比较排序。代价：它不是基于比较的，需要额外内存。

### LSD（最低有效位优先）

```cpp
// 简化的 32 位无符号整数 LSD 基数排序
void lsd_radix_sort(uint32_t* data, size_t n) {
  constexpr int bits_per_pass = 8;
  constexpr int buckets = 1 << bits_per_pass;
  constexpr int mask = buckets - 1;
  std::vector<uint32_t> temp(n);

  for (int shift = 0; shift < 32; shift += bits_per_pass) {
    size_t count[buckets] = {};
    for (size_t i = 0; i < n; ++i) {
      ++count[(data[i] >> shift) & mask];
    }
    size_t prefix = 0;
    for (int b = 0; b < buckets; ++b) {
      size_t c = count[b];
      count[b] = prefix;
      prefix += c;
    }
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = data[i];
      size_t bucket = (key >> shift) & mask;
      temp[count[bucket]++] = key;
    }
    std::swap(data, temp);
  }
}
```

**性能特征：**
- 32 位键需 4 轮（每轮 8 位）。
- 每轮都是线性扫描：若 `count` 能放入 L1，则缓存友好。
- 只要偏移跟踪正确，上述实现是稳定的。

### SIMD 机会

部分基数排序变体使用 SIMD 加速直方图（计数）阶段。AVX2 可同时处理 8 个整数，减少计数循环的串行依赖。详见仓库 `examples/04-simd-vectorization/` 中适用于直方图计算的向量化模式。

## 面向并行硬件的双调排序

双调排序是数据并行算法，深度为 <ComplexityBadge value="O(log² n)" />，非常适合 SIMD 和 GPU 执行。它在顺序 CPU 上不是比较高效的，但在宽向量单元可用时表现突出。

关键操作是**双调归并**：跨步长比较交换，然后步长减半。在 AVX-512 上，16 个元素的双调网络可放入单个寄存器，仅需少量置换-比较指令序列即可排序。

## 实用选择指南

<DiagramCanvas title="排序选择决策" caption="在性能分析前，用此流程缩小排序家族范围。">

```mermaid
flowchart TD
    A[需要排序？] --> B{需要稳定性？}
    B -->|是| C{链表 / 指针密集型？}
    B -->|否| D{整数 / 定宽键？}
    C -->|是| E[归并排序<br/>或 Timsort]
    C -->|否| F[稳定内省排序<br/>std::stable_sort]
    D -->|是| G{键分布已知？}
    D -->|否| H[内省排序<br/>std::sort]
    G -->|均匀 / 密集| I[基数排序<br/>LSD]
    G -->|稀疏 / 可变| H
    H --> J{N < 1e5？}
    J -->|是| K[快速排序<br/>缓存友好]
    J -->|否| L[内省排序<br/>+ 无分支分区]
    E --> M[用基准测试<br/>验证]
    F --> M
    I --> M
    K --> M
    L --> M

    classDef decision fill:var(--wp-diagram-decision),stroke:var(--wp-diagram-decision-stroke)
    classDef action fill:var(--wp-diagram-action),stroke:var(--wp-diagram-action-stroke)
    classDef complete fill:var(--wp-diagram-complete),stroke:var(--wp-diagram-complete-stroke)
    classDef start fill:var(--wp-diagram-start),stroke:var(--wp-diagram-start-stroke)

    class A start
    class B,D,C,G,J decision
    class E,F,I,H,K,L action
    class M complete
```

</DiagramCanvas>

## 基准测试说明

本仓库的基准测试工作流遵循以下协议：

1. 生成输入分布：随机、已排序、逆序、少重复、对抗性（中位数杀手）。
2. 预热 CPU 并绑定线程。
3. 从 `perf stat -e cycles,cache-misses` 报告每个元素的周期数和 LLC 缺失数。
4. 与 `std::sort`、`std::stable_sort` 以及可用的 `absl::c_sort` 对比。

## 参考文献

<Citation :references="[
  { id: 'sedgewick2011', author: 'Sedgewick, R. & Wayne, K.', title: 'Algorithms, 4th Edition', year: 2011, source: 'Addison-Wesley', url: 'https://algs4.cs.princeton.edu/home/' },
  { id: 'musser1997', author: 'Musser, D. R.', title: 'Introspective Sorting and Selection Algorithms', year: 1997, source: 'Software: Practice and Experience, 27(8)', url: 'https://doi.org/10.1002/(SICI)1097-024X(199708)27:8<983::AID-SPE117>3.0.CO;2-#' },
  { id: 'lemire2019', author: 'Lemire, D. & Boytsov, L.', title: 'Decoding billions of integers per second through vectorization', year: 2019, source: 'Software: Practice and Experience', url: 'https://arxiv.org/abs/1209.1717' },
  { id: 'mcllroy1999', author: 'McIlroy, P. M.', title: 'A Killer Adversary for Quicksort', year: 1999, source: 'Software: Practice and Experience, 29(4)', url: 'https://doi.org/10.1002/(SICI)1097-024X(19990325)29:4<341::AID-SPE258>3.0.CO;2-9' },
]" />
