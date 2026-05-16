---
layout: home
---

<div class="home-header">
  <div class="home-header-left">
    <div class="home-logo">C++</div>
    <div>
      <span class="home-title">C++ High Performance Guide</span>
      <span class="home-subtitle">Performance Engineering</span>
    </div>
  </div>
  <div class="home-nav">
    <a href="./getting-started/quickstart">Quick Start</a>
    <a href="./guides/learning-path">Guides</a>
    <a href="https://github.com/LessUp/cpp-high-performance-guide">GitHub</a>
    <a href="../zh/">中文</a>
  </div>
</div>

<div class="home-intro-row">
  <div class="home-intro">
    A practical C++20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling. Every topic is backed by runnable code and benchmark-oriented validation paths.
  </div>
  <div class="home-stats">
    <span><strong>C++20</strong> standard</span>
    <span><strong>5</strong> modules</span>
    <span><strong>Bilingual</strong> docs</span>
  </div>
</div>

## Learning Path

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">Memory Layout & Cache</div>
    <div class="feature-card-desc">
      Understand cache behavior, data locality, and memory alignment for optimal performance.
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">Learning Path</a>
      <a href="./exercises/module-02-memory" class="feature-tag">Exercises</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">SIMD Optimization</div>
    <div class="feature-card-desc">
      Leverage SIMD instructions for vectorized computation and data parallelism.
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">Learning Path</a>
      <a href="./exercises/module-04-simd" class="feature-tag">Exercises</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Concurrency & Threading</div>
    <div class="feature-card-desc">
      Master std::atomic, memory ordering, and lock-free data structures.
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">Learning Path</a>
      <a href="./exercises/module-05-concurrency" class="feature-tag">Exercises</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Build System (CMake)</div>
    <div class="feature-card-desc">
      Modern CMake with presets, sanitizers, and cross-platform build configuration.
    </div>
    <div class="feature-tags">
      <a href="./getting-started/quickstart" class="feature-tag">Quick Start</a>
      <a href="./getting-started/prerequisites" class="feature-tag">Prerequisites</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Profiling & Analysis</div>
    <div class="feature-card-desc">
      Performance analysis tools, benchmarking methodology, and optimization decision tree.
    </div>
    <div class="feature-tags">
      <a href="./guides/profiling-guide" class="feature-tag">Profiling</a>
      <a href="./guides/optimization-decision-tree" class="feature-tag">Decision Tree</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">Validation & Sanitizers</div>
    <div class="feature-card-desc">
      AddressSanitizer, ThreadSanitizer, UndefinedBehaviorSanitizer, and testing best practices.
    </div>
    <div class="feature-tags">
      <a href="./guides/validation" class="feature-tag">Sanitizers</a>
      <a href="./guides/best-practices" class="feature-tag">Best Practices</a>
    </div>
  </div>
</div>

<div class="quick-start">
  <div class="quick-start-title">Quick Start</div>
  <div class="quick-start-content">
    <div class="command-block">
      <code>cmake --preset=debug && cmake --build build/debug</code>
    </div>
    After building, run <code>ctest --preset=debug</code> to verify. See the <a href="./getting-started/quickstart">Quick Start guide</a> for details.
  </div>
</div>
