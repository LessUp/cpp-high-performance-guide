# Proposal: Final Repository Hardening

## Summary

Complete the normalization effort started by `project-closure-normalization` by resolving the remaining surface drift, removing stale scaffolding artifacts, tightening CI workflows, and confirming the repository is ready for low-frequency maintenance.

## Relationship to project-closure-normalization

`project-closure-normalization` established the structural contract: OpenSpec layout, VitePress docs, preset-driven CI, AI instruction files, and GitHub Pages positioning. All tasks in that change are complete.

This change (`final-repo-hardening`) is the explicit follow-on. It starts from the state that `project-closure-normalization` delivered and addresses the surfaces that still have unresolved drift, are empty/stale, or need verification before the repository enters a quiet maintenance posture.

Work in this change does not reopen or revisit decisions made in `project-closure-normalization`. It extends them.

## Why

After `project-closure-normalization` closed, a targeted audit identified several surfaces that remain inconsistent with the repository's final intended state:

1. **Stale scaffolding directories** — `_bmad/` and `_bmad-output/` are empty artifacts from AI-assisted planning tooling that was used during early development. They have no content or maintenance value and should be removed.
2. **Empty changelog** — `changelog/` contains no entries. Its presence without content is misleading; the directory should be removed or explicitly replaced by the GitHub Releases/tags surface.
3. **Bilingual docs gap** — `docs/en/` has 20 documentation files; `docs/zh/` has 12. The exercises section and the `reference/api/` subdirectory pages are English-only. This asymmetry needs an explicit decision: either translate or formally mark those surfaces as English-only.
4. **Workflow review needed** — `benchmark.yml` and `sanitizers.yml` need verification that they provide clear maintenance value and do not silently fail on the current repository state.
5. **AI guidance drift** — `docs/en/contributing/ai-workflow.md` and `docs/zh/contributing/ai-workflow.md` may have drifted from the canonical guidance in `AGENTS.md`, `CLAUDE.md`, and `.github/copilot-instructions.md`.
6. **openspec/explorations/** — Contains only a `.gitkeep`. The directory should remain if it provides scaffolding value, but its existence should be explicitly confirmed as intentional.
7. **project-closure-normalization archive** — The completed predecessor change has not been moved to `openspec/changes/archive/`, which leaves an active path cluttered with finished work.

## Scope

### In scope

- Remove empty stale directories (`_bmad/`, `_bmad-output/`, `changelog/`)
- Resolve the bilingual docs asymmetry (translate or explicitly limit scope)
- Verify and tighten `benchmark.yml` and `sanitizers.yml`
- Confirm GitHub metadata (description, topics, homepage URL) matches current positioning
- Align `docs/contributing/ai-workflow.md` with canonical AI instruction files
- Archive `openspec/changes/project-closure-normalization/`
- Archive this change itself when all phases are complete

### Out of scope

- New teaching modules or learning content
- Changes to examples, tests, or benchmarks that are not directly caused by surface cleanup
- New CI platforms or toolchains
- Any expansion of MCP, plugin, or automation tooling

## Success criteria

- No empty or orphaned scaffolding directories remain in the active repository tree.
- The bilingual docs surface is consistent: every user-facing page exists in both languages, or a documented policy explicitly limits scope.
- All four GitHub Actions workflows pass cleanly and provide concrete maintenance value.
- GitHub metadata (description, topics, homepage) accurately reflects the repository's purpose.
- AI guidance documents are internally consistent and not duplicating content unnecessarily.
- `openspec/changes/project-closure-normalization/` is archived.
- The repository can be handed off for low-frequency maintenance with no known surface drift.
