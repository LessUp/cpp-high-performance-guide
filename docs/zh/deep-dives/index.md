---
type: section-index
description: 深入探索 C++ 高性能编程核心概念的技术文章。
---

# 深度技术专题

这些深度技术专题文章对 C++ 高性能编程中的基础概念进行了全面、技术性的探索。每篇文章假设读者已了解基础知识，深入探讨实现细节、性能考量和实践指南。

## 可用的深度专题

<SectionIndex
  :cards="[
    {
      title: '内存布局',
      description: 'AOS 与 SOA 布局对比、缓存行对齐、伪共享消除以及内存预取策略。',
      links: [
        { text: '阅读', href: '/zh/deep-dives/memory-layout' }
      ]
    },
    {
      title: '无锁队列',
      description: 'SPSC 和 MPMC 队列实现、内存序语义以及无锁算法设计。',
      links: [
        { text: '阅读', href: '/zh/deep-dives/lock-free-queue' }
      ]
    },
    {
      title: 'SIMD 内部机制',
      description: '自动向量化条件、内联函数封装、运行时分发以及 AVX-512 掩码操作。',
      links: [
        { text: '阅读', href: '/zh/deep-dives/simd-internals' }
      ]
    }
  ]"
/>

## 前置知识

在深入这些高级主题之前，你应该熟悉：

- **学院**：模块总览和验证原则
- **架构**：仓库拓扑和性能方法论
- **C++ 基础**：移动语义、模板和 STL

## 阅读建议

这些文章专为**深入理解**而设计，而非快速参考。我们建议：

1. **按顺序阅读** - 每篇文章逐步构建概念
2. **动手实验** - 所有示例都可以在 examples/ 目录中运行
3. **自行基准测试** - 使用提供的基准测试验证性能声明
4. **查阅参考文献** - 每篇文章都引用了原始资料以供深入研究
