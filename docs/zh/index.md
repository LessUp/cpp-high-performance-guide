---
layout: home
---

<SectionHero
  eyebrow="性能工程"
  title="C++ 高性能指南"
  lede="一份实用的 C++20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。每个主题都配有可运行的代码和面向基准测试的验证路径。"
>
  可运行示例、预设化构建和验证说明放在一起，方便读者从概念直接走到测量与验证。

  <template #actions>
    <a class="home-nav-link" href="./getting-started/quickstart">快速开始</a>
    <a class="home-nav-link" href="./guides/learning-path">学习路径</a>
    <a class="home-nav-link" href="https://github.com/LessUp/cpp-high-performance-guide">GitHub</a>
    <a class="home-nav-link" href="../en/">English</a>
  </template>

  <template #aside>
    <MetricStrip
      label="验证信号"
      :items='[
        { "value": "C++20", "label": "标准" },
        { "value": "5", "label": "个模块" },
        { "value": "双语", "label": "文档" }
      ]'
    />
  </template>
</SectionHero>

## 学习路径

<SectionIndex
  :items='[
    {
      "title": "内存布局与缓存",
      "description": "理解缓存行为、数据局部性和内存对齐，以实现最佳性能。",
      "links": [
        { "label": "学习路径", "href": "./guides/learning-path" },
        { "label": "练习（英文）", "href": "../en/exercises/module-02-memory" }
      ]
    },
    {
      "title": "SIMD 优化",
      "description": "利用 SIMD 指令实现向量化计算和数据并行。",
      "links": [
        { "label": "学习路径", "href": "./guides/learning-path" },
        { "label": "练习（英文）", "href": "../en/exercises/module-04-simd" }
      ]
    },
    {
      "title": "并发与线程",
      "description": "掌握 std::atomic、内存排序和无锁数据结构。",
      "links": [
        { "label": "学习路径", "href": "./guides/learning-path" },
        { "label": "练习（英文）", "href": "../en/exercises/module-05-concurrency" }
      ]
    },
    {
      "title": "构建系统（CMake）",
      "description": "现代 CMake 预设、Sanitizer 和跨平台构建配置。",
      "links": [
        { "label": "快速开始", "href": "./getting-started/quickstart" },
        { "label": "先决条件", "href": "./getting-started/prerequisites" }
      ]
    },
    {
      "title": "性能分析",
      "description": "性能分析工具、基准测试方法论和优化决策树。",
      "links": [
        { "label": "性能分析", "href": "./guides/profiling-guide" },
        { "label": "决策树", "href": "./guides/optimization-decision-tree" }
      ]
    },
    {
      "title": "验证与 Sanitizer",
      "description": "AddressSanitizer、ThreadSanitizer、UndefinedBehaviorSanitizer 和测试最佳实践。",
      "links": [
        { "label": "Sanitizer", "href": "./guides/validation" },
        { "label": "最佳实践", "href": "./guides/best-practices" }
      ]
    }
  ]'
/>

<FigureFrame
  label="快速开始"
  caption="先完成构建，再用 ctest 验证"
  note="构建完成后，运行 ctest --preset=debug 进行验证。先决条件和工具链细节见快速开始指南。"
>
  <div class="command-block">
    <code>cmake --preset=debug && cmake --build build/debug</code>
  </div>
</FigureFrame>

<ReferenceList
  title="延伸阅读"
  :items='[
    {
      "title": "快速开始",
      "href": "./getting-started/quickstart",
      "meta": "配置",
      "description": "集中查看工具链、预设和首次构建步骤。"
    },
    {
      "title": "学习路径",
      "href": "./guides/learning-path",
      "meta": "概览",
      "description": "按推荐顺序浏览仓库中的核心主题。"
    },
    {
      "title": "验证与 Sanitizer",
      "href": "./guides/validation",
      "meta": "验证",
      "description": "在信任性能结论之前，先运行测试和 Sanitizer。"
    }
  ]'
/>
