# Design System

## Overview

The docs site should read like a modern engineering monograph: structured, typographically confident, and visually
precise. The primary scene is an expert reader reviewing architecture and implementation details on a laptop or large
monitor in normal daylight, so the main experience should privilege a bright paper-like surface with a disciplined
dark companion theme for late-night review.

## Visual Direction

- Whitepaper / lab notebook aesthetic, not product-marketing SaaS
- Restrained color strategy with cool neutrals and one committed cobalt signal color
- Strong typographic hierarchy, generous rhythm, and figure-like diagram framing
- Minimal motion, mostly opacity and transform, never decorative layout animation

## Color

- Primary accent: deep cobalt in OKLCH, used for links, active states, figures, and emphasis
- Neutrals: slightly blue-tinted paper, ink, and steel tones, never pure black or white
- States: success, warning, and danger should stay muted and publication-like rather than dashboard-bright
- Dark theme should invert contrast without turning diagrams into neon-on-black

## Typography

- Use the system sans stack for maintainability
- Let hierarchy come from scale, weight, and spacing rather than decorative font mixing
- Keep body measure around 70ch
- Headings should feel compressed and authoritative, body copy should feel calm and readable

## Layout

- Prefer sectional composition and editorial rhythm over repeated equal cards
- Use panels, callout rows, figure frames, and structured index blocks as reusable primitives
- Keep the docs landing page asymmetric where it helps emphasis, but preserve clarity over spectacle
- Treat sidebars and top navigation as orientation tools, not decorative chrome

## Components

- Bilingual language switcher integrated into the nav
- Section index blocks for academy / architecture / research navigation
- Figure shell for Mermaid and SVG diagrams with caption and note support
- Reference list and comparison table styles for citations and related-work sections
- Hero rail with thesis, validation signals, and route entry points

## Motion

- Very light fades and lifts for interactive affordances
- No animated gradients
- No motion that blocks reading or competes with code and diagrams
