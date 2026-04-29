#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path


TIME_UNIT_TO_NS = {
    "ns": 1.0,
    "us": 1_000.0,
    "ms": 1_000_000.0,
    "s": 1_000_000_000.0,
}


@dataclass(frozen=True)
class BenchmarkSample:
    name: str
    time_ns: float


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare two Google Benchmark JSON outputs and fail on regressions."
    )
    parser.add_argument("baseline", type=Path, help="Path to the baseline benchmark JSON file")
    parser.add_argument("candidate", type=Path, help="Path to the candidate benchmark JSON file")
    parser.add_argument(
        "--threshold",
        type=float,
        default=10.0,
        help="Maximum allowed regression percentage before exiting with code 1 (default: 10)",
    )
    return parser.parse_args()


def load_benchmark_samples(path: Path) -> dict[str, BenchmarkSample]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    benchmarks = payload.get("benchmarks")
    if not isinstance(benchmarks, list):
        raise ValueError(f"{path} does not contain a 'benchmarks' list")

    samples: dict[str, BenchmarkSample] = {}
    for entry in benchmarks:
        if not isinstance(entry, dict):
            continue

        name = entry.get("name")
        if not isinstance(name, str) or not name:
            continue

        if entry.get("aggregate_name") is not None:
            continue
        if entry.get("run_type") not in (None, "iteration"):
            continue

        raw_time = entry.get("cpu_time", entry.get("real_time"))
        if not isinstance(raw_time, (int, float)):
            continue

        time_unit = entry.get("time_unit", "ns")
        factor = TIME_UNIT_TO_NS.get(time_unit)
        if factor is None:
            raise ValueError(f"{path} uses unsupported time unit '{time_unit}' for benchmark '{name}'")

        samples[name] = BenchmarkSample(name=name, time_ns=float(raw_time) * factor)

    if not samples:
        raise ValueError(f"{path} does not contain any comparable benchmark samples")

    return samples


def format_row(name: str, baseline_ns: float, candidate_ns: float, delta_percent: float) -> str:
    return (
        f"{name:<40} "
        f"{baseline_ns:>14.2f} "
        f"{candidate_ns:>14.2f} "
        f"{delta_percent:>+9.2f}%"
    )


def compare_samples(
    baseline: dict[str, BenchmarkSample],
    candidate: dict[str, BenchmarkSample],
    threshold: float,
) -> int:
    baseline_names = set(baseline)
    candidate_names = set(candidate)
    if baseline_names != candidate_names:
        missing = sorted(baseline_names - candidate_names)
        extra = sorted(candidate_names - baseline_names)
        details = []
        if missing:
            details.append(f"missing in candidate: {', '.join(missing)}")
        if extra:
            details.append(f"only in candidate: {', '.join(extra)}")
        raise ValueError("Benchmark sets do not match (" + "; ".join(details) + ")")

    print(f"{'Benchmark':<40} {'Baseline(ns)':>14} {'Candidate(ns)':>14} {'Delta%':>10}")
    print("-" * 82)

    regressions = 0
    for name in sorted(baseline_names):
        baseline_ns = baseline[name].time_ns
        candidate_ns = candidate[name].time_ns
        if baseline_ns <= 0.0:
            raise ValueError(f"Baseline benchmark '{name}' has non-positive time {baseline_ns}")

        delta_percent = ((candidate_ns - baseline_ns) / baseline_ns) * 100.0
        print(format_row(name, baseline_ns, candidate_ns, delta_percent))
        if delta_percent > threshold:
            regressions += 1

    print("-" * 82)
    print(f"Threshold: +{threshold:.2f}%")
    if regressions:
        print(f"Regression detected: {regressions} benchmark(s) exceeded the threshold.")
        return 1

    print("No regressions detected.")
    return 0


def main() -> int:
    args = parse_args()
    baseline = load_benchmark_samples(args.baseline)
    candidate = load_benchmark_samples(args.candidate)
    return compare_samples(baseline, candidate, args.threshold)


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(2)
