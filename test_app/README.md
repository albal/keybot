# MacroPad Unit Tests

This directory contains automated unit tests for the ESP32 MacroPad firmware.

## Overview

The test application validates core functionality including:
- NVS storage operations (save/load macros)
- State management (mode transitions, selections)
- Button touch detection logic
- String manipulation utilities
- Timeout mechanisms
- Integration workflows

## Building and Running Tests

### Prerequisites

- ESP-IDF v5.1 or later installed and configured
- ESP32 development board with USB connection

### Build Tests

```bash
cd test_app
idf.py build
```

### Flash and Run Tests

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

Replace `/dev/ttyUSB0` with your actual serial port.

### Test Output

Tests run automatically on boot. You'll see output like:

```
Starting MacroPad Unit Tests
========================================
Unity test run 1 of 1
Running NVS: Save and load single macro...
NVS: Save and load single macro PASSED

...

-----------------------
28 Tests 0 Failures 0 Ignored
OK
```

## Test Organization

Tests are organized with tags for selective execution:

- `[storage]` - NVS storage tests
- `[state]` - State management tests  
- `[button]` - Button logic tests
- `[string]` - String manipulation tests
- `[timeout]` - Timeout mechanism tests
- `[integration]` - Integration tests

## Interactive Menu

The Unity test framework provides an interactive menu:

- Press `[*]` to run all tests
- Press a tag name (e.g., `[storage]`) to run tests with that tag
- Press specific test numbers to run individual tests

## Continuous Integration

Tests automatically build on every pull request via GitHub Actions. See `.github/workflows/test.yml` for the CI configuration.

## Adding New Tests

To add new tests, edit `main/test_macropad.c` and add new `TEST_CASE` definitions:

```c
TEST_CASE("Test name", "[tag]")
{
    // Test implementation
    TEST_ASSERT_EQUAL(expected, actual);
}
```

## Documentation

For more details, see the main [TESTING.md](../TESTING.md) documentation.
