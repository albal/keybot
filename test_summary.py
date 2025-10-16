#!/usr/bin/env python3
"""
Test Summary Script

This script analyzes the test suite and provides a summary of test coverage.
Run this to verify that all critical functionality is tested.
"""

import re
import sys

def analyze_tests(test_file):
    """Analyze the test file and generate a summary."""
    
    with open(test_file, 'r') as f:
        content = f.read()
    
    # Find all TEST_CASE definitions
    test_cases = re.findall(r'TEST_CASE\("([^"]+)",\s*"\[([^\]]+)\]"\)', content)
    
    if not test_cases:
        print("❌ No test cases found!")
        return False
    
    # Organize by tags
    tests_by_tag = {}
    for test_name, tag in test_cases:
        if tag not in tests_by_tag:
            tests_by_tag[tag] = []
        tests_by_tag[tag].append(test_name)
    
    # Print summary
    print("=" * 70)
    print("MacroPad Test Suite Summary")
    print("=" * 70)
    print()
    
    total_tests = len(test_cases)
    print(f"Total Tests: {total_tests}")
    print(f"Test Categories: {len(tests_by_tag)}")
    print()
    
    for tag in sorted(tests_by_tag.keys()):
        tests = tests_by_tag[tag]
        print(f"[{tag}] - {len(tests)} tests")
        for test in tests:
            print(f"  • {test}")
        print()
    
    # Check for critical coverage
    critical_tags = ['storage', 'state', 'button', 'integration']
    missing_critical = [tag for tag in critical_tags if tag not in tests_by_tag]
    
    if missing_critical:
        print(f"⚠️  Warning: Missing tests for critical areas: {', '.join(missing_critical)}")
        return False
    
    print("✅ All critical areas have test coverage")
    return True

if __name__ == "__main__":
    test_file = "test_app/main/test_macropad.c"
    
    try:
        success = analyze_tests(test_file)
        sys.exit(0 if success else 1)
    except FileNotFoundError:
        print(f"❌ Test file not found: {test_file}")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error: {e}")
        sys.exit(1)
