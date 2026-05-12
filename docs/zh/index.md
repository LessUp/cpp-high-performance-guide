---
layout: home
---

<div class="home-header">
  <div class="home-header-left">
    <div class="home-logo">C++</div>
    <div>
      <span class="home-title">C++ 高性能指南</span>
      <span class="home-subtitle">性能工程</span>
    </div>
  </div>
  <div class="home-nav">
    <a href="./getting-started/quickstart">快速开始</a>
    <a href="./guides/learning-path">学习路径</a>
    <a href="https://github.com/LessUp/cpp-high-performance-guide">GitHub</a>
    <a href="../en/">English</a>
  </div>
</div>

<div class="home-intro-row">
  <div class="home-intro">
    一份实用的 C++20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。每个主题都配有可运行的代码和面向基准测试的验证路径。
  </div>
  <div class="home-stats">
    <span><strong>C++20</strong> 标准</span>
    <span><strong>5</strong> 个模块</span>
    <span><strong>双语</strong> 文档</span>
  </div>
</div>

## 学习路径

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">内存布局与缓存</div>
    <div class="feature-card-desc">
      理解缓存行为、数据局部性和内存对齐，以实现最佳性能。
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">学习路径</a>
      <a href="/en/exercises/module-02-memory" class="feature-tag">练习 (英文)</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">SIMD 优化</div>
    <div class="feature-card-desc">
      利用 SIMD 指令实现向量化计算和数据并行。
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">学习路径</a>
      <a href="/en/exercises/module-04-simd" class="feature-tag">练习 (英文)</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">并发与线程</div>
    <div class="feature-card-desc">
      掌握 std::atomic、内存排序和无锁数据结构。
    </div>
    <div class="feature-tags">
      <a href="./guides/learning-path" class="feature-tag">学习路径</a>
      <a href="/en/exercises/module-05-concurrency" class="feature-tag">练习 (英文)</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">构建系统 (CMake)</div>
    <div class="feature-card-desc">
      现代 CMake 预设、Sanitizer 和跨平台构建配置。
    </div>
    <div class="feature-tags">
      <a href="./getting-started/quickstart" class="feature-tag">快速开始</a>
      <a href="./getting-started/prerequisites" class="feature-tag">先决条件</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">性能分析</div>
    <div class="feature-card-desc">
      性能分析工具、基准测试方法论和优化决策树。
    </div>
    <div class="feature-tags">
      <a href="./guides/profiling-guide" class="feature-tag">性能分析</a>
      <a href="./guides/optimization-decision-tree" class="feature-tag">决策树</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">验证与 Sanitizer</div>
    <div class="feature-card-desc">
      AddressSanitizer、ThreadSanitizer、UndefinedBehaviorSanitizer 和测试最佳实践。
    </div>
    <div class="feature-tags">
      <a href="./guides/validation" class="feature-tag">Sanitizer</a>
      <a href="./guides/best-practices" class="feature-tag">最佳实践</a>
    </div>
  </div>
</div>

<div class="quick-start">
  <div class="quick-start-title">快速开始</div>
  <div class="quick-start-content">
    <div class="command-block">
      <code>cmake --preset=debug && cmake --build build/debug</code>
    </div>
    构建完成后，运行 <code>ctest --preset=debug</code> 进行验证。详见<a href="./getting-started/quickstart">快速开始指南</a>。
  </div>
</div>
