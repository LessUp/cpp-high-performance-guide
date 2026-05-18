---
layout: home
---

<LandingHero
  badge="C++20 • Measured • Validated"
  title="High Performance"
  titleAccent="C++ Guide"
  subtitle="Runnable architecture notes for serious performance work. This repository treats performance advice as something to compile, test, benchmark, and falsify."
  :links='[
    { href: "/en/getting-started/quickstart", label: "Start Learning", primary: true },
    { href: "/en/academy/", label: "Academy" },
    { href: "/en/architecture/", label: "Architecture" },
    { href: "https://github.com/LessUp/cpp-high-performance-guide", label: "GitHub" },
  ]'
  :metrics='[
    { value: "5", label: "Teaching Modules" },
    { value: "6", label: "Whitepaper Sections" },
    { value: "Debug→UBSan", label: "Validation Ladder" },
  ]'
/>

## Thesis

Performance guidance becomes credible only when the argumentative chain stays visible. In this repository, each layer is inspectable:

- **code** lives in runnable example modules under `examples/`
- **evidence** lives in CMake presets, unit tests, property tests, sanitizer runs, and benchmark executables
- **interpretation** lives in the academy, architecture, playbook, reference, and research sections
- **governance** lives in `openspec/`, where non-trivial changes record intent before they land

An expert reader should be able to move from a claim on this page to the corresponding source file, preset, benchmark, or test target without guesswork.

## Validation claims

| Claim type | Minimum evidence in this repository | Primary route |
| --- | --- | --- |
| "the code still works" | `debug` configure, build, and `ctest --preset=debug` | [Validation Doctrine](/en/academy/validation-doctrine) |
| "the optimization is representative" | `release` or `relwithdebinfo` build plus benchmark or profiler output | [Performance Methodology](/en/architecture/performance-methodology) |
| "the concurrent path is safe" | unit or property tests plus `tsan` when synchronization changed | [Validation & Sanitizers](/en/guides/validation) |
| "the docs are current" | links, navigation, and built Pages output stay aligned with the live repository | [Playbook](/en/playbook/) |

## Whitepaper map

<SectionIndex
  :items='[
    {
      title: "Academy",
      description: "Learn how to read the repository, how the teaching modules map to code, and what counts as acceptable evidence.",
      links: [
        { href: "/en/academy/", label: "Academy overview" },
        { href: "/en/academy/module-atlas", label: "Module Atlas" },
        { href: "/en/academy/validation-doctrine", label: "Validation Doctrine" },
      ],
    },
    {
      title: "Architecture",
      description: "Inspect repository topology, preset-driven validation, and the methodology behind every performance claim.",
      links: [
        { href: "/en/architecture/", label: "Architecture overview" },
        { href: "/en/architecture/repository-topology", label: "Repository Topology" },
        { href: "/en/architecture/performance-methodology", label: "Performance Methodology" },
      ],
    },
    {
      title: "Algorithms",
      description: "High-performance algorithm implementations with complexity analysis, cache-aware trade-offs, and reproducible benchmarks.",
      links: [
        { href: "/en/algorithms/", label: "Algorithms overview" },
        { href: "/en/algorithms/sorting", label: "Sorting" },
        { href: "/en/algorithms/hashing", label: "Hashing" },
      ],
    },
    {
      title: "Playbook",
      description: "Run the repository without hunting for commands. The playbook regroups setup, learning, profiling, and hardening routes.",
      links: [
        { href: "/en/playbook/", label: "Playbook overview" },
        { href: "/en/getting-started/quickstart", label: "Quick Start" },
        { href: "/en/guides/profiling-guide", label: "Profiling Guide" },
      ],
    },
    {
      title: "Reference",
      description: "Use the reference hub for API entry points, troubleshooting, and exact command surfaces when you need precision more than narrative.",
      links: [
        { href: "/en/reference/", label: "Reference overview" },
        { href: "/en/reference/api-reference", label: "API Reference" },
        { href: "/en/reference/faq", label: "FAQ" },
      ],
    },
    {
      title: "Research",
      description: "See the external literature, public repositories, and evolution notes that frame this project as an archive-ready engineering artifact.",
      links: [
        { href: "/en/research/", label: "Research overview" },
        { href: "/en/research/related-work", label: "Related Work" },
        { href: "/en/research/references", label: "References" },
      ],
    },
  ]'
