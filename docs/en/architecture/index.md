# Architecture

The architecture section explains how the repository is assembled, how the docs map onto that structure, and how validation methods connect implementation work to performance claims.

## Architectural thesis

This repository is organized around a simple promise: **each important performance lesson should exist simultaneously as code, as evidence, and as explanation**.

That promise shapes the architecture:

- example modules are the primary teaching artifact
- reusable headers stay small and explicit
- presets provide the common entry point for builds and tests
- benchmarks and profilers provide the measurement layer
- docs and OpenSpec preserve interpretation and intent

## Architecture routes

| Page | Primary question | Key surfaces it ties together |
| --- | --- | --- |
| [Repository Topology](/en/architecture/repository-topology) | Where does each concern live? | top-level directories, build system, docs, and governance files |
| [Performance Methodology](/en/architecture/performance-methodology) | How are performance claims established? | presets, benchmarks, profiling tools, and evidence rules |
| [Playbook](/en/playbook/) | Which commands should I run next? | getting-started guides, profiling, validation, and best practices |
| [Module Atlas](/en/academy/module-atlas) | Which module teaches which subsystem? | examples, tests, helper headers, and docs routes |

## Preset-driven validation loop

The repository deliberately teaches validation through public CMake presets instead of ad hoc scripts.

```bash
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```

This loop is architectural, not merely procedural. It gives the project a stable vocabulary for local work, CI, documentation, and later maintenance.

## Who should read what first

| If you are... | Start with | Then move to |
| --- | --- | --- |
| checking whether the repo is coherent | [Repository Topology](/en/architecture/repository-topology) | [Playbook](/en/playbook/) |
| checking whether performance claims are rigorous | [Performance Methodology](/en/architecture/performance-methodology) | [Research References](/en/research/references) |
| trying to understand one module quickly | [Module Atlas](/en/academy/module-atlas) | the corresponding example directory |
| maintaining docs and repository policy | [Research Evolution](/en/research/evolution) | `openspec/` and contributor guidance on GitHub |

## What this section does not do

The architecture pages do not try to replace source reading. Their job is narrower and more durable:

- make repository boundaries legible
- explain why validation starts with presets
- define the measurement discipline behind the benchmarks
- reduce the amount of context a future maintainer must reconstruct from scratch
