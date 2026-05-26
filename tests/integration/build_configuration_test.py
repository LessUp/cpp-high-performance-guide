#!/usr/bin/env python3
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[2]


class BuildConfigurationTest(unittest.TestCase):
    def run_configure(self, *cmake_args: str) -> subprocess.CompletedProcess[str]:
        with tempfile.TemporaryDirectory() as tmpdir:
            build_dir = Path(tmpdir) / "build"
            return subprocess.run(
                [
                    "cmake",
                    "-S",
                    str(PROJECT_ROOT),
                    "-B",
                    str(build_dir),
                    "-G",
                    "Ninja",
                    *cmake_args,
                ],
                capture_output=True,
                text=True,
                check=False,
            )

    def run_configure_and_build(
        self,
        target: str,
        *cmake_args: str,
    ) -> subprocess.CompletedProcess[str]:
        with tempfile.TemporaryDirectory() as tmpdir:
            build_dir = Path(tmpdir) / "build"
            configure = subprocess.run(
                [
                    "cmake",
                    "-S",
                    str(PROJECT_ROOT),
                    "-B",
                    str(build_dir),
                    "-G",
                    "Ninja",
                    *cmake_args,
                ],
                capture_output=True,
                text=True,
                check=False,
            )
            if configure.returncode != 0:
                return configure

            return subprocess.run(
                [
                    "cmake",
                    "--build",
                    str(build_dir),
                    "--target",
                    target,
                    "-j2",
                ],
                capture_output=True,
                text=True,
                check=False,
            )

    def test_configure_succeeds_with_benchmarks_enabled_and_tests_disabled(self):
        result = self.run_configure(
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DHPC_BUILD_TESTS=OFF",
            "-DHPC_BUILD_BENCHMARKS=ON",
        )

        self.assertEqual(result.returncode, 0, msg=result.stdout + "\n" + result.stderr)

    def test_auto_vectorize_build_does_not_ignore_omp_simd_pragmas(self):
        result = self.run_configure_and_build(
            "auto_vectorize",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DHPC_BUILD_TESTS=OFF",
            "-DHPC_BUILD_BENCHMARKS=OFF",
        )

        self.assertEqual(result.returncode, 0, msg=result.stdout + "\n" + result.stderr)
        combined_output = result.stdout + "\n" + result.stderr
        self.assertNotIn("ignoring '#pragma omp simd'", combined_output, msg=combined_output)

    def test_core_test_build_avoids_core_header_warnings(self):
        result = self.run_configure_and_build(
            "core_test",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DHPC_BUILD_TESTS=ON",
            "-DHPC_BUILD_BENCHMARKS=OFF",
        )

        self.assertEqual(result.returncode, 0, msg=result.stdout + "\n" + result.stderr)
        combined_output = result.stdout + "\n" + result.stderr
        self.assertNotIn("-Winterference-size", combined_output, msg=combined_output)
        self.assertNotIn("shadows a previous local", combined_output, msg=combined_output)


if __name__ == "__main__":
    unittest.main()
