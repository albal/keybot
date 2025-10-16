# Contributing to Testing

Thank you for contributing to the MacroPad project! This guide explains how to work with the test suite.

## Running Tests Locally

### Prerequisites

- ESP-IDF v5.1 or later installed
- ESP32 development board (for hardware tests)

### Build and Run Tests

1. **Navigate to the test directory**:
   ```bash
   cd test_app
   ```

2. **Build the test application**:
   ```bash
   idf.py build
   ```

3. **Flash and run tests** (requires hardware):
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

4. **View test results**:
   - Tests run automatically on boot
   - Use the interactive menu to run specific test categories
   - All test output appears on the serial monitor

## Test Structure

Tests are organized in `test_app/main/test_macropad.c` with the following categories:

- **[storage]** - NVS storage operations
- **[state]** - Application state management
- **[button]** - Touch button detection logic
- **[string]** - String manipulation utilities
- **[timeout]** - Timeout mechanisms
- **[integration]** - End-to-end workflows

## Writing New Tests

### Basic Test Structure

```c
TEST_CASE("Test description", "[tag]")
{
    // Setup
    int expected = 42;
    int actual = my_function();
    
    // Assert
    TEST_ASSERT_EQUAL(expected, actual);
    
    // Teardown (if needed)
}
```

### Common Assertions

- `TEST_ASSERT_EQUAL(expected, actual)` - Values are equal
- `TEST_ASSERT_TRUE(condition)` - Condition is true
- `TEST_ASSERT_FALSE(condition)` - Condition is false
- `TEST_ASSERT_EQUAL_STRING(expected, actual)` - Strings match
- `TEST_ASSERT_NOT_NULL(pointer)` - Pointer is not NULL

### Test Guidelines

1. **One concept per test**: Each test should validate one specific behavior
2. **Clear test names**: Use descriptive names that explain what is being tested
3. **Arrange-Act-Assert**: Structure tests with clear setup, action, and verification
4. **Clean up**: Reset state between tests if needed
5. **Tag appropriately**: Use tags to group related tests

### Example Test

```c
TEST_CASE("NVS: Save and load macro with special characters", "[storage]")
{
    // Arrange
    test_init_nvs();
    test_erase_nvs_namespace();
    const char *test_text = "Hello!@#$";
    char buffer[256];
    
    // Act
    esp_err_t save_result = test_save_macro(0, test_text);
    esp_err_t load_result = test_load_macro(0, buffer, sizeof(buffer));
    
    // Assert
    TEST_ASSERT_EQUAL(ESP_OK, save_result);
    TEST_ASSERT_EQUAL(ESP_OK, load_result);
    TEST_ASSERT_EQUAL_STRING(test_text, buffer);
}
```

## Continuous Integration

All tests run automatically on pull requests via GitHub Actions:

1. **Test Build**: Verifies tests compile successfully
2. **PR Comment**: Posts test coverage summary on PRs
3. **Build Status**: Shows pass/fail status

To verify your changes locally before pushing:

```bash
# Build tests
cd test_app && idf.py build

# Run test summary
cd .. && python3 test_summary.py
```

## Test Coverage Analysis

Use the test summary script to analyze coverage:

```bash
python3 test_summary.py
```

This will show:
- Total number of tests
- Tests per category
- Coverage of critical areas

## Adding Tests for New Features

When adding new features, ensure you add corresponding tests:

1. **Identify testable functions**: Find pure functions and logic to test
2. **Create test cases**: Add tests to `test_app/main/test_macropad.c`
3. **Use appropriate tags**: Tag tests with relevant categories
4. **Verify coverage**: Run `test_summary.py` to confirm coverage

## Hardware vs. Software Tests

- **Software tests**: Test logic, state management, string operations (can run in simulator)
- **Hardware tests**: Test display, Bluetooth, NVS (require actual ESP32)

Most tests in this suite are software tests that validate business logic.

## Troubleshooting

### Tests won't build

- Ensure ESP-IDF is properly installed and sourced
- Check that you're in the `test_app` directory
- Verify all required components are available

### Tests fail on hardware

- Check serial monitor for detailed error messages
- Ensure NVS is properly initialized
- Verify device has sufficient memory

### CI tests fail

- Check the GitHub Actions logs for specific errors
- Verify YAML syntax in workflow files
- Ensure all required files are committed

## Questions?

See the main [TESTING.md](../TESTING.md) for comprehensive testing documentation, or open an issue on GitHub.
