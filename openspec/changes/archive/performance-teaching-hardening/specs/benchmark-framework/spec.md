# Benchmark Framework

## ADDED Requirements

### Requirement: Benchmark Regression Comparison

THE HPC_Guide SHALL provide a script to compare two Google Benchmark JSON output files and identify regressions.

#### Scenario: No regressions detected

- **WHEN** `scripts/compare_benchmarks.py` is run with two JSON files where all benchmarks are within the threshold
- **THEN** the script prints a comparison table and exits with code 0

#### Scenario: Regression detected

- **WHEN** `scripts/compare_benchmarks.py` is run with two JSON files where one or more benchmarks exceed the regression threshold
- **THEN** the script prints the offending benchmarks and exits with code 1

#### Scenario: Threshold configurable

- **WHEN** the script is invoked with `--threshold N`
- **THEN** the regression threshold is set to N percent rather than the default 10 percent

#### Scenario: Script smoke test passes

- **WHEN** `scripts/compare_benchmarks.py` is invoked with two synthesised JSON inputs (one stable, one regressed)
- **THEN** it exits 0 for the stable case and exits 1 for the regressed case, confirming the script is functional
