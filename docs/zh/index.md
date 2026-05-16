---
layout: home
---

<SectionHero
  eyebrow="白皮书 / 工程专著"
  title="C++ 高性能指南"
  lede="一组可运行的 C++ 性能工程文档，把构建纪律、内存布局、SIMD、并发、性能分析与验证路径串成一条清晰的技术叙事。"
>
  这份仓库不仅提供可运行示例，也面向需要快速判断工程质量的读者：模块如何衔接、性能结论如何验证、以及在看到一条基准结果后应该继续读哪里。

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
        { "value": "C++20", "label": "基础语言标准" },
        { "value": "5", "label": "核心模块" },
        { "value": "双语", "label": "入口文档" }
      ]'
    />
  </template>
</SectionHero>

## 学习路径

<SectionIndex
  :items='[
    {
      "title": "内存布局与缓存",
      "description": "从数据移动与缓存命中率出发理解优化，而不是只停留在“更快”这类抽象口号上。",
      "links": [
        { "label": "学习路径", "href": "./guides/learning-path" },
        { "label": "练习（英文）", "href": "../en/exercises/module-02-memory" }
      ]
    },
    {
      "title": "SIMD 优化",
      "description": "结合向量化模式、封装权衡与基准方法，理解吞吐提升何时可信、何时只是噪声。",
      "links": [
        { "label": "学习路径", "href": "./guides/learning-path" },
        { "label": "练习（英文）", "href": "../en/exercises/module-04-simd" }
      ]
    },
    {
      "title": "并发与线程",
      "description": "将并发章节视作执行模型指南：原子操作、内存序以及围绕争用成本的设计思维。",
      "links": [
        { "label": "学习路径", "href": "./guides/learning-path" },
        { "label": "练习（英文）", "href": "../en/exercises/module-05-concurrency" }
      ]
    },
    {
      "title": "构建系统（CMake）",
      "description": "通过预设、Sanitizer 变体和统一入口命令，把构建配置也纳入性能工程的证据链。",
      "links": [
        { "label": "快速开始", "href": "./getting-started/quickstart" },
        { "label": "先决条件", "href": "./getting-started/prerequisites" }
      ]
    },
    {
      "title": "性能分析",
      "description": "从测量搭建到结果解读，借助性能分析流程与决策树回答“瓶颈为什么在这里”。",
      "links": [
        { "label": "性能分析", "href": "./guides/profiling-guide" },
        { "label": "决策树", "href": "./guides/optimization-decision-tree" }
      ]
    },
    {
      "title": "验证与 Sanitizer",
      "description": "用测试命令、Sanitizer 与可重复流程约束优化结论，让文档保持“先验证、后宣称”。",
      "links": [
        { "label": "Sanitizer", "href": "./guides/validation" },
        { "label": "最佳实践", "href": "./guides/best-practices" }
      ]
    }
  ]'
/>

<FigureFrame
  label="默认验证路径"
  caption="先完成构建与测试，再进入基准阶段"
  note="本地默认入口是 debug 预设；需要更深验证时，再切换到 sanitizer 或 release 预设。"
>
  <div class="command-block">
    <code>cmake --preset=debug && cmake --build build/debug && ctest --preset=debug</code>
  </div>
</FigureFrame>

<ReferenceList
  title="参考脉络"
  :items='[
    {
      "title": "验证与 Sanitizer",
      "href": "./guides/validation",
      "meta": "证据标准",
      "description": "默认构建、测试与 sanitizer 路径，保证优化结论可复现。"
    },
    {
      "title": "性能分析指南",
      "href": "./guides/profiling-guide",
      "meta": "测量流程",
      "description": "在修改代码之前，先用工具和解读方法明确热点来源。"
    },
    {
      "title": "API 入口",
      "href": "./reference/api-reference",
      "meta": "参考资料",
      "description": "集中查看共享工具、包装层以及面向排障的参考内容。"
    }
  ]'
/>
