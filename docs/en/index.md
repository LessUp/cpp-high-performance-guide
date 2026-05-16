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

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">Memory Layout & Cache</div>
    <div class="feature-card-desc">
      Understand cache behavior, data locality, and memory alignment so optimization starts with data movement rather than slogans.
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">Learning Path</a>
      <a href="./exercises/module-02-memory" class="feature-tag">Exercises</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">SIMD Optimization</div>
    <div class="feature-card-desc">
      Follow vectorization patterns, wrapper trade-offs, and benchmark discipline before claiming a throughput win.
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">Learning Path</a>
      <a href="./exercises/module-04-simd" class="feature-tag">Exercises</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Concurrency & Threading</div>
    <div class="feature-card-desc">
      Read the repository’s concurrency material as an execution-model guide: atomics, memory ordering, and contention-aware design.
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">Learning Path</a>
      <a href="./exercises/module-05-concurrency" class="feature-tag">Exercises</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Build System (CMake)</div>
    <div class="feature-card-desc">
      Use preset-driven builds, sanitizer variants, and repeatable test entry points as part of the performance story.
    </div>
    <div class="feature-tags">
      <a href="./getting-started/quickstart" class="feature-tag">Quick Start</a>
      <a href="./getting-started/prerequisites" class="feature-tag">Prerequisites</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Profiling & Analysis</div>
    <div class="feature-card-desc">
      Move from measurement setup to interpretation with profiling workflows and decision trees that explain why a result matters.
    </div>
    <div class="feature-tags">
      <a href="./guides/profiling-guide" class="feature-tag">Profiling</a>
      <a href="./guides/optimization-decision-tree" class="feature-tag">Decision Tree</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Validation & Sanitizers</div>
    <div class="feature-card-desc">
      Keep optimizations honest with sanitizer passes, test commands, and a bias toward evidence over anecdote.
    </div>
    <div class="feature-tags">
      <a href="./guides/validation" class="feature-tag">Sanitizers</a>
      <a href="./guides/best-practices" class="feature-tag">Best Practices</a>
    </div>
  </div>
</div>

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
