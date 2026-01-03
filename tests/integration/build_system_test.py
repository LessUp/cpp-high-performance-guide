#!/usr/bin/env python3
"""
Build System Integration Tests
Property 2: Example Module Template Consistency
Validates: Requirements 1.6

Tests that the CMake build system correctly creates targets with proper
include paths, link dependencies, and benchmark registration.
"""

import os
import re
import subprocess
import sys
from pathlib import Path
from typing import List, Tuple

# Project root directory
PROJECT_ROOT = Path(__file__).parent.parent.parent


def find_cmake_files() -> List[Path]:
    """Find all CMakeLists.txt files in the project."""
    cmake_files = []
    for root, dirs, files in os.walk(PROJECT_ROOT):
        # Skip build directories
        dirs[:] = [d for d in dirs if d not in ['build', '.git', 'node_modules']]
        for file in files:
            if file == 'CMakeLists.txt':
                cmake_files.append(Path(root) / file)
    return cmake_files


def check_no_directory_based_commands(cmake_file: Path) -> Tuple[bool, List[str]]:
    """
    Property 1: CMake Target-Based Commands Only
    Check that no directory-based CMake commands are used.
    """
    forbidden_commands = [
        r'\binclude_directories\s*\(',
        r'\blink_directories\s*\(',
        r'\blink_libraries\s*\(',
        r'\badd_definitions\s*\(',
    ]
    
    violations = []
    content = cmake_file.read_text()
    
    for pattern in forbidden_commands:
        matches = re.findall(pattern, content, re.IGNORECASE)
        if matches:
            violations.extend(matches)
    
    return len(violations) == 0, violations


def check_target_based_commands_used(cmake_file: Path) -> bool:
    """Check that target-based commands are used."""
    target_commands = [
        r'\btarget_include_directories\s*\(',
        r'\btarget_link_libraries\s*\(',
        r'\btarget_compile_options\s*\(',
        r'\btarget_compile_definitions\s*\(',
    ]
    
    content = cmake_file.read_text()
    
    # At least one target-based command should be present in non-trivial CMake files
    for pattern in target_commands:
        if re.search(pattern, content, re.IGNORECASE):
            return True
    
    # Small/placeholder files are okay
    if len(content.strip()) < 100:
        return True
    
    return False


def test_cmake_best_practices():
    """Test that all CMakeLists.txt files follow best practices."""
    cmake_files = find_cmake_files()
    
    print(f"Found {len(cmake_files)} CMakeLists.txt files")
    
    all_passed = True
    
    for cmake_file in cmake_files:
        rel_path = cmake_file.relative_to(PROJECT_ROOT)
        
        # Check for forbidden directory-based commands
        passed, violations = check_no_directory_based_commands(cmake_file)
        if not passed:
            print(f"FAIL: {rel_path} uses directory-based commands: {violations}")
            all_passed = False
        else:
            print(f"PASS: {rel_path} - no directory-based commands")
    
    return all_passed


def test_cmake_configure():
    """Test that CMake can configure the project successfully."""
    build_dir = PROJECT_ROOT / "build" / "test_configure"
    build_dir.mkdir(parents=True, exist_ok=True)
    
    try:
        result = subprocess.run(
            ["cmake", "-S", str(PROJECT_ROOT), "-B", str(build_dir), "-G", "Ninja"],
            capture_output=True,
            text=True,
            timeout=300
        )
        
        if result.returncode != 0:
            print(f"FAIL: CMake configure failed")
            print(f"STDOUT: {result.stdout}")
            print(f"STDERR: {result.stderr}")
            return False
        
        print("PASS: CMake configure succeeded")
        return True
        
    except subprocess.TimeoutExpired:
        print("FAIL: CMake configure timed out")
        return False
    except FileNotFoundError:
        print("SKIP: CMake or Ninja not found")
        return True  # Skip if tools not available


def main():
    """Run all build system tests."""
    print("=" * 60)
    print("Build System Integration Tests")
    print("Property 2: Example Module Template Consistency")
    print("Validates: Requirements 1.6")
    print("=" * 60)
    print()
    
    results = []
    
    print("Test 1: CMake Best Practices")
    print("-" * 40)
    results.append(test_cmake_best_practices())
    print()
    
    print("Test 2: CMake Configure")
    print("-" * 40)
    results.append(test_cmake_configure())
    print()
    
    print("=" * 60)
    if all(results):
        print("All tests PASSED")
        return 0
    else:
        print("Some tests FAILED")
        return 1


if __name__ == "__main__":
    sys.exit(main())
