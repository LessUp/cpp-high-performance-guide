---
layout: home
---

<SectionHero
  badge="C++"
  title="C++ 高性能指南"
  subtitle="技术白皮书"
  intro="一份实用的 C++20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。新的白皮书主线依次通向学院、架构、实践手册、参考与研究。"
  links-aria-label="落地页链接"
  :links='[
    { href: "/zh/academy/", label: "学院" },
    { href: "/zh/playbook/", label: "实践手册" },
    { href: "/zh/reference/", label: "参考" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
    { href: "/en/", label: "English" },
  ]'
>
  <template #aside>
    <MetricStrip
      aria-label="项目指标"
      :items='[
        { value: "5", label: "个章节" },
        { value: "Preset", label: "验证路径" },
        { value: "双语", label: "主线页面" },
      ]'
    />
  </template>
</SectionHero>

## 白皮书导览

<SectionIndex
  :items='[
    {
      title: "学院",
      description: "从模块地图、学习顺序与验证原则开始，为后续阅读建立统一坐标。",
      links: [
        { href: "/zh/academy/", label: "学院概览" },
        { href: "/zh/academy/module-atlas", label: "模块总览" },
      ],
    },
    {
      title: "架构",
      description: "查看仓库拓扑、主要代码表面，以及性能结论背后的方法论。",
      links: [
        { href: "/zh/architecture/", label: "架构概览" },
        { href: "/zh/architecture/repository-topology", label: "仓库拓扑" },
      ],
    },
    {
      title: "实践手册",
      description: "不移动现有文件，继续从这里进入快速开始、安装、性能分析、验证与最佳实践。",
      links: [
        { href: "/zh/playbook/", label: "实践手册概览" },
        { href: "/zh/getting-started/quickstart", label: "快速开始" },
      ],
    },
    {
      title: "参考",
      description: "需要精确命令、接口或常见问题答案时，从参考资料入口进入。",
      links: [
        { href: "/zh/reference/", label: "参考概览" },
        { href: "/zh/reference/api-reference", label: "API 入口" },
        { href: "/zh/reference/faq", label: "常见问题" },
      ],
    },
    {
      title: "研究",
      description: "相关工作、参考资料和演进记录共同说明这个仓库为何以当前方式收敛。",
      links: [
        { href: "/zh/research/", label: "研究概览" },
        { href: "/zh/research/related-work", label: "相关工作" },
      ],
    },
  ]'
/>

## 操作入口

<SectionIndex
  :items='[
    {
      title: "学习路径",
      description: "保留原有学习顺序及其关联练习，并将它们挂到新的信息架构之下。",
      links: [
        { href: "/zh/guides/learning-path", label: "学习路径" },
        { href: "/en/exercises/module-02-memory", label: "内存练习（英文）" },
      ],
    },
    {
      title: "SIMD 与并发",
      description: "练习仍然可以直达，但在主叙事中保持次级位置。",
      links: [
        { href: "/en/exercises/module-04-simd", label: "SIMD 练习（英文）" },
        { href: "/en/exercises/module-05-concurrency", label: "并发练习（英文）" },
      ],
    },
    {
      title: "验证与分析",
      description: "先决条件、性能分析、决策树与 Sanitizer 页面继续作为操作性支柱存在。",
      links: [
        { href: "/zh/getting-started/prerequisites", label: "先决条件" },
        { href: "/zh/guides/profiling-guide", label: "性能分析" },
        { href: "/zh/guides/optimization-decision-tree", label: "决策树" },
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
    构建完成后，运行 <code>ctest --preset=debug</code> 进行验证。详见<BaseAwareLink href="/zh/getting-started/quickstart">快速开始指南</BaseAwareLink>。
  </div>
</div>

## 次要入口

- [贡献流程](/zh/contributing/ai-workflow) 继续保留，但不再主导顶层叙事。
- [练习](/en/exercises/README) 仍可访问，但作为英文优先的次级实践表面存在。
