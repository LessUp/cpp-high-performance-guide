---
layout: home
---

<SectionHero
  badge="C++20 • 经测量"
  title="C++ 高性能指南"
  subtitle="面向严肃性能工作的可运行架构札记"
  intro="这个仓库把性能建议视为必须能够编译、测试、基准比较并被证伪的对象。文档站是一层覆盖在可执行 C++ 模块、preset 驱动验证与研究说明之上的白皮书，用来解释哪些内容值得相信，以及为什么。"
  links-aria-label="落地页链接"
  :links='[
     { href: "/zh/academy/", label: "学院" },
     { href: "/zh/architecture/", label: "架构" },
     { href: "/zh/playbook/", label: "实践手册" },
     { href: "/zh/research/", label: "研究" },
     { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
     { href: "/en/", label: "English" },
  ]'
>
  <template #aside>
    <MetricStrip
      aria-label="项目指标"
      :items='[
        { value: "5", label: "个教学模块" },
        { value: "debug→ubsan", label: "验证阶梯" },
        { value: "academy→research", label: "阅读弧线" },
      ]'
    />
  </template>
</SectionHero>

## 论点

只有当论证链条保持可见时，性能指导才有可信度。在这个仓库里，每一层都可以被检查：

- **代码** 位于 `examples/` 下可运行的示例模块中
- **证据** 位于 CMake preset、单元测试、性质测试、sanitizer 运行与基准可执行文件中
- **解释** 位于学院、架构、实践手册、参考与研究部分中
- **治理** 位于 `openspec/` 中，在非平凡改动落地之前先记录意图

一个熟练读者应当可以从本页上的任何结论，直接追到对应的源文件、preset、基准或测试目标，而不需要猜测。

## 验证主张

| 结论类型 | 在本仓库中的最低证据 | 主要入口 |
| --- | --- | --- |
| “代码仍然正确工作” | `debug` 配置、构建与 `ctest --preset=debug` | [验证原则](/zh/academy/validation-doctrine) |
| “这个优化具有代表性” | `release` 或 `relwithdebinfo` 构建，加基准或 profiler 输出 | [性能方法论](/zh/architecture/performance-methodology) |
| “并发路径是安全的” | 单元或性质测试，加上同步改动时的 `tsan` | [验证与 Sanitizer](/zh/guides/validation) |
| “文档仍然是最新的” | 链接、导航与 Pages 构建输出与仓库现状保持一致 | [实践手册](/zh/playbook/) |

## 白皮书地图

<SectionIndex
  :items='[
    {
      title: "学院",
      description: "理解如何阅读仓库、教学模块如何映射到代码，以及什么证据才算合格。",
      links: [
        { href: "/zh/academy/", label: "学院概览" },
        { href: "/zh/academy/module-atlas", label: "模块总览" },
        { href: "/zh/academy/validation-doctrine", label: "验证原则" },
      ],
    },
    {
      title: "架构",
      description: "检查仓库拓扑、preset 驱动验证，以及每条性能结论背后的方法论。",
      links: [
        { href: "/zh/architecture/", label: "架构概览" },
        { href: "/zh/architecture/repository-topology", label: "仓库拓扑" },
        { href: "/zh/architecture/performance-methodology", label: "性能方法论" },
      ],
    },
    {
      title: "实践手册",
      description: "无需四处寻找命令。实践手册把配置、学习、性能分析与加固路线重新聚合起来。",
      links: [
        { href: "/zh/playbook/", label: "实践手册概览" },
        { href: "/zh/getting-started/quickstart", label: "快速开始" },
        { href: "/zh/guides/profiling-guide", label: "性能分析指南" },
      ],
    },
    {
      title: "参考",
      description: "当你更需要精确命令、接口与故障路径，而不是叙事时，从这里进入。",
      links: [
        { href: "/zh/reference/", label: "参考概览" },
        { href: "/zh/reference/api-reference", label: "API 入口" },
        { href: "/zh/reference/faq", label: "常见问题" },
      ],
    },
    {
      title: "研究",
      description: "查看外部文献、公开仓库与演进说明，理解本项目为何以当前方式收口为一份可归档工程成果。",
      links: [
        { href: "/zh/research/", label: "研究概览" },
        { href: "/zh/research/related-work", label: "相关工作" },
        { href: "/zh/research/references", label: "参考资料" },
      ],
    },
  ]'
