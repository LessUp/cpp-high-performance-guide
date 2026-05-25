# Contributing

Thanks for contributing. This repository prefers **spec-driven, low-noise changes** over broad, loosely scoped edits.

## First-time setup

```bash
git clone https://github.com/<your-username>/cpp-high-performance-guide.git
cd cpp-high-performance-guide

./scripts/setup.sh
cmake --preset=debug
cmake --build build/debug
ctest --preset=debug
```

`./scripts/setup.sh` checks the local toolchain and configures repository-managed Git hooks through `.githooks/`.

## Default workflow

1. Keep docs and code in sync.
2. Run the required validation commands.
3. Use `/review` before merge for non-trivial work.

## Validation commands

```bash
cmake --preset=debug && cmake --build build/debug && ctest --preset=debug
cmake --preset=release && cmake --build build/release && ctest --preset=release

cmake --preset=asan && cmake --build build/asan && ctest --preset=asan
cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan
cmake --preset=ubsan && cmake --build build/ubsan && ctest --preset=ubsan
```

The `tsan` preset is standardized on **Clang** because it is the most reliable ThreadSanitizer path for this repository.

## Formatting and hooks

```bash
./scripts/format.sh
./scripts/format.sh --check
./scripts/setup-hooks.sh
```

The repository hooks currently enforce:

- no generated build or docs artifacts in commits
- formatting checks for staged C++ files
- a debug build/test pass on push

## Documentation rules

- Keep root README files concise and entry-focused.
- Treat `docs/` as the richer narrative surface.
- Keep user-facing English and Chinese entry points aligned.
- Do not reintroduce GitBook / HonKit / `.kiro` references.

## Process rules

- Prefer deleting or archiving stale content over preserving it in active paths.
- Keep workflow additions justified and repository-specific.
- Avoid long-lived branch sprawl; one focused branch or PR per change is preferred.
- Do not merge non-trivial structural work without review.

## Need the AI workflow?

See `CLAUDE.md` for Claude Code specific guidance.