/>

## Module map

| Module | Primary code surface | Typical evidence | Best entry page |
| --- | --- | --- | --- |
| 01. Modern CMake | `examples/01-cmake-modern/`, `CMakeLists.txt`, `cmake/` | configure and target-level builds | [Quick Start](/en/getting-started/quickstart) |
| 02. Memory & cache | `examples/02-memory-cache/`, `tests/unit/memory/`, `tests/property/memory_properties.cpp` | cache-sensitive benchmarks, `perf stat` | [Module Atlas](/en/academy/module-atlas) |
| 03. Modern C++ | `examples/03-modern-cpp/`, `tests/unit/modern_cpp/` | benchmark comparisons and unit tests | [Learning Path](/en/guides/learning-path) |
| 04. SIMD vectorization | `examples/04-simd-vectorization/`, `tests/unit/simd/`, `tests/property/simd_properties.cpp` | vectorization checks and `simd_bench` | [Optimization Decision Tree](/en/guides/optimization-decision-tree) |
| 05. Concurrency | `examples/05-concurrency/`, `tests/unit/concurrency/`, `tests/property/concurrency_properties.cpp` | `tsan`, queue tests, scaling benchmarks | [Validation Doctrine](/en/academy/validation-doctrine) |

<div class="wp-figure-shell wp-quick-start">
  <div class="wp-meta-strip" title="Quick Start">Quick Start</div>
  <div class="wp-quick-start-body">
    <p>Use the preset-driven path first. It is the shortest route that still preserves architectural intent.</p>
    <div class="wp-command-block">
      <code>cmake --preset=debug && cmake --build build/debug && ctest --preset=debug</code>
    </div>
    For a broader operational path, continue with the <BaseAwareLink href="/en/getting-started/quickstart">Quick Start guide</BaseAwareLink>.
  </div>
</div>

## Expert reader callouts

- **Interviewer or reviewer:** start with [Architecture](/en/architecture/) and [Research](/en/research/). Those pages expose the validation model and external context fastest.
- **Future maintainer:** read [Module Atlas](/en/academy/module-atlas), [Repository Topology](/en/architecture/repository-topology), and the repository-level guidance in `AGENTS.md` and `CLAUDE.md` on GitHub.
- **Practicing engineer:** use the [Playbook](/en/playbook/) for commands, then move to profiling and sanitizer routes only after a baseline run succeeds.

## Operational routes

<SectionIndex
  :items='[
    {
      title: "Learning path",
      description: "Keep the original study sequence visible, but treat it as a guided reading program rather than the whole documentation strategy.",
      links: [
        { href: "/en/guides/learning-path", label: "Learning Path" },
        { href: "/en/exercises/module-02-memory", label: "Memory exercise" },
      ],
    },
    {
      title: "SIMD & concurrency",
      description: "Reach the performance-heavy modules directly when you already know the fundamentals and want to inspect ISA or threading trade-offs.",
      links: [
        { href: "/en/exercises/module-04-simd", label: "SIMD exercise" },
        { href: "/en/exercises/module-05-concurrency", label: "Concurrency exercise" },
      ],
    },
    {
      title: "Validation & analysis",
      description: "Use the operational guides to reproduce, profile, benchmark, and harden changes with the same vocabulary used throughout the whitepaper.",
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

## Technical specifications

| Surface | Detail |
| --- | --- |
| C++ standard | C++17 minimum, C++20 where beneficial |
| Build system | CMake 3.20+ with presets |
| Platforms | Linux primary; macOS and Windows via cross-platform headers |
| Compilers | GCC 11+, Clang 14+, MSVC 2022+ |
| Testing | GoogleTest + RapidCheck (property-based) |
| Benchmarks | Google Benchmark, `perf` integration |
| Sanitizers | ASan, TSan, UBSan via CMake presets |
| Documentation | VitePress, bilingual (en/zh), GitHub Pages |

## Secondary routes

- [Contributing workflow](/en/contributing/ai-workflow) remains available for repository contributors, but it is not part of the main expert-reading arc.
- [Exercises](/en/exercises/) remain intentionally secondary. They are practice surfaces, not the primary architectural narrative.
