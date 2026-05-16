---
layout: home
---

<SectionHero
  badge="C++"
  title="C++ High Performance Guide"
  subtitle="Technical Whitepaper"
  intro="A practical C++20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling. The new whitepaper path moves from academy to architecture, playbook, reference, and research."
  links-aria-label="Landing page links"
  :links='[
    { href: "/en/academy/", label: "Academy" },
    { href: "/en/playbook/", label: "Playbook" },
    { href: "/en/reference/", label: "Reference" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
    { href: "/zh/", label: "中文" },
  ]'
>
  <template #aside>
    <MetricStrip
      aria-label="Project metrics"
      :items='[
        { value: "5", label: "sections" },
        { value: "Preset", label: "validation" },
        { value: "Bilingual", label: "surface" },
      ]'
    />
  </template>
</SectionHero>

## Whitepaper map

<SectionIndex
  :items='[
    {
      title: "Academy",
      description: "Start with the module map, learning posture, and the validation habits that frame the rest of the repository.",
      links: [
        { href: "/en/academy/", label: "Academy overview" },
        { href: "/en/academy/module-atlas", label: "Module Atlas" },
      ],
    },
    {
      title: "Architecture",
      description: "Inspect repository topology, the major code surfaces, and the methodology behind performance claims.",
      links: [
        { href: "/en/architecture/", label: "Architecture overview" },
        { href: "/en/architecture/repository-topology", label: "Repository Topology" },
      ],
    },
    {
      title: "Playbook",
      description: "Run the practical guides without moving files: quick start, installation, profiling, validation, and best practices stay reachable here.",
      links: [
        { href: "/en/playbook/", label: "Playbook overview" },
        { href: "/en/getting-started/quickstart", label: "Quick Start" },
      ],
    },
    {
      title: "Reference",
      description: "Use the FAQ, troubleshooting notes, and API hubs when you need exact commands or interfaces.",
      links: [
        { href: "/en/reference/", label: "Reference overview" },
        { href: "/en/reference/api-reference", label: "API Reference" },
        { href: "/en/reference/faq", label: "FAQ" },
      ],
    },
    {
      title: "Research",
      description: "Follow the related work, references, and evolution notes that position the repository as a durable technical artifact.",
      links: [
        { href: "/en/research/", label: "Research overview" },
        { href: "/en/research/related-work", label: "Related Work" },
      ],
    },
  ]'
/>

## Operational routes

<SectionIndex
  :items='[
    {
      title: "Learning Path",
      description: "Keep the original study sequence and its linked exercises reachable through the new IA.",
      links: [
        { href: "/en/guides/learning-path", label: "Learning Path" },
        { href: "/en/exercises/module-02-memory", label: "Memory exercise" },
      ],
    },
    {
      title: "SIMD & Concurrency",
      description: "Reach the English-first exercises directly while keeping them secondary to the main narrative.",
      links: [
        { href: "/en/exercises/module-04-simd", label: "SIMD exercise" },
        { href: "/en/exercises/module-05-concurrency", label: "Concurrency exercise" },
      ],
    },
    {
      title: "Validation & Analysis",
      description: "Operational guides still anchor prerequisites, profiling, decision-making, and sanitizer-backed hardening.",
      links: [
        { href: "/en/getting-started/prerequisites", label: "Prerequisites" },
        { href: "/en/guides/profiling-guide", label: "Profiling" },
        { href: "/en/guides/optimization-decision-tree", label: "Decision Tree" },
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

## Secondary routes

- [Contributing workflow](/en/contributing/ai-workflow) remains available but sits outside the main reading sequence.
- [Exercises](/en/exercises/README) stay reachable as a secondary, English-first practice surface.
