# Proposal: Docs Whitepaper Overhaul

## Summary

Rebuild the GitHub Pages documentation site into a bilingual whitepaper-grade project surface that matches the
underlying VitePress architecture and deployment model used by `/home/shane/dev/kimi-cli`, while replacing the
current lightweight landing layout with a deeper academy, architecture, and research narrative.

## Motivation

The current docs site already borrows parts of the Kimi CLI docs stack, but it still behaves like a thin project
landing page:

1. The information architecture is shallow and does not present the repository as a technical whitepaper.
2. The home page is visually cleaner than the original docs, but it does not convey architecture depth, validation
   rigor, or research context strongly enough for expert readers and interviewers.
3. The docs config references `logo.svg`, but no corresponding asset exists in the docs source tree.
4. Dark/light visual behavior is incomplete for diagrams and future SVG-heavy surfaces.
5. The repository lacks explicit pages for architecture, methodology, related work, and evolution thinking.

## In Scope

- Align the docs engine, route handling, and GitHub Pages workflow with the Kimi CLI docs baseline where it improves
  maintainability.
- Redesign the visual system to a whitepaper / engineering-lab aesthetic with robust dark/light behavior.
- Reorganize navigation into a clearer academy / architecture / playbook / reference / research structure.
- Add new bilingual content pages covering system architecture, methodology, related work, references, and project
  evolution.
- Standardize diagram presentation and make Mermaid/SVG output legible in both themes.
- Repair missing or stale assets and metadata used by the docs site.

## Out of Scope

- Rewriting the C++ examples themselves.
- Adding new runtime libraries beyond the existing VitePress, Mermaid, and llms tooling already used by the repo.
- Translating English-only exercises or API subpages that are intentionally out of the bilingual scope.

## Success Criteria

- `docs/` builds successfully with VitePress after the redesign.
- The docs site exposes a clear expert-facing narrative from landing page to architecture and methodology pages.
- The new design keeps both English and Chinese entry paths intact.
- Mermaid diagrams and SVG-based visual surfaces remain legible in dark and light themes.
- Contributor-facing references continue to reflect the VitePress + GitHub Pages stack.
