# 哈希与哈希表

哈希表是性能关键代码中仅次于数组的最常用数据结构。它的速度并非神秘，而是围绕缓存行、负载因子和探测序列进行精细工程的结果。

---

## 概览

| 技术 | 查找 | 插入 | 删除 | 内存 | 缓存 | 说明 |
| --- | --- | --- | --- | --- | --- | --- |
| 分离链接 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(n)" /> + 开销 | 差 | 指针分散内存；简单 |
| 线性探测 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(n)" /> | 好 | 主聚集；最佳缓存行为 |
| Robin Hood | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(n)" /> | 好 | 有界探测距离；删除用墓碑 |
| 布谷鸟哈希 | <ComplexityBadge value="O(1)" /> 最坏 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 最坏 | <ComplexityBadge value="O(n)" /> | 中等 | 双表 + 循环时重哈希 |
| Swiss Tables（F14） | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(n)" /> | 优秀 | SIMD 组探测；扁平布局 |

## 哈希函数质量

在表布局之前，哈希函数必须均匀分布键。差的哈希函数可将任何开放寻址表变成链表。

### 什么让哈希函数在 C++ 中更快

1. **避免素数取模。** 优先使用 2 的幂次表大小和位掩码（`hash & (size - 1)`）。这只是一个整数操作，而非除法。
2. **雪崩性。** 键的单个比特变化应翻转哈希输出中约一半的比特。
3. **常见类型特化。** 整数哈希应混合数值；字符串哈希应以字大小块处理字符串。

```cpp
// 快速、尚可的整数哈希（MurmurHash3 终结器）
uint64_t hash_u64(uint64_t x) {
  x ^= x >> 33;
  x *= 0xff51afd7ed558ccdULL;
  x ^= x >> 33;
  x *= 0xc4ceb9fe1a85ec53ULL;
  x ^= x >> 33;
  return x;
}
```

对于字符串，仓库使用 FNV-1a 变体或可用时的硬件 CRC32。详见 `examples/02-memory-cache/` 中对相同键分布下哈希函数的基准对比。

## 线性探测及其局限

线性探测是最简单的开放寻址方案：若槽 `h(key)` 被占用，尝试 `(h(key) + 1) % M`，然后 `+2`，依此类推。

### 为什么它快

- **缓存友好。** 探测连续数组意味着预取器和缓存行为你工作。
- **代码简单。** 无额外间接、无指针追逐。

### 为什么可能失效

- **主聚集。** 连续的被占用槽形成簇，增加平均探测距离。
- **对负载因子敏感。** 超过 0.7–0.75 后性能急剧退化。

```cpp
// 简化的线性探测查找
Value* find(const Key& key) {
  size_t idx = hash(key) & mask;
  while (states[idx] != EMPTY) {
    if (states[idx] == OCCUPIED && keys[idx] == key) return &values[idx];
    idx = (idx + 1) & mask;
  }
  return nullptr;
}
```

## Robin Hood 哈希

Robin Hood 哈希通过置换已有条目来限制最大探测距离——当新条目的理想槽距离大于已有条目时，发生置换。

### Robin Hood 原理

插入时，若新元素的"距理想槽距离"*大于*已有元素的，则交换两者并继续探测被置换元素。这保持了探测距离的方差较低。

### 删除

标准开放寻址删除需要墓碑。Robin Hood 表可采用后向位移：删除时向前扫描，若元素可回到理想位置，则将其后移。

### SIMD 加速 Robin Hood

部分实现（如 `ska::flat_hash_map`）在独立的并行数组中存储元数据字节（哈希片段）。查找时先用 SSE/NEON 同时比较 16 个元数据字节，再仅对匹配片段检查键。这减少了缓存缺失和键比较次数。

## Swiss Tables 与 F14

Facebook 的 F14 和 Abseil 的 Swiss Tables 代表了 C++ 通用哈希表的当前最先进水平。

### 关键设计思想

