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
- `reference/` top-level pages: `index.md`, `faq.md`, `troubleshooting.md`, `api-reference.md`
- `research/`
- `contributing/`

These surfaces carry the main reader journey and therefore require bilingual
parity in information architecture, navigational links, and argument depth.

## English-only technical-detail surfaces

The following areas remain English-only on purpose:

- `exercises/` - lab exercises and solutions stay close to code and are kept
  English-first to reduce maintenance overhead.
- `reference/api/` - low-level API detail pages such as memory utilities, SIMD
  wrappers, and benchmark helpers prioritize exact technical wording over full
  translation coverage.

Chinese pages should continue linking readers to these English-only areas where
useful, but they do not require Chinese counterparts.

## Reader guidance

Chinese landing and hub pages should keep directing readers to the English-only
exercise and deep API surfaces when those routes are the correct next step.

## Maintenance rule

When adding or significantly reshaping a page in a bilingual surface, update the
English and Chinese counterparts together in the same commit. When adding a page
inside an English-only surface, no Chinese counterpart is required unless the
scope policy changes first.