/>

## 模块地图

| 模块 | 主要代码表面 | 典型证据 | 最佳入口页 |
| --- | --- | --- | --- |
| 01. 现代 CMake | `examples/01-cmake-modern/`、`CMakeLists.txt`、`cmake/` | 配置与目标级构建 | [快速开始](/zh/getting-started/quickstart) |
| 02. 内存与缓存 | `examples/02-memory-cache/`、`tests/unit/memory/`、`tests/property/memory_properties.cpp` | 缓存敏感基准、`perf stat` | [模块总览](/zh/academy/module-atlas) |
| 03. 现代 C++ | `examples/03-modern-cpp/`、`tests/unit/modern_cpp/` | 基准对比与单元测试 | [学习路径](/zh/guides/learning-path) |
| 04. SIMD 向量化 | `examples/04-simd-vectorization/`、`tests/unit/simd/`、`tests/property/simd_properties.cpp` | 向量化检查与 `simd_bench` | [优化决策树](/zh/guides/optimization-decision-tree) |
| 05. 并发 | `examples/05-concurrency/`、`tests/unit/concurrency/`、`tests/property/concurrency_properties.cpp` | `tsan`、队列测试、扩展性基准 | [验证原则](/zh/academy/validation-doctrine) |

<div class="wp-figure-shell wp-quick-start">
  <div class="wp-meta-strip" title="快速开始">快速开始</div>
  <div class="wp-quick-start-body">
    <p>优先采用 preset 驱动路径。这是最短、同时仍保留架构意图的进入方式。</p>
    <div class="wp-command-block">
      <code>cmake --preset=debug && cmake --build build/debug && ctest --preset=debug</code>
    </div>
    如需更完整的操作路线，请继续阅读<BaseAwareLink href="/zh/getting-started/quickstart">快速开始指南</BaseAwareLink>。
  </div>
</div>

## 面向熟练读者的提示

- **面试官或评审者：** 先读[架构](/zh/architecture/)和[研究](/zh/research/)。这两部分最快暴露验证模型与外部上下文。
- **未来维护者：** 阅读[模块总览](/zh/academy/module-atlas)、[仓库拓扑](/zh/architecture/repository-topology)，以及 GitHub 上的 `AGENTS.md` 与 `CLAUDE.md`。
- **实践工程师：** 先用[实践手册](/zh/playbook/)获取命令，再在基线通过后进入 profiling 与 sanitizer 路线。

## 操作路线

<SectionIndex
  :items='[
    {
      title: "学习路径",
      description: "保留原有主题顺序，但把它视为导读程序，而不是唯一的文档策略。",
      links: [
        { href: "/zh/guides/learning-path", label: "学习路径" },
        { href: "/en/exercises/module-02-memory", label: "内存练习（英文）" },
      ],
    },
    {
      title: "SIMD 与并发",
      description: "当你已掌握基础并想直接查看 ISA 或线程取舍时，可以直接切入性能最重的模块。",
      links: [
        { href: "/en/exercises/module-04-simd", label: "SIMD 练习（英文）" },
        { href: "/en/exercises/module-05-concurrency", label: "并发练习（英文）" },
      ],
    },
    {
      title: "验证与分析",
      description: "使用这些操作页复现、分析、基准比较并加固改动，且术语与整份白皮书保持一致。",
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

## 次要入口

- [贡献流程](/zh/contributing/ai-workflow) 继续为仓库贡献者保留，但不属于主要的专家阅读弧线。
- [练习](/en/exercises/) 继续作为刻意练习表面存在，但有意保持为次级入口，而不是主要架构叙事。
