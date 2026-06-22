# Project Meta

This file consolidates the repository's domain glossary, product positioning, and design system into a single internal reference. It is not part of the VitePress route graph; maintain it alongside code and governance changes.

## Domain Glossary

This section defines the core domain terms of the C++ high-performance guide project, unifying Chinese and English expressions.

### Memory Optimization / 内存优化

#### cache-line alignment / 缓存行对齐
Data aligned to CPU cache-line boundaries (typically 64 bytes, 128 bytes on some ARM systems).
Goal: avoid the performance degradation caused by cross-cache-line accesses.

#### false sharing / 伪共享
When multiple threads access different variables on the same cache line, cache-coherence protocol traffic degrades performance.
Mitigation: cache-line padding isolates variables onto distinct lines.

#### AOS/SOA / 结构数组 vs 数组结构
- **AOS (Array of Structures)**: `struct { float x, y, z; } particles[N]`
- **SOA (Structure of Arrays)**: `struct { float x[N], y[N], z[N]; } particles`

SOA layout vectorizes better; AOS fits object-oriented thinking.

#### memory alignment / 内存对齐
Data addresses are integer multiples of their size. Aligned access is typically faster than unaligned access.

#### prefetch / 预取
Load data into cache before actual use to hide memory latency.

### Concurrency / 并发

#### lock-free / 无锁
Concurrent data structures that use atomic operations instead of mutexes.
Property: even if one thread is suspended, others can still make progress.

#### memory ordering / 内存序
Visibility ordering for atomic operations:
- `relaxed`: no synchronization, atomicity only
- `acquire/release`: establishes synchronization points
- `seq_cst`: sequential consistency (strongest guarantee)

#### SPSC / MPMC
- **SPSC (Single Producer Single Consumer)**: simpler, higher performance
- **MPMC (Multiple Producer Multiple Consumer)**: more general, more complex

#### atomic / 原子操作
Indivisible operations that either complete fully or not at all. Provided in C++ via `std::atomic`.

#### spinlock / 自旋锁
A thread busy-waits on lock failure instead of yielding the CPU. Suitable for short hold times.

### SIMD / 单指令多数据

#### SIMD (Single Instruction Multiple Data)
One instruction processes multiple data elements simultaneously.
x86: SSE (128-bit), AVX (256-bit), AVX-512 (512-bit)
ARM: NEON (128-bit)

#### vectorization / 向量化
Compiler-automatic or manual use of SIMD instructions to optimize loops.
- Auto-vectorization: compiler analyzes loops and emits SIMD code
- Manual vectorization: intrinsics or SIMD wrapper libraries

#### intrinsic
Compiler-provided low-level functions mapping directly to specific CPU instructions.
Example: `_mm_add_ps` maps to the SSE `addps` instruction.

#### SIMD width / 向量宽度
Data processed per SIMD operation:
- SSE: 4 floats (128-bit)
- AVX: 8 floats (256-bit)
- AVX-512: 16 floats (512-bit)

### Build System / 构建系统

#### preset-driven build / 预设驱动构建
Using `CMakePresets.json` to unify build configuration across local and CI environments.

#### cache-line allocator / 缓存行对齐分配器
Allocator aligned to cache-line boundaries (typically 64 bytes) to eliminate false sharing.
See `hpc::memory::AlignedAllocator<T, Alignment>`; alignment policy is a **compile-time constant**.

#### SIMD-width allocator / SIMD 宽度对齐分配器
Allocator aligned to SIMD vector-width boundaries (16/32/64 bytes) for SIMD load/store operations.
See `hpc::simd::AlignedAllocator<T>`; alignment policy is **runtime CPU-feature detected**.

**Key distinction**: cache-line allocator serves multi-threaded concurrency (false-sharing elimination); SIMD-width allocator serves single-threaded vectorization (aligned loads). The two exist independently and share no implementation.

#### sanitizer / 消毒器
Runtime detection tools for undefined behavior and memory errors:
- **ASAN (AddressSanitizer)**: out-of-bounds, use-after-free
- **TSAN (ThreadSanitizer)**: data races
- **UBSAN (UndefinedBehaviorSanitizer)**: undefined behavior
- **MSAN (MemorySanitizer)**: uninitialized memory reads

