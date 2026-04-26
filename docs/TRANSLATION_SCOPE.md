# Translation Scope Policy

This document defines which documentation surfaces are maintained bilingually
(English + Simplified Chinese) and which are English-only by design.

## Bilingual surfaces

The following directories are fully bilingual. Every page in `docs/en/` has a
corresponding page in `docs/zh/`:

- `getting-started/`
- `guides/`
- `reference/` (top-level pages: `faq.md`, `troubleshooting.md`, `api-reference.md`)
- `contributing/`

## English-only surfaces

The following are maintained in English only. Chinese readers are directed to the
English version:

- `exercises/` — lab exercises and solutions are tightly coupled to code
  examples; translating them would increase maintenance cost without proportional
  reader benefit.
- `reference/api/` — auto-generated or low-level API detail pages.

## Reader guidance

A note is present in `docs/zh/index.md` directing Chinese readers to the English
exercises section when they reach that part of the learning path.

## Maintenance rule

When adding a new page to a bilingual surface, add it to both `docs/en/` and
`docs/zh/` in the same commit. When adding a page to an English-only surface,
no Chinese counterpart is required.
