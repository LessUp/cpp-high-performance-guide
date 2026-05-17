# 算法

本模块将算法视为可执行的结论。下面每个条目都链接到本仓库中的可运行代码、基准证据或可复现实验。

## 算法地图

<SectionIndex
  :items='[
    {
      title: "排序",
      description: "从比较排序到 SIMD 加速基数排序。性能是测量出来的，不是假设出来的。",
      links: [
        { href: "/zh/algorithms/sorting", label: "排序算法" },
      ],
    },
    {
      title: "哈希",
      description: "在缓存压力和并发访问下仍保持高效的哈希函数与哈希表布局。",
      links: [
        { href: "/zh/algorithms/hashing", label: "哈希与哈希表" },
      ],
    },
  ]'
/>

## 复杂度速查表

| 算法 | 时间 | 空间 | 缓存友好 | 可并行 | 关键权衡 |
| --- | --- | --- | --- | --- | --- |
| 快速排序（内省式） | <ComplexityBadge value="O(n log n)" /> 平均 | <ComplexityBadge value="O(log n)" /> | 部分 | 困难 | 枢轴的分支预测失误 |
| 归并排序 | <ComplexityBadge value="O(n log n)" /> | <ComplexityBadge value="O(n)" /> | 是 | 容易 | 合并需额外内存 |
| 基数排序（LSD） | <ComplexityBadge value="O(nk)" /> | <ComplexityBadge value="O(n + r)" /> | 是 | 中等 | 键宽和数位大小 |
| Robin Hood 哈希 | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(n)" /> | 是 | 困难 | 插入时的 Robin Hood 位移 |
| Swiss Tables（F14） | <ComplexityBadge value="O(1)" /> 期望 | <ComplexityBadge value="O(n)" /> | 是 | 困难 | SIMD 探测、扁平布局 |

## 什么让算法在本项目中成为"高性能"

1. **测量优先。** 关于复杂度或吞吐量的结论必须由可通过本仓库 CMake preset 编译的基准测试支撑。
2. **缓存感知，而非仅大-O 感知。** 常数因子和内存访问模式往往在实际硬件上主导渐进界。
3. **ISA 敏感。** 当向量化改变算法思路（如 SIMD 上的双调排序、F14 的 SIMD 探测）时，实现会在 Compiler Explorer 中检查并用 `perf stat` 验证。
4. **可维护性边界。** 本仓库在成为生产库之前止步。每个算法都是教学规模的实现，附带明确的权衡说明，而非 exhaustive 优化目录。

## 阅读顺序

1. 若想了解经典算法如何适配现代 CPU 流水线，从[排序](/zh/algorithms/sorting)开始。
2. 继续阅读[哈希](/zh/algorithms/hashing)，理解影响每个哈希密集型工作负载的内存布局决策。
3. 每当需要校准基准结论时，交叉引用[性能方法论](/zh/architecture/performance-methodology)。
