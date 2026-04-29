# Design: Final Repository Hardening

## Overview

This change completes the normalization sequence. `project-closure-normalization` established the structural contract. `final-repo-hardening` closes the remaining gaps and brings the repository to a state where it can be maintained infrequently without accumulating drift.

## Design goals

1. Remove every surface that has no content and no expected future content.
2. Make the bilingual docs surface consistent by policy, not just by effort.
3. Confirm every GitHub Actions workflow either provides clear value or is removed.
4. Confirm AI guidance files are internally consistent and non-redundant.
5. Archive completed OpenSpec changes so active paths stay clean.
6. Leave the repository in a state where a returning maintainer can orient quickly.

---

## Surface audit

The table below classifies each surface under review. Quadrants: **Keep**, **Remove**, **Archive**, **Decide**.

| Surface | Classification | Rationale |
|---|---|---|
| `_bmad/` | **Remove** | Empty directory. Artifact of bmad AI-planning tooling used during initial scaffolding. No content, no future use. |
| `_bmad-output/` | **Remove** | Empty directory. Paired artifact with `_bmad/`. Same rationale: no content, no maintenance value. |
| `changelog/` | **Remove** | Empty directory. Historical releases are tracked through Git tags and GitHub Releases. An empty `changelog/` is misleading noise. |
| `docs/en/exercises/` | **Keep** | Active bilingual surface with exercises content. English-side is complete. Chinese translation is absent — requires explicit decision (see bilingual policy below). |
| `docs/zh/` (gap pages) | **Decide → translate or scope-limit** | `docs/zh/` is missing: exercises section (5 files), `reference/api/` subdirectory (3 files). Must be translated or explicitly excluded from the bilingual scope. |
| `docs/en/contributing/ai-workflow.md` + `docs/zh/contributing/ai-workflow.md` | **Keep with alignment check** | User-facing docs explaining the AI workflow. Must stay aligned with `AGENTS.md`, `CLAUDE.md`, and `.github/copilot-instructions.md`. No duplication of canonical guidance. |
| `.vscode/` | **N/A — not present** | No `.vscode/` directory exists in the repository. No action needed. Editor config is intentionally out of scope. |
| `openspec/explorations/` | **Keep (empty, intentional)** | The `.gitkeep` is deliberate scaffolding for future exploratory notes. The directory is low-cost and consistent with the OpenSpec layout. |
| `ci.yml` | **Keep** | Preset-driven CI for debug/release builds on Linux and macOS. Core maintenance value. |
| `pages.yml` | **Keep** | VitePress build and GitHub Pages deployment. Core maintenance value. |
| `sanitizers.yml` | **Keep with verification** | Sanitizer workflow (ASan, TSan, UBSan). High value; must be verified to pass cleanly on the current codebase before this change closes. |
| `benchmark.yml` | **Keep with verification** | Benchmark execution workflow. Must be verified to pass cleanly. If it silently fails or runs no benchmarks, it should be tightened or removed. |
| `openspec/changes/project-closure-normalization/` | **Archive** | All tasks are complete. Move to `openspec/changes/archive/project-closure-normalization/`. |

---

## Bilingual docs policy decision

**Decision:** The bilingual surface (`docs/en/` + `docs/zh/`) covers the following sections for both languages:
- `getting-started/`
- `guides/`
- `reference/` (top-level pages: `api-reference.md`, `faq.md`, `troubleshooting.md`)
- `contributing/`

The following sections are **English-only by design** and do not require Chinese translations:
- `docs/en/exercises/` — Hands-on exercises are primarily consumed in English in this educational context. Adding a note in `docs/zh/index.md` or the navigation pointing to the English exercises is acceptable.
- `docs/en/reference/api/` subdirectory files (`benchmark-utils.md`, `memory-utils.md`, `simd-wrapper.md`) — API reference pages for C++ utilities. Code-heavy content where Chinese translation provides diminishing returns.

This policy must be documented in `docs/.vitepress/config.ts` navigation structure or a `docs/TRANSLATION_SCOPE.md` so future contributors know what is and is not expected to be translated.

---

## AI guidance alignment strategy

Three canonical AI instruction files exist:
- `AGENTS.md` — primary repo guidance for all AI agents
- `CLAUDE.md` — Claude Code-specific workflow guidance
- `.github/copilot-instructions.md` — GitHub Copilot repository instructions

`docs/en/contributing/ai-workflow.md` and `docs/zh/contributing/ai-workflow.md` are **user-facing docs** that explain the AI workflow to human contributors. They should summarize and link to the canonical files rather than duplicating their content. If any of these docs have drifted from the current canonical guidance, they are updated as part of Phase 3.

---

## GitHub Actions rationalization

Each workflow is expected to satisfy the following contract:

| Workflow | Trigger | Pass criteria | Action if failing |
|---|---|---|---|
| `ci.yml` | push/PR to master | All preset builds pass on Linux (GCC, Clang) and macOS | Blocking — fix before merge |
| `pages.yml` | push to master (docs/** changes) | VitePress build succeeds, `index.html` exists | Blocking — fix before merge |
| `sanitizers.yml` | scheduled or manual | ASan/TSan/UBSan presets pass | Verify passes; fix if broken, remove if structurally broken |
| `benchmark.yml` | scheduled or manual | Benchmarks run to completion without error | Verify passes; tighten or remove if silently empty |

---

## OpenSpec archive sequence

1. When Phase 4 tasks are complete, move `openspec/changes/project-closure-normalization/` to `openspec/changes/archive/project-closure-normalization/`.
2. Move `openspec/changes/final-repo-hardening/` to `openspec/changes/archive/final-repo-hardening/`.
3. Confirm `openspec/changes/` contains only the `archive/` subdirectory.

---

## Trade-offs

- The exercises and API reference subdirectory pages are designated English-only rather than translated. This is a scope decision, not a quality compromise. The highest-value bilingual surfaces (getting-started, guides, reference top-level, contributing) remain fully bilingual.
- `openspec/explorations/` is kept as an intentional empty scaffold. Removing it would be equally valid, but the `.gitkeep` pattern is already established in the repo and costs nothing to maintain.
- `benchmark.yml` is verified before removal is considered. If it works, it stays. This avoids a premature removal of a potentially useful signal.
