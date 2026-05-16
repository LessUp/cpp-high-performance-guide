---
layout: home
---

<SectionHero
  eyebrow="Performance Engineering"
  title="C++ High Performance Guide"
  lede="A practical C++20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling. Every topic is backed by runnable code and benchmark-oriented validation paths."
>
  Runnable examples, preset-driven builds, and verification guidance stay close together so readers can move from concept to measurement without leaving the docs.

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
        { "value": "C++20", "label": "standard" },
        { "value": "5", "label": "modules" },
        { "value": "Bilingual", "label": "docs" }
      ]'
    />
  </template>
</SectionHero>

## Learning Path

<SectionIndex
  :items='[
    {
      "title": "Memory Layout & Cache",
      "description": "Understand cache behavior, data locality, and memory alignment for optimal performance.",
      "links": [
        { "label": "Learning Path", "href": "./guides/learning-path" },
        { "label": "Exercises", "href": "./exercises/module-02-memory" }
      ]
    },
    {
      "title": "SIMD Optimization",
      "description": "Leverage SIMD instructions for vectorized computation and data parallelism.",
      "links": [
        { "label": "Learning Path", "href": "./guides/learning-path" },
        { "label": "Exercises", "href": "./exercises/module-04-simd" }
      ]
    },
    {
      "title": "Concurrency & Threading",
      "description": "Master std::atomic, memory ordering, and lock-free data structures.",
      "links": [
        { "label": "Learning Path", "href": "./guides/learning-path" },
        { "label": "Exercises", "href": "./exercises/module-05-concurrency" }
      ]
    },
    {
      "title": "Build System (CMake)",
      "description": "Modern CMake with presets, sanitizers, and cross-platform build configuration.",
      "links": [
        { "label": "Quick Start", "href": "./getting-started/quickstart" },
        { "label": "Prerequisites", "href": "./getting-started/prerequisites" }
      ]
    },
    {
      "title": "Profiling & Analysis",
      "description": "Performance analysis tools, benchmarking methodology, and optimization decision tree.",
      "links": [
        { "label": "Profiling", "href": "./guides/profiling-guide" },
        { "label": "Decision Tree", "href": "./guides/optimization-decision-tree" }
      ]
    },
    {
      "title": "Validation & Sanitizers",
      "description": "AddressSanitizer, ThreadSanitizer, UndefinedBehaviorSanitizer, and testing best practices.",
      "links": [
        { "label": "Sanitizers", "href": "./guides/validation" },
        { "label": "Best Practices", "href": "./guides/best-practices" }
      ]
    }
  ]'
/>

<FigureFrame
  label="Quick Start"
  caption="Build first, then verify with ctest"
  note="After building, run ctest --preset=debug to verify. See the Quick Start guide for prerequisites and toolchain details."
>
  <div class="command-block">
    <code>cmake --preset=debug && cmake --build build/debug</code>
  </div>
</FigureFrame>

<ReferenceList
  title="Further Reading"
  :items='[
    {
      "title": "Quick Start",
      "href": "./getting-started/quickstart",
      "meta": "Setup",
      "description": "Get the toolchain, presets, and first build steps in one place."
    },
    {
      "title": "Learning Path",
      "href": "./guides/learning-path",
      "meta": "Overview",
      "description": "Follow the repository topics in a suggested learning order."
    },
    {
      "title": "Validation & Sanitizers",
      "href": "./guides/validation",
      "meta": "Validation",
      "description": "Use tests and sanitizers before trusting performance claims."
    }
  ]'
/>
