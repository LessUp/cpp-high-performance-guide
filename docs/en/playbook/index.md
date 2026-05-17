# Playbook

The playbook is the operational layer of the site. It keeps the runnable guides together so that readers can move from first build to profiling and hardening without re-learning the repository layout.

## Operating principle

Use the playbook when you know what you need to do but not yet which file or page carries the command. It regroups existing guides without physically moving them, which keeps links stable while giving the site a clearer expert-facing narrative.

## Runbook matrix

| Need | Start here | Outcome |
| --- | --- | --- |
| confirm toolchain and platform assumptions | [Prerequisites](/en/getting-started/prerequisites) | a correct local environment before builds start |
| install dependencies and produce binaries | [Installation](/en/getting-started/installation) | a configured repository with fetched dependencies |
| reach the first verified run | [Quick Start](/en/getting-started/quickstart) | a baseline build and test pass |
| study modules in sequence | [Learning Path](/en/guides/learning-path) | a guided reading order across topics |
| diagnose a hot path | [Profiling Guide](/en/guides/profiling-guide) | profiler-first evidence for the next change |
| choose an optimization tactic | [Optimization Decision Tree](/en/guides/optimization-decision-tree) | a narrowed set of candidate interventions |
| harden low-level code before closure | [Validation & Sanitizers](/en/guides/validation) | memory, race, and UB checks |
| review project-wide habits | [Best Practices](/en/guides/best-practices) | durable rules that stay aligned with the repository posture |

## Recommended operational sequence

1. Run [Prerequisites](/en/getting-started/prerequisites) and [Installation](/en/getting-started/installation) once.
2. Use [Quick Start](/en/getting-started/quickstart) to prove the repository builds cleanly.
3. Choose a topic via the [Academy](/en/academy/) or [Learning Path](/en/guides/learning-path).
4. When optimizing, switch to [Profiling Guide](/en/guides/profiling-guide) and [Optimization Decision Tree](/en/guides/optimization-decision-tree).
5. Before declaring victory, run [Validation & Sanitizers](/en/guides/validation).

## Secondary support surfaces

- [Exercises](/en/exercises/) remain available for deliberate practice, especially when you want to reproduce a concept in isolation.
- [AI Workflow](/en/contributing/ai-workflow) remains a contributor-facing route rather than part of the core reader journey.
- [Reference](/en/reference/) is the right place to land when you already know the topic and need exact commands or API details.

## Why the playbook matters in a whitepaper

A whitepaper without runnable routes risks becoming ornamental. The playbook keeps that from happening. It is the operational proof that the architectural and research pages are attached to a live repository, not merely to a polished narrative.
