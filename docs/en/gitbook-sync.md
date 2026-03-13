# GitBook Sync Guide

This repository is prepared for GitBook online reading using Git Sync.

## Recommended Setup

1. Create or open a GitBook space.
2. In the space header, choose `Configure`.
3. Select `GitHub Sync`.
4. Authenticate your GitHub account.
5. Install the GitBook GitHub App for the account or organization that owns this repository.
6. Select the repository `LessUp/cpp-high-performance-guide`.
7. Select the branch you want to sync, usually `master` in this repository.
8. For the initial import, choose `GitHub -> GitBook` if the repository already contains the documentation.

## Repository Configuration

This repository already includes the following files for GitBook:

- `.gitbook.yaml`
- `SUMMARY.md`
- `DOCS.md`
- Markdown documentation under `docs/`

## Content Structure

GitBook will use:

- `DOCS.md` as the first page
- `SUMMARY.md` as the table of contents

The `.gitbook.yaml` file explicitly defines this structure.

## Notes

- Do not edit the docs entry page from the GitBook UI when Git Sync is enabled. Keep `DOCS.md` managed from the repository.
- If you do not manually customize the table of contents in GitBook, `SUMMARY.md` will be used directly.
- If you later move your docs under another root folder, update `.gitbook.yaml` accordingly.
