#!/usr/bin/env python3
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = PROJECT_ROOT / "tools" / "performance" / "benchmark_compare.py"


class CompareBenchmarksScriptTest(unittest.TestCase):
    def run_script(self, baseline_payload, candidate_payload, threshold=0.1, fail_on_regression=False):
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_path = Path(tmpdir)
            baseline_path = tmp_path / "baseline.json"
            candidate_path = tmp_path / "candidate.json"
            baseline_path.write_text(json.dumps(baseline_payload), encoding="utf-8")
            candidate_path.write_text(json.dumps(candidate_payload), encoding="utf-8")

            args = [
                sys.executable,
                str(SCRIPT_PATH),
                str(baseline_path),
                str(candidate_path),
                "--threshold",
                str(threshold),
            ]
            if fail_on_regression:
                args.append("--fail-on-regression")

            return subprocess.run(
                args,
                capture_output=True,
                text=True,
                check=False,
            )

    def test_exit_zero_when_all_benchmarks_stay_within_threshold(self):
        baseline = {
            "benchmarks": [
                {"name": "BM_AddArrays/1024", "cpu_time": 100.0, "time_unit": "ns"},
            ]
        }
        candidate = {
            "benchmarks": [
                {"name": "BM_AddArrays/1024", "cpu_time": 108.0, "time_unit": "ns"},
            ]
        }

        result = self.run_script(baseline, candidate, threshold=0.10)

        self.assertEqual(result.returncode, 0, msg=result.stderr or result.stdout)
        self.assertIn("BM_AddArrays/1024", result.stdout)
        self.assertIn("+8.0%", result.stdout)

    def test_exit_one_when_any_benchmark_regresses_beyond_threshold(self):
        baseline = {
            "benchmarks": [
                {"name": "BM_AddArrays/1024", "cpu_time": 100.0, "time_unit": "ns"},
            ]
        }
        candidate = {
            "benchmarks": [
                {"name": "BM_AddArrays/1024", "cpu_time": 120.0, "time_unit": "ns"},
            ]
        }

        result = self.run_script(
            baseline,
            candidate,
            threshold=0.10,
            fail_on_regression=True,
        )

        self.assertEqual(result.returncode, 1, msg=result.stderr or result.stdout)
        self.assertIn("regression(s) detected", result.stdout)
        self.assertIn("+20.0%", result.stdout)


if __name__ == "__main__":
    unittest.main()
