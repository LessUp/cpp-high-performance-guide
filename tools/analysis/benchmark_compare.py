#!/usr/bin/env python3
"""
benchmark_compare.py - Compare benchmark results and detect performance regressions

Usage:
    python benchmark_compare.py baseline.json current.json [--threshold 0.1]
    python benchmark_compare.py --report baseline.json current.json output.md

Features:
- Compare two benchmark JSON files
- Detect performance regressions
- Generate markdown reports
- Calculate speedup/slowdown percentages
"""

import json
import argparse
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum


class ChangeType(Enum):
    IMPROVEMENT = "improvement"
    REGRESSION = "regression"
    UNCHANGED = "unchanged"
    NEW = "new"
    REMOVED = "removed"


@dataclass
class BenchmarkComparison:
    name: str
    baseline_time: Optional[float]
    current_time: Optional[float]
    change_percent: float
    change_type: ChangeType
    speedup: float


def load_benchmark_json(filepath: str) -> Dict:
    """Load benchmark results from JSON file."""
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    # Handle both Google Benchmark format and our custom format
    if 'benchmarks' in data:
        return {b['name']: b for b in data['benchmarks']}
    return data


def get_time(benchmark: Dict) -> float:
    """Extract time from benchmark result (prefer cpu_time, fallback to real_time)."""
    if 'cpu_time' in benchmark:
        return benchmark['cpu_time']
    if 'real_time' in benchmark:
        return benchmark['real_time']
    raise ValueError(f"No time field found in benchmark: {benchmark}")


def compare_benchmarks(
    baseline: Dict,
    current: Dict,
    threshold: float = 0.1
) -> List[BenchmarkComparison]:
    """Compare two sets of benchmark results."""
    comparisons = []
    
    all_names = set(baseline.keys()) | set(current.keys())
    
    for name in sorted(all_names):
        baseline_bench = baseline.get(name)
        current_bench = current.get(name)
        
        if baseline_bench is None:
            # New benchmark
            comparisons.append(BenchmarkComparison(
                name=name,
                baseline_time=None,
                current_time=get_time(current_bench),
                change_percent=0,
                change_type=ChangeType.NEW,
                speedup=1.0
            ))
        elif current_bench is None:
            # Removed benchmark
            comparisons.append(BenchmarkComparison(
                name=name,
                baseline_time=get_time(baseline_bench),
                current_time=None,
                change_percent=0,
                change_type=ChangeType.REMOVED,
                speedup=1.0
            ))
        else:
            baseline_time = get_time(baseline_bench)
            current_time = get_time(current_bench)
            
            if baseline_time > 0:
                change_percent = (current_time - baseline_time) / baseline_time
                speedup = baseline_time / current_time if current_time > 0 else 0
            else:
                change_percent = 0
                speedup = 1.0
            
            if change_percent > threshold:
                change_type = ChangeType.REGRESSION
            elif change_percent < -threshold:
                change_type = ChangeType.IMPROVEMENT
            else:
                change_type = ChangeType.UNCHANGED
            
            comparisons.append(BenchmarkComparison(
                name=name,
                baseline_time=baseline_time,
                current_time=current_time,
                change_percent=change_percent,
                change_type=change_type,
                speedup=speedup
            ))
    
    return comparisons


def check_regression(
    comparisons: List[BenchmarkComparison],
    threshold: float = 0.1
) -> Tuple[bool, List[BenchmarkComparison]]:
    """Check if there are any performance regressions."""
    regressions = [c for c in comparisons if c.change_type == ChangeType.REGRESSION]
    return len(regressions) > 0, regressions


def format_time(ns: float) -> str:
    """Format time in nanoseconds to human-readable string."""
    if ns < 1000:
        return f"{ns:.2f} ns"
    elif ns < 1000000:
        return f"{ns/1000:.2f} µs"
    elif ns < 1000000000:
        return f"{ns/1000000:.2f} ms"
    else:
        return f"{ns/1000000000:.2f} s"


