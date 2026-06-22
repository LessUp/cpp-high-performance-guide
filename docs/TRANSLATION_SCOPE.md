# Translation Scope Policy

This document defines which documentation surfaces are maintained bilingually
(English + Simplified Chinese) and which remain English-only by design.

## Bilingual whitepaper surfaces

The user-facing whitepaper routes below are explicitly bilingual and should stay
structurally aligned between `docs/en/` and `docs/zh/` in the same commit:

- landing pages: `index.md`
- `academy/`
- `architecture/`
- `playbook/`
- playbook-routed learning pages: `getting-started/`, `guides/`
- `reference/` top-level pages plus `reference/api/` detail pages
- `research/`
- `contributing/`
- `deep-dives/`
- `exercises/`
- `algorithms/`

These surfaces carry the main reader journey and therefore require bilingual
parity in information architecture, navigational links, and argument depth.
`getting-started/` and `guides/` remain part of that bilingual scope even
though they are surfaced through the playbook narrative rather than living
under `playbook/` on disk.

## Maintenance rule

When adding or significantly reshaping a page in a bilingual surface, update the
English and Chinese counterparts together in the same commit.
