---
layout: home
---

<SectionHero
  badge="C++"
  title="C++ High Performance Guide"
  subtitle="Performance Engineering"
  intro="A practical C++20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling. Every topic is backed by runnable code and benchmark-oriented validation paths."
  :links='[
    { href: "./getting-started/quickstart", label: "Quick Start" },
    { href: "./guides/learning-path", label: "Guides" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
    { href: "../zh/", label: "中文" },
  ]'
>
  <template #aside>
    <MetricStrip
      :items='[
        { value: "C++20", label: "standard" },
        { value: "5", label: "modules" },
        { value: "Bilingual", label: "docs" },
      ]'
    />
  </template>
</SectionHero>

## Learning Path

<SectionIndex
  :items='[
    {
      title: "Memory Layout & Cache",
      description: "Understand cache behavior, data locality, and memory alignment for optimal performance.",
      links: [
        { href: "./guides/learning-path", label: "Learning Path" },
        { href: "./exercises/module-02-memory", label: "Exercises" },
      ],
    },
    {
      title: "SIMD Optimization",
      description: "Leverage SIMD instructions for vectorized computation and data parallelism.",
      links: [
        { href: "./guides/learning-path", label: "Learning Path" },
        { href: "./exercises/module-04-simd", label: "Exercises" },
      ],
    },
    {
      title: "Concurrency & Threading",
      description: "Master std::atomic, memory ordering, and lock-free data structures.",
      links: [
        { href: "./guides/learning-path", label: "Learning Path" },
        { href: "./exercises/module-05-concurrency", label: "Exercises" },
      ],
    },
    {
      title: "Build System (CMake)",
      description: "Modern CMake with presets, sanitizers, and cross-platform build configuration.",
      links: [
        { href: "./getting-started/quickstart", label: "Quick Start" },
        { href: "./getting-started/prerequisites", label: "Prerequisites" },
      ],
    },
    {
      title: "Profiling & Analysis",
      description: "Performance analysis tools, benchmarking methodology, and optimization decision tree.",
      links: [
        { href: "./guides/profiling-guide", label: "Profiling" },
        { href: "./guides/optimization-decision-tree", label: "Decision Tree" },
      ],
    },
    {
      title: "Validation & Sanitizers",
      description: "AddressSanitizer, ThreadSanitizer, UndefinedBehaviorSanitizer, and testing best practices.",
      links: [
        { href: "./guides/validation", label: "Sanitizers" },
        { href: "./guides/best-practices", label: "Best Practices" },
      ],
    },
  ]'
/>

<div class="wp-figure-shell wp-quick-start">
  <div class="wp-meta-strip" title="Quick Start">Quick Start</div>
  <div class="wp-quick-start-body">
    <div class="wp-command-block">
      <code>cmake --preset=debug && cmake --build build/debug</code>
    </div>
    After building, run <code>ctest --preset=debug</code> to verify. See the <a href="./getting-started/quickstart">Quick Start guide</a> for details.
  </div>
</div>
