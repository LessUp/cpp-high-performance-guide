# Proposal: Project Closure Normalization

## Summary

Normalize the repository so its OpenSpec structure, documentation, GitHub workflows, AI collaboration files, and public GitHub surfaces all tell one coherent story and support an archive-ready maintenance posture.

## Why

The repository currently contains drift across multiple layers:

1. OpenSpec structure and capability coverage are incomplete or inconsistent.
2. Documentation still references superseded tooling and older project structures.
3. Workflow behavior is noisier and less aligned with CMake presets than it should be.
4. AI collaboration guidance is fragmented or missing.
5. GitHub Pages and repository metadata do not yet reflect the final intended presentation.

Without a dedicated cleanup change, future maintenance will keep paying the cost of this drift.

## Scope

### In scope

- OpenSpec normalization and missing capability coverage
- docs / README / AGENTS / CLAUDE / Copilot instruction consolidation
- workflow and hook hardening
- VitePress landing-page refinement
- GitHub description / topics / homepage cleanup
- repository-specific AI workflow guidance

### Out of scope

- new teaching modules
- large new feature areas
- broad tooling additions with unclear maintenance value

## Success criteria

- Active OpenSpec changes follow the expected structure.
- No contributor-facing docs reference legacy HonKit / GitBook / `.kiro` assumptions.
- GitHub workflows are preset-driven and fail clearly on real validation errors.
- AI tooling guidance is discoverable and project-specific.
- GitHub Pages and repository metadata present a cleaner final project story.
