# Tasks: Final Repository Hardening

## Dependency order

Phases must execute in sequence. Each phase has an explicit completion gate that must be satisfied before the next phase begins.

---

## Phase 1 — Structure, Docs, Pages, Bug Baseline, Version Anchors

**Goal:** Remove stale scaffolding artifacts, resolve the bilingual docs gap, verify VitePress and GitHub Pages, and establish a clean structural baseline.

**Completion gate:** All stale directories are removed. Bilingual docs scope is explicitly documented. VitePress builds without error. `docs/zh/` contains no unresolved gaps relative to the declared bilingual scope policy. No regressions in the existing docs site layout.

### 1.1 Remove `_bmad/`

Delete the empty `_bmad/` directory. It is an artifact of AI planning tooling used during initial scaffolding with no content or future maintenance value. Verify no CI workflow or script references it.

### 1.2 Remove `_bmad-output/`

Delete the empty `_bmad-output/` directory. Same rationale as `_bmad/`. Verify no CI workflow or script references it.

### 1.3 Remove `changelog/`

Delete the empty `changelog/` directory. Historical release information is tracked through Git tags and GitHub Releases. Verify no CI workflow, docs page, or script references this path as a content source.

### 1.4 Document bilingual scope policy

Create `docs/TRANSLATION_SCOPE.md` (or add a clearly marked section to `docs/en/contributing/ai-workflow.md`) stating:
- `getting-started/`, `guides/`, top-level `reference/` pages, and `contributing/` are fully bilingual.
- `exercises/` and `reference/api/` subdirectory pages are English-only by design.
- Add a note in `docs/zh/index.md` or in the VitePress `zh` navigation pointing readers to the English exercises section.

### 1.5 Verify VitePress build baseline

Run `cd docs && npm ci && npm run docs:build` in the worktree. Confirm the build completes without error and `docs/.vitepress/dist/index.html` exists. Record any build errors or deprecation warnings that require follow-up.

### 1.6 Verify Node.js / package version alignment

Check that the Node.js version referenced in `pages.yml` (`node-version: 22`) matches the version available in the local environment. Confirm `docs/package.json` VitePress version is pinned to a stable release with no pending major-version migration required.

---

## Phase 2 — CI, Workflows, GitHub Metadata, Dev Flow

**Goal:** Confirm all four GitHub Actions workflows pass cleanly and provide real maintenance value. Verify GitHub repository metadata matches current project positioning.

**Completion gate:** All four workflows either pass cleanly or are explicitly tightened/removed with a documented rationale. GitHub description, topics, and homepage URL are verified or updated to match the current repository state.

### 2.1 Verify `sanitizers.yml` passes

Run the sanitizer presets locally:
```
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```
If any preset fails, fix the underlying issue. If the workflow definition itself is broken (e.g., wrong runner dependencies), fix the workflow.

### 2.2 Verify `benchmark.yml` passes

Check `benchmark.yml` content. Run the benchmark preset locally:
```
cmake --preset=release && cmake --build build/release
./build/release/benchmarks/<benchmark-binary>
```
Confirm benchmarks execute to completion and produce output. If the workflow runs no benchmarks or silently exits with success, tighten the step to assert meaningful output or remove the workflow with a documented rationale.

### 2.3 Verify GitHub repository metadata

Using `gh repo view`, check the current repository description, topics, and homepage URL. Update if they do not match:
- **Description:** A concise summary of the repository's purpose as a C++ high-performance guide with examples, benchmarks, and learning modules.
- **Topics:** `cpp`, `performance`, `cmake`, `vitepress`, appropriate technical tags.
- **Homepage:** The GitHub Pages URL for the repository.

```
gh repo edit --description "..." --add-topic ... --homepage "..."
```

### 2.4 Confirm CI workflow matrix is current

Review `ci.yml` matrix (GCC 12, Clang, macOS). Verify the compiler versions and runner images are not end-of-life or deprecated. Update if needed to keep the matrix current without adding new platforms.