#### header-only library / 仅头文件库
A library usable by including headers only, no compile/link step.
Pros: simple integration; Cons: may increase compile time.

### Performance Measurement / 性能测量

#### benchmark / 基准测试
Standardized measurement of code execution time.
Key elements: warmup, multiple runs, median/percentile statistics.

#### flame graph / 火焰图
Visualization of program execution time distribution, showing call stacks and time share.

#### cache miss / 缓存未命中
CPU fails to find needed data in cache and must load from main memory.
A common cause of performance bottlenecks.

#### throughput / 吞吐量
Operations or data volume processed per unit time. Usually ops/sec, MB/s.

#### latency / 延迟
Time for a single operation from start to finish. Usually nanoseconds, microseconds.

## Product

### Register

brand

### Users

The primary readers are strict interviewers, advanced GitHub developers, and future maintainers evaluating the repository as a serious engineering artifact. They arrive in a browser, often from GitHub, and want fast evidence of architectural depth, technical rigor, validation discipline, and documentation quality.

### Product Purpose

This project teaches high-performance C++ through runnable examples, but the docs site must do more than teach. It must frame the repository as an archive-ready technical whitepaper, academy, and architecture guide that explains why the code exists, how the modules connect, how claims are validated, and where the project fits in the broader systems ecosystem.

### Brand Personality

Rigorous, scholarly, exacting. The tone should feel calm, credible, and opinionated, with enough visual character to avoid looking like a generic docs template.

### Anti-references

Do not look like a default GitBook clone, a card-farm SaaS landing page, a glassy AI-generated template, or a cyberpunk dashboard. Avoid ornamental gradients, gimmicky hero metrics, and any layout that signals "marketing site" before "engineering publication".

### Design Principles

1. Show evidence, not slogans.
2. Make reading flow as strong as visual style.
3. Treat diagrams as arguments, not decoration.
4. Keep bilingual entry surfaces structurally equivalent.
5. Favor durable, low-maintenance design primitives over one-off flair.

### Accessibility & Inclusion

Target WCAG AA contrast on both themes. Support reduced-motion users by keeping motion subtle and non-essential. Maintain strong readability for long-form technical content, including diagram captions, code-adjacent reference surfaces, and bilingual navigation.

## Design System

### Overview

The docs site should read like a modern engineering monograph: structured, typographically confident, and visually precise. The primary scene is an expert reader reviewing architecture and implementation details on a laptop or large monitor in normal daylight, so the main experience should privilege a bright paper-like surface with a disciplined dark companion theme for late-night review.

### Visual Direction

- Whitepaper / lab notebook aesthetic, not product-marketing SaaS
- Restrained color strategy with cool neutrals and one committed cobalt signal color
- Strong typographic hierarchy, generous rhythm, and figure-like diagram framing
- Minimal motion, mostly opacity and transform, never decorative layout animation

### Color

- Primary accent: deep cobalt in OKLCH, used for links, active states, figures, and emphasis
- Neutrals: slightly blue-tinted paper, ink, and steel tones, never pure black or white
- States: success, warning, and danger should stay muted and publication-like rather than dashboard-bright
- Dark theme should invert contrast without turning diagrams into neon-on-black

### Typography

- Use the system sans stack for maintainability
- Let hierarchy come from scale, weight, and spacing rather than decorative font mixing
- Keep body measure around 70ch
- Headings should feel compressed and authoritative, body copy should feel calm and readable

### Layout

- Prefer sectional composition and editorial rhythm over repeated equal cards
- Use panels, callout rows, figure frames, and structured index blocks as reusable primitives
- Keep the docs landing page asymmetric where it helps emphasis, but preserve clarity over spectacle
- Treat sidebars and top navigation as orientation tools, not decorative chrome

### Components

- Bilingual language switcher integrated into the nav
- Section index blocks for academy / architecture / research navigation
- Figure shell for Mermaid and SVG diagrams with caption and note support
- Reference list and comparison table styles for citations and related-work sections
- Hero rail with thesis, validation signals, and route entry points

### Motion

- Very light fades and lifts for interactive affordances
- No animated gradients
- No motion that blocks reading or competes with code and diagrams
