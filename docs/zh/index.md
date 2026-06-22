---
layout: home
---

<LandingHero
  badge="C++20 学习型白皮书入口"
  title="先学会验证"
  titleAccent="再谈性能"
  subtitle="这是一个以学习顺序为先的首页，面向希望从可运行示例、验证路径与证据标准进入现代 C++ 性能实践的读者。"
  intro="把这里当作白皮书入口页：先判断这份指南是否适合你，再按建议顺序学习，最后走完首次运行路线，然后再进入更深的参考与方法论页面。"
  actions-aria-label="首页主要操作"
  :actions='[
    { href: "/zh/getting-started/quickstart", label: "从快速开始进入", primary: true },
    { href: "/zh/guides/learning-path", label: "查看学习路径" },
    { href: "/zh/academy/", label: "阅读学院概览" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "查看 GitHub" },
  ]'
  guides-aria-label="建议阅读顺序"
  :guides='[
    {
      href: "/zh/academy/module-atlas",
      title: "先建立仓库地图，再讨论优化",
      description: "先弄清示例、测试与辅助头文件分别在哪里，再去判断某条性能结论是否站得住。",
    },
    {
      href: "/zh/academy/validation-doctrine",
      title: "先理解证据标准",
      description: "这份页面解释正确性、优化收益与并发安全各自需要什么级别的证明。",
    },
    {
      href: "/zh/architecture/performance-methodology",
      title: "基线稳定后再做测量",
      description: "当 debug 与 sanitizer 路线已经跑通，再进入 benchmark 与 profiler 方法论会更稳妥。",
    },
  ]'
  metrics-aria-label="首页证据指标"
  :metrics='[
    { value: "5", label: "教学模块" },
    { value: "6", label: "白皮书章节" },
    { value: "debug→ubsan", label: "验证阶梯" },
  ]'
/>

## 这份指南适合谁？

如果你需要的不是零散技巧，而是一条可核验的学习入口，这份指南就是为你准备的：

- 想系统复习现代 C++ 性能主题、但不希望只看到结论的工程师
- 需要把页面论断追溯到代码、测试和 preset 的评审者或面试官
- 未来会维护这个仓库，需要先建立整体地图的接手者

这个仓库把性能建议视为必须能够编译、测试、基准比较并被证伪的对象。理想状态下，你应当能从首页上的任何结论直接追到对应的源文件、preset、基准或测试目标，而不需要猜。

## 建议学习顺序

建议先建立理解框架，再进入专题深水区：

1. 先看[学院概览](/zh/academy/)，理解整套内容的教学组织方式。
2. 再读[模块总览](/zh/academy/module-atlas)，把模块与真实代码位置对上。
3. 接着看[验证原则](/zh/academy/validation-doctrine)，先知道什么才算证据充分。
4. 需要理解仓库结构与测量方法时，再进入[架构](/zh/architecture/)。
5. 日常查命令、排路线时，把[实践手册](/zh/playbook/)和[参考](/zh/reference/)当作随手册。

## 首次运行路线

第一次进入仓库时，优先走 preset 驱动的基线路线。这样最短，也最能保留项目原本的工程语义。

<div class="wp-figure-shell wp-quick-start">
  <div class="wp-meta-strip" title="快速开始">快速开始</div>
  <div class="wp-quick-start-body">
    <p>先把基线构建与测试跑通，再去读更激进的优化页面。这样学习顺序会稳，也更容易判断后续结论是不是可信。</p>
    <div class="wp-command-block">
      <code>cmake --preset=debug && cmake --build build/debug && ctest --preset=debug</code>
    </div>
    如需更完整的操作路线，请继续阅读<BaseAwareLink href="/zh/getting-started/quickstart">快速开始指南</BaseAwareLink>。
  </div>
</div>

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
      title: "算法",
      description: "高性能算法实现，附带复杂度分析、缓存感知权衡与可复现基准测试。",
      links: [
        { href: "/zh/algorithms/", label: "算法概览" },
        { href: "/zh/algorithms/sorting", label: "排序算法" },
        { href: "/zh/algorithms/hashing", label: "哈希算法" },
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
- **未来维护者：** 阅读[模块总览](/zh/academy/module-atlas)、[仓库拓扑](/zh/architecture/repository-topology)，以及 GitHub 上的 `CLAUDE.md`。
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

## 技术规格

| 层面 | 详情 |
| --- | --- |
| C++ 标准 | C++17 最低，C++20 在有益处时使用 |
| 构建系统 | CMake 3.20+，基于 preset |
| 平台 | 以 Linux 为主；macOS 和 Windows 通过跨平台头文件支持 |
| 编译器 | GCC 11+、Clang 14+、MSVC 2022+ |
| 测试 | GoogleTest + RapidCheck（基于性质的测试） |
| 基准测试 | Google Benchmark，`perf` 集成 |
| Sanitizer | ASan、TSan、UBSan，通过 CMake preset 启用 |
| 文档 | VitePress，双语（中/英），GitHub Pages 发布 |

## 次要入口

- [贡献流程](/zh/contributing/ai-workflow) 继续为仓库贡献者保留，但不属于主要的专家阅读弧线。
- [练习](/en/exercises/) 继续作为刻意练习表面存在，但有意保持为次级入口，而不是主要架构叙事。
