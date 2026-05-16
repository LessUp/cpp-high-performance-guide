---
layout: home
---

<SectionHero
  eyebrow="Whitepaper / Engineering Monograph"
  title="C++ High Performance Guide"
  lede="Runnable C++ performance engineering docs that connect build discipline, memory layout, SIMD, concurrency, profiling, and validation into one archive-ready technical narrative."
>
  This repository teaches high-performance C++ with runnable examples, but the docs surface is written for readers who want evidence: how modules relate, how claims are verified, and where to go next when a benchmark result needs context.

  <template #actions>
    <a class="home-nav-link" href="./getting-started/quickstart">Quick Start</a>
    <a class="home-nav-link" href="./guides/learning-path">Learning Path</a>
    <a class="home-nav-link" href="https://github.com/LessUp/cpp-high-performance-guide">GitHub</a>
    <a class="home-nav-link" href="../zh/">中文</a>
  </template>

  <template #aside>
    <MetricStrip
      label="Validation signals"
      :items='[
        { "value": "C++20", "label": "baseline language" },
        { "value": "5", "label": "core modules" },
        { "value": "Bilingual", "label": "entry docs" }
      ]'
    />
  </template>
</SectionHero>

## Learning Path

<SectionIndex
  :items='[
    {
      "title": "Memory Layout & Cache",
      "description": "Understand cache behavior, data locality, and memory alignment so optimization starts with data movement rather than slogans.",
      "links": [
        { "label": "Learning Path", "href": "./guides/learning-path" },
        { "label": "Exercises", "href": "./exercises/module-02-memory" }
      ]
    },
    {
      "title": "SIMD Optimization",
      "description": "Follow vectorization patterns, wrapper trade-offs, and benchmark discipline before claiming a throughput win.",
      "links": [
        { "label": "Learning Path", "href": "./guides/learning-path" },
        { "label": "Exercises", "href": "./exercises/module-04-simd" }
      ]
    },
    {
      "title": "Concurrency & Threading",
      "description": "Read the repository’s concurrency material as an execution-model guide: atomics, memory ordering, and contention-aware design.",
      "links": [
        { "label": "Learning Path", "href": "./guides/learning-path" },
        { "label": "Exercises", "href": "./exercises/module-05-concurrency" }
      ]
    },
    {
      "title": "Build System (CMake)",
      "description": "Use preset-driven builds, sanitizer variants, and repeatable test entry points as part of the performance story.",
      "links": [
        { "label": "Quick Start", "href": "./getting-started/quickstart" },
        { "label": "Prerequisites", "href": "./getting-started/prerequisites" }
      ]
    },
    {
      "title": "Profiling & Analysis",
      "description": "Move from measurement setup to interpretation with profiling workflows and decision trees that explain why a result matters.",
      "links": [
        { "label": "Profiling", "href": "./guides/profiling-guide" },
        { "label": "Decision Tree", "href": "./guides/optimization-decision-tree" }
      ]
    },
    {
      "title": "Validation & Sanitizers",
      "description": "Keep optimizations honest with sanitizer passes, test commands, and a bias toward evidence over anecdote.",
      "links": [
        { "label": "Sanitizers", "href": "./guides/validation" },
        { "label": "Best Practices", "href": "./guides/best-practices" }
      ]
    }
  ]'
/>

<FigureFrame
  label="Default verification path"
  caption="Build first, then confirm behavior before benchmarking"
  note="Use the preset-driven debug path as the default local safety net, then move to sanitizer or release presets when deeper validation is needed."
>
  <div class="command-block">
    <code>cmake --preset=debug && cmake --build build/debug && ctest --preset=debug</code>
  </div>
</FigureFrame>

<ReferenceList
  title="Reference trail"
  :items='[
    {
      "title": "Validation & Sanitizers",
      "href": "./guides/validation",
      "meta": "Evidence standard",
      "description": "Default build, test, and sanitizer paths that keep optimization claims reproducible."
    },
    {
      "title": "Profiling Guide",
      "href": "./guides/profiling-guide",
      "meta": "Measurement workflow",
      "description": "Tooling and interpretation guidance for tracing hotspots before changing code."
    },
    {
      "title": "API Reference",
      "href": "./reference/api-reference",
      "meta": "Reference surface",
      "description": "Entry points for reusable helpers, wrappers, and troubleshooting-oriented reference material."
    }
  ]'
/>