def generate_markdown_report(
    comparisons: List[BenchmarkComparison],
    baseline_file: str,
    current_file: str
) -> str:
    """Generate a markdown report of benchmark comparisons."""
    lines = [
        "# Benchmark Comparison Report",
        "",
        f"- **Baseline**: `{baseline_file}`",
        f"- **Current**: `{current_file}`",
        "",
        "## Summary",
        "",
    ]
    
    improvements = [c for c in comparisons if c.change_type == ChangeType.IMPROVEMENT]
    regressions = [c for c in comparisons if c.change_type == ChangeType.REGRESSION]
    unchanged = [c for c in comparisons if c.change_type == ChangeType.UNCHANGED]
    new_benchmarks = [c for c in comparisons if c.change_type == ChangeType.NEW]
    removed = [c for c in comparisons if c.change_type == ChangeType.REMOVED]
    
    lines.extend([
        f"- ✅ Improvements: {len(improvements)}",
        f"- ❌ Regressions: {len(regressions)}",
        f"- ➖ Unchanged: {len(unchanged)}",
        f"- 🆕 New: {len(new_benchmarks)}",
        f"- 🗑️ Removed: {len(removed)}",
        "",
    ])
    
    if regressions:
        lines.extend([
            "## ❌ Regressions",
            "",
            "| Benchmark | Baseline | Current | Change | Speedup |",
            "|-----------|----------|---------|--------|---------|",
        ])
        for c in regressions:
            baseline_str = format_time(c.baseline_time) if c.baseline_time else "N/A"
            current_str = format_time(c.current_time) if c.current_time else "N/A"
            lines.append(
                f"| {c.name} | {baseline_str} | {current_str} | "
                f"+{c.change_percent*100:.1f}% | {c.speedup:.2f}x |"
            )
        lines.append("")
    
    if improvements:
        lines.extend([
            "## ✅ Improvements",
            "",
            "| Benchmark | Baseline | Current | Change | Speedup |",
            "|-----------|----------|---------|--------|---------|",
        ])
        for c in improvements:
            baseline_str = format_time(c.baseline_time) if c.baseline_time else "N/A"
            current_str = format_time(c.current_time) if c.current_time else "N/A"
            lines.append(
                f"| {c.name} | {baseline_str} | {current_str} | "
                f"{c.change_percent*100:.1f}% | {c.speedup:.2f}x |"
            )
        lines.append("")
    
    lines.extend([
        "## All Results",
        "",
        "| Benchmark | Baseline | Current | Change | Status |",
        "|-----------|----------|---------|--------|--------|",
    ])
    
    status_emoji = {
        ChangeType.IMPROVEMENT: "✅",
        ChangeType.REGRESSION: "❌",
        ChangeType.UNCHANGED: "➖",
        ChangeType.NEW: "🆕",
        ChangeType.REMOVED: "🗑️",
    }
    
    for c in comparisons:
        baseline_str = format_time(c.baseline_time) if c.baseline_time else "N/A"
        current_str = format_time(c.current_time) if c.current_time else "N/A"
        
        if c.change_type in (ChangeType.NEW, ChangeType.REMOVED):
            change_str = "N/A"
        else:
            sign = "+" if c.change_percent > 0 else ""
            change_str = f"{sign}{c.change_percent*100:.1f}%"
        
        lines.append(
            f"| {c.name} | {baseline_str} | {current_str} | "
            f"{change_str} | {status_emoji[c.change_type]} |"
        )
    
    return "\n".join(lines)


def print_summary(comparisons: List[BenchmarkComparison]):
    """Print a summary of benchmark comparisons to stdout."""
    print("\n=== Benchmark Comparison Summary ===\n")
    
    for c in comparisons:
        if c.change_type == ChangeType.NEW:
            print(f"  🆕 {c.name}: NEW ({format_time(c.current_time)})")
        elif c.change_type == ChangeType.REMOVED:
            print(f"  🗑️  {c.name}: REMOVED")
        else:
            sign = "+" if c.change_percent > 0 else ""
            emoji = "❌" if c.change_type == ChangeType.REGRESSION else \
                    "✅" if c.change_type == ChangeType.IMPROVEMENT else "➖"
            print(f"  {emoji} {c.name}: {sign}{c.change_percent*100:.1f}% "
                  f"({format_time(c.baseline_time)} → {format_time(c.current_time)})")
    
    print()


def main():
    parser = argparse.ArgumentParser(
        description="Compare benchmark results and detect performance regressions"
    )
    parser.add_argument("baseline", help="Baseline benchmark JSON file")
    parser.add_argument("current", help="Current benchmark JSON file")
    parser.add_argument(
        "--threshold", "-t",
        type=float,
        default=0.1,
        help="Regression threshold (default: 0.1 = 10%%)"
    )
    parser.add_argument(
        "--report", "-r",
        help="Generate markdown report to file"
    )
    parser.add_argument(
        "--fail-on-regression",
        action="store_true",
        help="Exit with error code if regressions detected"
    )
    parser.add_argument(
        "--quiet", "-q",
        action="store_true",
        help="Suppress output (only exit code)"
    )
    
    args = parser.parse_args()
    
    # Load benchmark files
    try:
        baseline = load_benchmark_json(args.baseline)
        current = load_benchmark_json(args.current)
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"Error parsing JSON: {e}", file=sys.stderr)
        sys.exit(1)
    
    # Compare benchmarks
    comparisons = compare_benchmarks(baseline, current, args.threshold)
    
    # Print summary
    if not args.quiet:
        print_summary(comparisons)
    
    # Generate report if requested
    if args.report:
        report = generate_markdown_report(comparisons, args.baseline, args.current)
        with open(args.report, 'w') as f:
            f.write(report)
        if not args.quiet:
            print(f"Report saved to: {args.report}")
    
    # Check for regressions
    has_regression, regressions = check_regression(comparisons, args.threshold)
    
    if has_regression:
        if not args.quiet:
            print(f"\n⚠️  {len(regressions)} regression(s) detected!")
        if args.fail_on_regression:
            sys.exit(1)
    else:
        if not args.quiet:
            print("\n✅ No regressions detected.")
    
    sys.exit(0)


if __name__ == "__main__":
    main()
