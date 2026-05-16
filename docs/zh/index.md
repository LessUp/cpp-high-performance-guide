---
layout: home
---

<SectionHero
  badge="C++"
  title="C++ 高性能指南"
  subtitle="性能工程"
  intro="一份实用的 C++20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。每个主题都配有可运行的代码和面向基准测试的验证路径。"
  links-aria-label="落地页链接"
  :links='[
    { href: "/zh/getting-started/quickstart", label: "快速开始" },
    { href: "/zh/guides/learning-path", label: "学习路径" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
    { href: "/en/", label: "English" },
  ]'
>
  <template #aside>
    <MetricStrip
      aria-label="项目指标"
      :items='[
        { value: "C++20", label: "标准" },
        { value: "5", label: "个模块" },
        { value: "双语", label: "文档" },
      ]'
    />
  </template>
</SectionHero>

## 学习路径

<SectionIndex
  :items='[
    {
      title: "内存布局与缓存",
      description: "理解缓存行为、数据局部性和内存对齐，以实现最佳性能。",
      links: [
        { href: "/zh/guides/learning-path", label: "学习路径" },
        { href: "/en/exercises/module-02-memory", label: "练习 (英文)" },
      ],
    },
    {
      title: "SIMD 优化",
      description: "利用 SIMD 指令实现向量化计算和数据并行。",
      links: [
        { href: "/zh/guides/learning-path", label: "学习路径" },
        { href: "/en/exercises/module-04-simd", label: "练习 (英文)" },
      ],
    },
    {
      title: "并发与线程",
      description: "掌握 std::atomic、内存排序和无锁数据结构。",
      links: [
        { href: "/zh/guides/learning-path", label: "学习路径" },
        { href: "/en/exercises/module-05-concurrency", label: "练习 (英文)" },
      ],
    },
    {
      title: "构建系统 (CMake)",
      description: "现代 CMake 预设、Sanitizer 和跨平台构建配置。",
      links: [
        { href: "/zh/getting-started/quickstart", label: "快速开始" },
        { href: "/zh/getting-started/prerequisites", label: "先决条件" },
      ],
    },
    {
      title: "性能分析",
      description: "性能分析工具、基准测试方法论和优化决策树。",
      links: [
        { href: "/zh/guides/profiling-guide", label: "性能分析" },
        { href: "/zh/guides/optimization-decision-tree", label: "决策树" },
      ],
    },
    {
      title: "验证与 Sanitizer",
      description: "AddressSanitizer、ThreadSanitizer、UndefinedBehaviorSanitizer 和测试最佳实践。",
      links: [
        { href: "/zh/guides/validation", label: "Sanitizer" },
        { href: "/zh/guides/best-practices", label: "最佳实践" },
      ],
    },
  ]'
/>

<div class="wp-figure-shell wp-quick-start">
  <div class="wp-meta-strip" title="快速开始">快速开始</div>
  <div class="wp-quick-start-body">
    <div class="wp-command-block">
      <code>cmake --preset=debug && cmake --build build/debug</code>
    </div>
    构建完成后，运行 <code>ctest --preset=debug</code> 进行验证。详见<a href="/zh/getting-started/quickstart">快速开始指南</a>。
  </div>
</div>
