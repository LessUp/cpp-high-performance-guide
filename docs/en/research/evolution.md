# Evolution

This repository now favors consistency, hardening, and archive-ready maintenance over endless topic expansion. That posture is the result of deliberate narrowing, not of neglect.

## Evolution in phases

| Phase | What changed | Durable outcome |
| --- | --- | --- |
| Initial module expansion | the repository established topic-oriented example modules for build systems, memory, modern C++, SIMD, and concurrency | the executable example remains the primary teaching unit |
| Validation normalization | CMake presets, tests, benchmarks, and sanitizers became the common vocabulary for proof | readers can verify claims with a stable command surface |
| Documentation re-architecture | the docs site moved toward academy, architecture, playbook, reference, and research routes | the repository now reads like a whitepaper rather than a loose guide collection |
| Closure and hardening | maintainers prioritized drift reduction, explicit governance, and low-frequency upkeep | the project became easier to preserve, review, and update carefully |

## Why the current posture is stricter

Three pressures shape the current maintenance model:

1. **Performance advice decays quickly when it is not re-validated.** The repository therefore prefers fewer, better-explained surfaces.
2. **Docs drift is expensive.** The site now favors route clarity and explicit cross-links over a large number of shallow pages.
3. **Archive-ready maintenance requires low indirection.** Future maintainers should be able to understand the project from the repository itself, without hidden workflows.

## What is intentionally preserved

- runnable example modules under `examples/`
- preset-driven validation as the public operational contract
- benchmark and profiling workflows that keep performance claims inspectable
- bilingual reader-facing landing surfaces where the site intends to serve both audiences
- OpenSpec and repository guidance that explain structural decisions before future cleanup is needed

## What is intentionally resisted

- uncontrolled growth in topic count without matching validation depth
- extra automation layers that add maintenance cost without clearer evidence
- documentation copy that sounds broader or more certain than the code can support
- duplicate surfaces that compete to be the source of truth

## Reading the current repository correctly

The most important interpretive shift is this: the repository is no longer trying to be a constantly expanding encyclopedia of performance tricks. It is trying to remain a compact, trustworthy set of examples and notes that a strict reviewer can still audit years later.

That is why the current docs emphasize topology, methodology, references, and evolution. Those pages turn the repository from a collection of examples into a durable engineering record.