1. **组探测。** 表被分成 16 槽的组。SIMD 比较可同时检查所有 16 个控制字节以找到候选。
2. **扁平布局。** 键和值内联存储在单一数组中；无分离链接或指针间接。
3. **两级存储。** F14 使用小型元数据数组（控制字节）和独立的值数组，允许重分配值时无需移动键。

```cpp
// 概念性 SIMD 组探测（SSE2/NEON）
// control_bytes: 16 字节，每个为 (hash_fragment | special_states)
// target: 键的 8 位哈希片段
uint16_t match = simd_eq_16x8(control_bytes, target);
while (match) {
  int slot = ctz(match);  // 计算尾随零
  if (keys[slot] == key) return &values[slot];
  match &= match - 1;     // 清除最低置位比特
}
```

### 何时使用 Swiss Tables

- 查找占主导的通用映射。
- 当 `absl::flat_hash_map` 或 `folly::F14VectorMap` 已作为依赖可用时。
- 当仓库的教学规模实现不足以支撑生产工作负载时。

## 实用选择指南

<DiagramCanvas title="哈希表选择" caption="将工作负载特征匹配到表家族。">

```mermaid
flowchart TD
    A[需要哈希表] --> B{大小预先可知？}
    B -->|是| C{查找密集型？}
    B -->|否| D[Robin Hood<br/>或 F14Vector]
    C -->|是| E{SIMD 可用？}
    C -->|否| F[Robin Hood<br/>线性探测]
    E -->|是| G[Swiss Tables / F14<br/>组探测]
    E -->|否| H[Robin Hood<br/>元数据探测]
    D --> I{删除密集型？}
    I -->|是| J[F14<br/>两级存储]
    I -->|否| K[Robin Hood<br/>后向位移]
    F --> L[用基准测试<br/>验证]
    G --> L
    H --> L
    J --> L
    K --> L

    classDef decision fill:var(--wp-diagram-decision),stroke:var(--wp-diagram-decision-stroke)
    classDef action fill:var(--wp-diagram-action),stroke:var(--wp-diagram-action-stroke)
    classDef complete fill:var(--wp-diagram-complete),stroke:var(--wp-diagram-complete-stroke)
    classDef start fill:var(--wp-diagram-start),stroke:var(--wp-diagram-start-stroke)

    class A start
    class B,C,E,I decision
    class D,F,G,H,J,K action
    class L complete
```

</DiagramCanvas>

## 基准测试说明

仓库对哈希表的基准测试包括：

1. **突发插入。** 插入 N 个元素，测量每次插入的周期数。
2. **混合查找。** 80% 成功查找，20% 缺失（典型缓存模式）。
3. **删除模式。** 删除一半、重新插入一半，测量重哈希率。
4. **内存占用。** 峰值和稳态下的 RSS。

所有基准测试均附带 `perf stat -e cycles,cache-misses,branch-misses` 运行。

## 参考文献

<Citation :references="[
  { id: 'celis1985', author: 'Celis, P.', title: 'Robin Hood Hashing', year: 1985, source: 'PhD thesis, University of Waterloo', url: 'https://cs.uwaterloo.ca/research/tr/1985/CS-85-14.pdf' },
  { id: 'leiserson2020', author: 'Leiserson, C. E. et al.', title: 'F14: A Memory-Efficient Hash Table', year: 2020, source: 'Facebook Engineering', url: 'https://engineering.fb.com/2019/04/25/developer-tools/f14/' },
  { id: 'lemire2018', author: 'Lemire, D.', title: 'Fast Random Integer Generation in an Interval', year: 2018, source: 'ACM Trans. Model. Comput. Simul.', url: 'https://doi.org/10.1145/3230636' },
  { id: 'absl2024', author: 'Abseil Authors', title: 'Swiss Tables Design Notes', year: 2024, source: 'Abseil C++ Documentation', url: 'https://abseil.io/about/design/swisstables' },
]" />
