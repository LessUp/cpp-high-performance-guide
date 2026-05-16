---
layout: home
---

<SectionHero
  badge="C++"
  title="C++ High Performance Guide"
  subtitle="Performance Engineering"
  intro="A practical C++20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling. Every topic is backed by runnable code and benchmark-oriented validation paths."
  links-aria-label="Landing page links"
  :links='[
    { href: "/en/getting-started/quickstart", label: "Quick Start" },
    { href: "/en/guides/learning-path", label: "Guides" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
    { href: "/zh/", label: "中文" },
  ]'
>
  <template #aside>
    <MetricStrip
      aria-label="Project metrics"
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
        { href: "/en/guides/learning-path", label: "Learning Path" },
        { href: "/en/exercises/module-02-memory", label: "Exercises" },
      ],
    },
    {
      title: "SIMD Optimization",
      description: "Leverage SIMD instructions for vectorized computation and data parallelism.",
      links: [
        { href: "/en/guides/learning-path", label: "Learning Path" },
        { href: "/en/exercises/module-04-simd", label: "Exercises" },
      ],
    },
    {
      title: "Concurrency & Threading",
      description: "Master std::atomic, memory ordering, and lock-free data structures.",
      links: [
        { href: "/en/guides/learning-path", label: "Learning Path" },
        { href: "/en/exercises/module-05-concurrency", label: "Exercises" },
      ],
    },
    {
      title: "Build System (CMake)",
      description: "Modern CMake with presets, sanitizers, and cross-platform build configuration.",
      links: [
        { href: "/en/getting-started/quickstart", label: "Quick Start" },
        { href: "/en/getting-started/prerequisites", label: "Prerequisites" },
      ],
    },
    {
      title: "Profiling & Analysis",
      description: "Performance analysis tools, benchmarking methodology, and optimization decision tree.",
      links: [
        { href: "/en/guides/profiling-guide", label: "Profiling" },
        { href: "/en/guides/optimization-decision-tree", label: "Decision Tree" },
      ],
    },
    {
      title: "Validation & Sanitizers",
      description: "AddressSanitizer, ThreadSanitizer, UndefinedBehaviorSanitizer, and testing best practices.",
      links: [
        { href: "/en/guides/validation", label: "Sanitizers" },
        { href: "/en/guides/best-practices", label: "Best Practices" },
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
    After building, run <code>ctest --preset=debug</code> to verify. See the <BaseAwareLink href="/en/getting-started/quickstart">Quick Start guide</BaseAwareLink> for details.
  </div>
</div>
