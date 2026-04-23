#!/usr/bin/env bash
set -euo pipefail

formatter="${CLANG_FORMAT:-clang-format}"
mode="fix"
scope="all"

for arg in "$@"; do
    case "$arg" in
        --check)
            mode="check"
            ;;
        --staged)
            scope="staged"
            ;;
        *)
            echo "Unknown argument: $arg" >&2
            echo "Usage: $0 [--check] [--staged]" >&2
            exit 1
            ;;
    esac
done

if ! command -v "$formatter" >/dev/null 2>&1; then
    echo "clang-format is required but was not found." >&2
    exit 1
fi

collect_all_files() {
    find examples tests benchmarks \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) -print
}

collect_staged_files() {
    git diff --cached --name-only --diff-filter=ACMR | \
        grep -E '^(examples|tests|benchmarks)/.*\.(cpp|hpp|h)$' || true
}

mapfile -t files < <(
    if [[ "$scope" == "staged" ]]; then
        collect_staged_files
    else
        collect_all_files
    fi
)

if [[ "${#files[@]}" -eq 0 ]]; then
    echo "No C++ files selected for formatting."
    exit 0
fi

if [[ "$mode" == "check" ]]; then
    echo "Checking formatting for ${#files[@]} file(s)..."
    "$formatter" --dry-run --Werror "${files[@]}"
    echo "Formatting check passed."
    exit 0
fi

echo "Formatting ${#files[@]} file(s)..."
"$formatter" -i "${files[@]}"

if [[ "$scope" == "staged" ]]; then
    git add "${files[@]}"
fi

echo "Formatting completed."
