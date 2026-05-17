# Reference

Use the reference section when you need exact interfaces, command destinations, or failure-path guidance more than narrative. It is the site's precision layer.

## Reference matrix

| If you need... | Start here | Scope |
| --- | --- | --- |
| reusable helper APIs and module-level headers | [API Reference](/en/reference/api-reference) | entry hub for documented English-only API detail pages |
| fast answers to recurring setup and workflow questions | [FAQ](/en/reference/faq) | high-frequency questions with short answers |
| build, runtime, or benchmark failure recovery | [Troubleshooting](/en/reference/troubleshooting) | operational diagnosis routes |
| source material behind the methodology | [Research References](/en/research/references) | standards, manuals, repositories, and articles |
| module-to-code mapping | [Module Atlas](/en/academy/module-atlas) | architectural orientation rather than API details |

## Command lookup

| Task | Canonical route |
| --- | --- |
| baseline validation | `cmake --preset=debug && cmake --build build/debug && ctest --preset=debug` |
| optimized validation | `cmake --preset=release && cmake --build build/release && ctest --preset=release` |
| memory-safety pass | `cmake --preset=asan && cmake --build build/asan && ctest --preset=asan` |
| race detection | `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan` |
| undefined-behavior pass | `cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan` |

## English-only detail pages

Low-level detail pages under `reference/api/` remain English-first by design. They document code surfaces such as memory utilities, SIMD wrappers, and benchmark helpers where precision matters more than translation coverage.

## Relationship to other sections

- use the [Playbook](/en/playbook/) when you need a guided workflow
- use [Architecture](/en/architecture/) when you need system context
- use [Research](/en/research/) when you need external citations or historical framing

The reference hub is intentionally compact. It is meant to stay stable even when other narrative surfaces deepen.
