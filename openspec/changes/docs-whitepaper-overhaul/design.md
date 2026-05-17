# Design: Docs Whitepaper Overhaul

## Overview

This change turns the docs site from a concise project landing page into a high-signal technical publication.
The implementation keeps the existing VitePress foundation and aligns it more closely with the Kimi CLI docs
baseline, but adapts the content model to this repository's educational and architecture-heavy domain.

## Design goals

1. Present the repository as a serious engineering artifact rather than a simple tutorial collection.
2. Keep the docs stack simple: VitePress, Mermaid, local search, bilingual routing, GitHub Pages deployment.
3. Make the landing surface narratively strong for expert readers: why the project exists, how it is structured,
   how claims are validated, and what broader literature or adjacent projects inform it.
4. Ensure all new visual surfaces remain readable in both dark and light themes.
5. Prefer reusable documentation primitives over one-off page styling.

---

## Baseline alignment with Kimi CLI

The target baseline from `/home/shane/dev/kimi-cli` is:

- VitePress with a custom theme entry under `docs/.vitepress/theme/`
- Mermaid and `vitepress-plugin-llms`
- root-level language redirect behavior
- GitHub Pages deployment using `actions/configure-pages` and `VITEPRESS_BASE`

The current repository already uses these pieces, so the redesign is an architectural normalization rather than a
stack migration. The change will preserve the current toolchain, align naming and routing patterns more closely with
Kimi CLI, and deepen the custom theme implementation instead of introducing a different framework.

---

## Information architecture

The current `getting-started / guides / exercises / reference / contributing` layout is correct but not sufficient
for a whitepaper-grade entry surface. The new docs narrative is:

| Section | Purpose |
|---|---|
| `academy/` | Reader onboarding, learning path, module atlas, validation doctrine |
| `architecture/` | System overview, repository topology, performance methodology, execution model |
| `playbook/` | Practical guides such as quick start, installation, profiling, decision tree, best practices |
| `reference/` | FAQ, troubleshooting, API reference, glossary/reference anchors |
| `research/` | Related projects, references, evolution notes, future directions |
| `contributing/` | AI workflow and contribution guidance |

Exercises remain available as a secondary learning surface but are removed from the primary narrative emphasis.

---

## Visual language

The visual system should feel closer to an engineering whitepaper than a marketing landing page:

- restrained cobalt brand accent
- neutral paper-like backgrounds with subtle grid / panel structure
- typographic hierarchy that privileges reading flow and figure captions
- structured data cards, metrics strips, section index cards, and diagram frames
- minimal animation; emphasis on contrast, rhythm, and compositional clarity

The existing CSS file will be refactored into clearer design tokens and reusable layout primitives. New Markdown pages
will rely on a small set of HTML/CSS building blocks instead of large one-off inline structures.

---

## Diagram and SVG strategy

The repository already uses Mermaid extensively. The redesign keeps Mermaid as the primary diagram system but upgrades
its framing:

1. Mermaid blocks are wrapped in consistent "figure shell" styles with captions and notes.
2. Theme-aware Mermaid variables are configured so diagram strokes, fills, and labels remain legible in both themes.
3. Standalone SVG assets use explicit light/dark-safe strokes and surfaces instead of assuming a white background.
4. Missing docs assets such as `logo.svg` are added to source control so metadata and Pages output are correct.

---

## Content strategy

New content is added where the current site is weakest:

- architecture overview page explaining repository layers, build presets, and validation flow
- methodology page describing benchmarking, profiling, sanitizer usage, and evidence standards
- related-work page situating the project against other C++ and systems-learning repositories
- references page with papers, articles, standards material, and tooling references
- evolution page explaining trade-offs, current repository posture, and why the project is archive-ready

The academy and architecture pages should directly connect code modules, docs modules, and validation commands so the
site reads like a technical guide rather than a static brochure.

---

## Bilingual scope

User-facing whitepaper surfaces remain bilingual:

- landing pages
- academy pages
- architecture pages
- playbook pages
- reference top-level pages
- research pages
- contributing pages

Exercises and deeply code-centric API detail pages remain English-first where the repository already treats them as
English-only by design. The translation scope document should make this explicit.

---

## Workflow and validation

The GitHub Pages workflow remains preset-free and docs-specific, but it is normalized toward the Kimi CLI pattern:

- configure Pages base path first
- pass `VITEPRESS_BASE` into the docs build
- ensure `.nojekyll` is present
- upload the built VitePress output as the Pages artifact

Docs validation remains `cd docs && npm ci && npm run build`.

---

## Trade-offs

- This redesign prioritizes architectural clarity and editorial depth over preserving the current sparse landing page.
- New docs pages increase maintenance surface slightly, but the payoff is a more coherent and archive-ready repo story.
- Mermaid remains the main diagram tool instead of introducing image-heavy custom illustration tooling, which keeps the
  site maintainable for low-frequency updates.