---

## Phase 3 — AI Guidance, Tooling, LSP, Editor Strategy

**Goal:** Confirm that AI instruction files are internally consistent, non-redundant, and aligned with the repository's current workflow. Confirm `docs/contributing/ai-workflow.md` summarizes rather than duplicates canonical guidance.

**Completion gate:** `AGENTS.md`, `CLAUDE.md`, `.github/copilot-instructions.md`, and `docs/*/contributing/ai-workflow.md` are mutually consistent. No file duplicates canonical guidance verbatim. `clangd` and `compile_commands.json` are the documented LSP path. No `.vscode/` directory has been inadvertently introduced.

### 3.1 Audit AI instruction file alignment

Read `AGENTS.md`, `CLAUDE.md`, `.github/copilot-instructions.md`, and both `docs/en/contributing/ai-workflow.md` and `docs/zh/contributing/ai-workflow.md` side by side. Identify any:
- Contradictions between files
- Content in `docs/contributing/ai-workflow.md` that verbatim duplicates what is in the canonical instruction files (should be a summary + link instead)
- Stale references (HonKit, GitBook, `.kiro/`, old task-list formats)

Fix any contradictions or stale references found.

### 3.2 Verify clangd LSP guidance is current

Confirm that `AGENTS.md` and `CLAUDE.md` both reference `clangd` + `compile_commands.json` as the default LSP. Verify `CMakePresets.json` or `CMakeLists.txt` exports `compile_commands.json` (the `CMAKE_EXPORT_COMPILE_COMMANDS` flag). If any AI guidance references a different LSP path, align it to `clangd`.

### 3.3 Confirm no `.vscode/` directory has been introduced

Verify `ls .vscode/` returns nothing. The repository does not commit editor configuration. If a `.vscode/` directory is found, assess whether to add it to `.gitignore` or remove it.

### 3.4 Confirm MCP and plugin restraint guidance is present

Verify that at least one AI instruction file explicitly states the policy of avoiding MCP server or plugin expansion unless there is clear repository-specific value. This was established in `project-closure-normalization` and must not have been removed.

---

## Phase 4 — Final Stabilization, Handoff, Archive Readiness

**Goal:** Run the full validation suite, confirm no regressions, archive completed OpenSpec changes, and leave the repository ready for low-frequency maintenance.

**Completion gate:** All standard commands pass. `openspec/changes/project-closure-normalization/` is archived. This change is archived. `openspec/changes/` contains only `archive/`. The repository presents a clean, coherent story to a returning maintainer.

### 4.1 Run full validation suite

```
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release
./scripts/format.sh --check
```

All commands must exit with status 0. Fix any failures before proceeding.

### 4.2 Run a targeted sanitizer check

Run at least one sanitizer preset end-to-end to confirm no regressions from the surface cleanup:
```
cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
```

### 4.3 Run `/review` pass

Invoke `/review` on the branch diff. Address any high-signal findings before merge. Low-signal style comments may be noted and deferred. Document what was reviewed and any decisions made.

### 4.4 Archive `project-closure-normalization`

```
mv openspec/changes/project-closure-normalization openspec/changes/archive/project-closure-normalization
```

Commit with a clear message referencing this change.

### 4.5 Archive this change (`final-repo-hardening`)

```
mv openspec/changes/final-repo-hardening openspec/changes/archive/final-repo-hardening
```

Confirm `openspec/changes/` now contains only `archive/`. Commit as the final act of the hardening effort.

### 4.6 Confirm repository handoff state

Verify:
- `README.md` entry points are accurate and current.
- `AGENTS.md` reflects the final workflow with no stale instructions.
- No directories exist in the repository root that are empty or have no documented purpose.
- The GitHub Pages site builds and deploys correctly.
- Git tags or a GitHub Release captures the repository's stable state if appropriate.
