# Testing Strategy for ESP32 MacroPad

This document outlines the comprehensive testing strategy for the ESP32 MacroPad firmware, including automated unit tests, manual test cases, and debugging procedures.

## Table of Contents

1. [Automated Unit Tests](#automated-unit-tests)
2. [Test Environment Setup](#test-environment-setup)
3. [Manual Test Cases](#manual-test-cases)
4. [Unit Test Examples](#unit-test-examples)
5. [Integration Tests](#integration-tests)
6. [Edge Cases & Stress Tests](#edge-cases--stress-tests)
7. [Debugging Procedures](#debugging-procedures)

---

## Automated Unit Tests

### Overview

The project includes a comprehensive suite of automated unit tests that validate core functionality. These tests run on every pull request via GitHub Actions and can also be run manually on hardware.

### Test Coverage

The unit tests validate:

- **NVS Storage Operations**: Save/load macros, empty strings, long strings, special characters, overwrite operations
- **State Management**: Mode transitions, macro selection, reset operations
- **Button Logic**: Touch detection within button bounds, boundary conditions
- **String Manipulation**: Append/remove characters, buffer overflow protection
- **Timeout Mechanisms**: Selection timeout logic and boundary conditions
- **Integration Workflows**: Complete save/load and macro selection flows

### Running Tests on CI

Tests automatically run on every pull request. The GitHub Actions workflow:
1. Builds the test application
2. Reports build status on the PR
3. Validates that tests compile correctly

### Running Tests on Hardware

To run tests on actual ESP32 hardware:

1. **Build the test application**:
   ```bash
   cd test_app
   idf.py build
   ```

2. **Flash to device**:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```
   Replace `/dev/ttyUSB0` with your actual serial port.

3. **Interactive test menu**:
   - Tests run automatically on boot
   - Use the Unity test menu to run specific tests or test groups
   - Press `[*]` to run all tests
   - Press `[tag]` to run tests with a specific tag (e.g., `[storage]`, `[state]`)

4. **View results**:
   - Test results are displayed on the serial monitor
   - Each test shows PASS/FAIL status
   - Summary shows total tests run and failures

### Test Tags

Tests are organized by tags for selective execution:
- `[storage]` - NVS storage tests
- `[state]` - State management tests
- `[button]` - Button logic tests
- `[string]` - String manipulation tests
- `[timeout]` - Timeout mechanism tests
- `[integration]` - Integration workflow tests

### Example Test Output

```
Starting MacroPad Unit Tests
========================================
Running NVS: Save and load single macro...
NVS: Save and load single macro PASSED

Running NVS: Save and load all four macros...
NVS: Save and load all four macros PASSED

...

-----------------------
28 Tests 0 Failures 0 Ignored
OK
```

---

## Test Environment Setup

### Required Hardware

- ESP32-WROOM-32 development board
- ILI9341 320x240 TFT touchscreen display
- USB cable for power and serial monitoring
- Computer or smartphone with Bluetooth capability
- Text editor application on connected device

### Required Software

- Arduino IDE or PlatformIO
- Serial Monitor (115200 baud)
- Bluetooth pairing capability on test device

### Pre-Test Checklist

- [ ] Firmware uploaded successfully
- [ ] Display powers on and shows UI
- [ ] Serial monitor connected and showing debug output
- [ ] All connections verified and secure
- [ ] Bluetooth enabled on test device

---

## Manual Test Cases

### 1. Display & Touch Functionality

#### Test 1.1: Display Initialization
**Objective**: Verify display powers on and shows main screen

**Steps**:
1. Power on the ESP32
2. Observe display initialization

**Expected Results**:
- Display shows "ESP32 MacroPad" title
- 4 macro buttons visible (M1, M2, M3, M4)
- Settings button visible in bottom-right
- Bluetooth status shown in top-right
- All text is readable and properly aligned

**Pass Criteria**: All UI elements visible and properly rendered

---

#### Test 1.2: Touch Accuracy
**Objective**: Verify touch controller accurately detects touches

**Steps**:
1. Touch each of the 4 macro buttons in sequence
2. Touch the Settings button
3. Observe Serial Monitor for touch coordinates

**Expected Results**:
- Each touch is detected
- Correct button is identified
- Serial Monitor shows touch coordinates
- Visual feedback on button press (highlight)

**Pass Criteria**: 100% of touches correctly detected and identified

---

#### Test 1.3: Two-Step Safeguard
**Objective**: Verify macro selection and confirmation mechanism

**Steps**:
1. Touch Macro 1 button
2. Observe UI changes
3. Note position of SEND button
4. Touch SEND button
5. Repeat for Macro 2, 3, and 4

**Expected Results**:
- Selected macro button highlights in red
- SEND button appears on opposite side of screen
  - M1 (top-left) → SEND appears bottom-right
  - M2 (top-right) → SEND appears bottom-left
  - M3 (bottom-left) → SEND appears top-right
  - M4 (bottom-right) → SEND appears top-left
- SEND button is green color
- Pressing SEND executes the macro

**Pass Criteria**: SEND button always appears opposite to selected macro

---

#### Test 1.4: Selection Timeout
**Objective**: Verify 5-second timeout clears selection

**Steps**:
1. Touch Macro 1 button
2. Do not touch SEND button
3. Wait 5 seconds
4. Observe UI

**Expected Results**:
- Selection clears after 5 seconds
- SEND button disappears
- Screen returns to initial state
- No macro is executed

**Pass Criteria**: Selection automatically clears within 5-6 seconds

---

#### Test 1.5: Touch Cancel
**Objective**: Verify touching elsewhere cancels selection

**Steps**:
1. Touch Macro 1 button
2. Touch empty area of screen (not SEND button)
3. Observe UI

**Expected Results**:
- Selection immediately clears
- SEND button disappears
- Screen returns to initial state
- No macro is executed

**Pass Criteria**: Selection cancels on any touch outside SEND button

---

### 2. Bluetooth Connection Tests

#### Test 2.1: Device Discovery
**Objective**: Verify device is discoverable

**Steps**:
1. Open Bluetooth settings on test device
2. Scan for new devices
3. Look for "ESP32 MacroPad"

**Expected Results**:
- "ESP32 MacroPad" appears in device list
- Device is connectable

**Pass Criteria**: Device visible and named correctly

---

#### Test 2.2: Initial Pairing
**Objective**: Verify successful initial pairing

**Steps**:
1. Select "ESP32 MacroPad" from Bluetooth list
2. Complete pairing process
3. Observe display status

**Expected Results**:
- Pairing completes successfully
- Display shows "BT: Connected" (green text)
- Serial Monitor shows connection confirmation
- No pairing code required (should be automatic)

**Pass Criteria**: Successful pairing with status indication

---

#### Test 2.3: Connection Stability
**Objective**: Verify connection remains stable

**Steps**:
1. Pair device
2. Leave connected for 5 minutes
3. Check status periodically

**Expected Results**:
- Connection remains stable
- Status stays "BT: Connected"
- No unexpected disconnections

**Pass Criteria**: Connection stable for 5+ minutes

---

#### Test 2.4: Disconnection Handling
**Objective**: Verify graceful handling of disconnection

**Steps**:
1. Pair device
2. Turn off Bluetooth on test device
3. Observe display status

**Expected Results**:
- Display updates to "BT: Waiting..." (red text)
- No crash or freeze
- Device continues to function

**Pass Criteria**: Graceful handling of disconnection

---

#### Test 2.5: Reconnection
**Objective**: Verify automatic reconnection

**Steps**:
1. Start with paired but disconnected device
2. Turn on Bluetooth on test device
3. Wait for automatic reconnection

**Expected Results**:
- Device automatically reconnects
- Status updates to "BT: Connected"
- No manual intervention required

**Pass Criteria**: Automatic reconnection within 30 seconds

---

### 3. Macro Playback Tests

#### Test 3.1: Default Macro Execution
**Objective**: Verify default macros work on first boot

**Steps**:
1. Fresh firmware upload (first boot)
2. Pair with test device
3. Open text editor on test device
4. Select and send Macro 1

**Expected Results**:
- Display shows "Sending..." message
- Text "Macro 1" appears in text editor
- Display shows "Sent!" confirmation

**Pass Criteria**: Default macro text successfully transmitted

---

#### Test 3.2: Alphanumeric Characters
**Objective**: Verify correct transmission of letters and numbers

**Steps**:
1. Configure Macro 1: "Hello World 123"
2. Open text editor on test device
3. Select and send Macro 1

**Expected Results**:
- Exact text "Hello World 123" appears
- Correct capitalization
- Correct spacing

**Pass Criteria**: Text matches exactly with proper case and spacing

---

#### Test 3.3: Special Characters
**Objective**: Verify special characters transmit correctly

**Steps**:
1. Configure Macro 1: "!@#$%^&*()_+-=[]{}|;':\",./<>?"
2. Send Macro 1 to text editor

**Expected Results**:
- All special characters appear correctly
- No garbled or missing characters

**Pass Criteria**: All special characters match configuration

---

#### Test 3.4: Mixed Content
**Objective**: Verify complex mixed content

**Steps**:
1. Configure Macro 1: "User123! Email: test@example.com Password: P@ssw0rd!"
2. Send Macro 1

**Expected Results**:
- Entire string appears correctly
- Mix of upper/lowercase, numbers, and symbols
- Special characters in correct positions

**Pass Criteria**: Complex string transmitted accurately

---

#### Test 3.5: Multiple Macros
**Objective**: Verify all 4 macros work independently

**Steps**:
1. Configure all 4 macros with unique text
2. Send each macro in sequence
3. Verify each transmits correctly

**Expected Results**:
- Each macro sends its configured text
- No cross-contamination between macros
- All 4 macros work correctly

**Pass Criteria**: All 4 macros function independently

---

#### Test 3.6: No Connection Error
**Objective**: Verify error handling when not connected

**Steps**:
1. Ensure Bluetooth is NOT connected
2. Attempt to send a macro

**Expected Results**:
- Error message displayed: "BT Not Connected!"
- No crash or freeze
- Can try again after connecting

**Pass Criteria**: Graceful error message shown

---

### 4. Configuration Mode Tests

#### Test 4.1: Enter Configuration Mode
**Objective**: Verify Settings button opens config mode

**Steps**:
1. From main screen, touch Settings button
2. Observe screen change

**Expected Results**:
- Screen changes to "Configure Macros"
- Instruction text: "Tap a button to edit its macro"
- 4 macro buttons labeled "Edit M1" through "Edit M4"
- Back button visible
- Current macro text shown under each button

**Pass Criteria**: Config screen displays correctly

---

#### Test 4.2: Edit Macro Button
**Objective**: Verify macro selection for editing

**Steps**:
1. In config mode, touch "Edit M1"
2. Observe screen change

**Expected Results**:
- Screen changes to keyboard view
- Title shows "Edit Macro 1"
- Text input area visible
- Current macro text shown in input area
- Full keyboard displayed

**Pass Criteria**: Keyboard screen appears with current text

---

#### Test 4.3: On-Screen Keyboard - Letters
**Objective**: Verify letter keys work

**Steps**:
1. Enter edit mode for Macro 1
2. Clear existing text
3. Type "hello" using keyboard

**Expected Results**:
- Each keypress adds letter to input area
- Letters appear lowercase
- Text updates in real-time

**Pass Criteria**: All letter keys function correctly

---

#### Test 4.4: Shift Key - Uppercase
**Objective**: Verify Shift key for uppercase

**Steps**:
1. Enter edit mode
2. Press Shift key (observe color change)
3. Press "h" key
4. Observe result

**Expected Results**:
- Shift key highlights when pressed
- "H" (uppercase) appears in input area
- Shift automatically deactivates after one character

**Pass Criteria**: Shift produces uppercase letter for one character

---

#### Test 4.5: Shift Key - Symbols
**Objective**: Verify Shift key changes symbols

**Steps**:
1. Enter edit mode
2. Press Shift key
3. Press number keys (observe symbols)
4. Press special character keys

**Expected Results**:
- Number row shows symbols: !@#$%^&*()
- Special characters change: [] → {}, ; → :, etc.
- Shift deactivates after one character

**Pass Criteria**: Shift correctly toggles to symbol mode

---

#### Test 4.6: Space Key
**Objective**: Verify space key adds spaces

**Steps**:
1. Enter edit mode
2. Type "Hello World" using Space key

**Expected Results**:
- Space appears between words
- Proper spacing maintained

**Pass Criteria**: Space key functions correctly

---

#### Test 4.7: Backspace Key
**Objective**: Verify backspace deletes characters

**Steps**:
1. Enter edit mode
2. Type "Hello123"
3. Press Backspace 3 times

**Expected Results**:
- Each backspace removes one character
- Text updates to "Hello"
- Backspace doesn't delete beyond empty string

**Pass Criteria**: Backspace removes characters one at a time

---

#### Test 4.8: Save Macro
**Objective**: Verify Save button stores macro

**Steps**:
1. Edit Macro 1 to "Test Save"
2. Press Save button
3. Observe screen change

**Expected Results**:
- Screen returns to config mode
- Macro 1 now shows "Test Save" text
- Serial Monitor shows save confirmation

**Pass Criteria**: Macro saved and visible in config screen

---

#### Test 4.9: Back Button
**Objective**: Verify Back button returns to main screen

**Steps**:
1. In config mode, touch Back button

**Expected Results**:
- Screen returns to main playback mode
- All 4 macro buttons visible
- Updated macro text displayed

**Pass Criteria**: Successful return to main screen

---

### 5. Persistence Tests

#### Test 5.1: Save and Power Cycle
**Objective**: Verify macros persist after power cycle

**Steps**:
1. Configure all 4 macros with unique text
2. Note the text for each macro
3. Power off ESP32 completely
4. Wait 10 seconds
5. Power on ESP32
6. Check macro text

**Expected Results**:
- All 4 macros retain their configured text
- No data loss
- Macros immediately available

**Pass Criteria**: All macros survive power cycle

---

#### Test 5.2: Multiple Save Operations
**Objective**: Verify repeated saves work correctly

**Steps**:
1. Edit Macro 1 to "Version 1"
2. Save
3. Edit Macro 1 to "Version 2"
4. Save
5. Edit Macro 1 to "Version 3"
6. Save
7. Power cycle
8. Check Macro 1

**Expected Results**:
- Macro 1 contains "Version 3"
- Latest save is what persists
- No corruption from multiple saves

**Pass Criteria**: Latest version is retained

---

#### Test 5.3: Firmware Update
**Objective**: Verify macros survive firmware update

**Steps**:
1. Configure all 4 macros
2. Note the text
3. Re-upload firmware
4. Check macros

**Expected Results**:
- All macros still contain their text
- NVS partition not erased by upload

**Pass Criteria**: Macros survive firmware update

---

## Unit Test Examples

The following are example unit tests that could be implemented using a testing framework like Google Test or Arduino Unit Testing Library.

### Storage Tests

```cpp
TEST(PreferencesStorage, SaveAndLoadMacro) {
  // Test saving and loading a macro
  String testString = "Test Macro 123!";
  int macroIndex = 0;
  
  saveMacro(macroIndex, testString);
  String loaded = loadMacro(macroIndex);
  
  ASSERT_EQUAL(testString, loaded);
}

TEST(PreferencesStorage, SaveEmptyString) {
  // Test saving an empty macro
  String emptyString = "";
  int macroIndex = 1;
  
  saveMacro(macroIndex, emptyString);
  String loaded = loadMacro(macroIndex);
  
  ASSERT_EQUAL(emptyString, loaded);
}

TEST(PreferencesStorage, SaveLongString) {
  // Test saving a very long macro
  String longString = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.";
  int macroIndex = 2;
  
  saveMacro(macroIndex, longString);
  String loaded = loadMacro(macroIndex);
  
  ASSERT_EQUAL(longString, loaded);
}

TEST(PreferencesStorage, SaveAllMacros) {
  // Test saving and loading all 4 macros
  String macros[4] = {"Macro 1", "Macro 2", "Macro 3", "Macro 4"};
  
  for (int i = 0; i < 4; i++) {
    saveMacro(i, macros[i]);
  }
  
  for (int i = 0; i < 4; i++) {
    String loaded = loadMacro(i);
    ASSERT_EQUAL(macros[i], loaded);
  }
}
```

### Touch Detection Tests

```cpp
TEST(TouchInput, ButtonHitDetection) {
  // Test if touch coordinates properly detect button press
  uint16_t bx = 0, by = 0, bw = 100, bh = 100;
  
  // Touch inside button
  ASSERT_TRUE(isTouchInButton(50, 50, bx, by, bw, bh));
  ASSERT_TRUE(isTouchInButton(0, 0, bx, by, bw, bh));
  ASSERT_TRUE(isTouchInButton(100, 100, bx, by, bw, bh));
  
  // Touch outside button
  ASSERT_FALSE(isTouchInButton(150, 150, bx, by, bw, bh));
  ASSERT_FALSE(isTouchInButton(-1, 50, bx, by, bw, bh));
  ASSERT_FALSE(isTouchInButton(50, -1, bx, by, bw, bh));
}

TEST(TouchInput, MacroButtonDetection) {
  // Test detection of each macro button
  uint16_t startX = 10, startY = 35;
  uint16_t btnWidth = 140, btnHeight = 90, margin = 10;
  
  // Touch center of Macro 1 (top-left)
  uint16_t x = startX + btnWidth / 2;
  uint16_t y = startY + btnHeight / 2;
  // Should detect Macro 1
  
  // Touch center of Macro 2 (top-right)
  x = startX + btnWidth + margin + btnWidth / 2;
  y = startY + btnHeight / 2;
  // Should detect Macro 2
}
```

### State Management Tests

```cpp
TEST(MacroSelection, SelectionState) {
  // Test macro selection state management
  selectedMacro = -1;
  sendButtonVisible = false;
  
  // Simulate selecting macro 1
  selectedMacro = 0;
  selectionTime = millis();
  sendButtonVisible = true;
  
  ASSERT_EQUAL(0, selectedMacro);
  ASSERT_TRUE(sendButtonVisible);
}

TEST(MacroSelection, ResetSelection) {
  // Test selection reset
  selectedMacro = 2;
  sendButtonVisible = true;
  selectionTime = millis();
  
  resetSelection();
  
  ASSERT_EQUAL(-1, selectedMacro);
  ASSERT_FALSE(sendButtonVisible);
  ASSERT_EQUAL(0, selectionTime);
}

TEST(MacroSelection, TimeoutBehavior) {
  // Test timeout behavior (mock time)
  selectedMacro = 0;
  sendButtonVisible = true;
  selectionTime = millis() - 6000;  // 6 seconds ago
  
  // Check if timeout has occurred
  bool timedOut = (millis() - selectionTime > 5000);
  
  ASSERT_TRUE(timedOut);
}
```

### Display Tests

```cpp
TEST(Display, ColorDefinitions) {
  // Verify color definitions are valid 16-bit colors
  ASSERT_TRUE(COLOR_BG >= 0x0000 && COLOR_BG <= 0xFFFF);
  ASSERT_TRUE(COLOR_BUTTON >= 0x0000 && COLOR_BUTTON <= 0xFFFF);
  ASSERT_TRUE(COLOR_TEXT >= 0x0000 && COLOR_TEXT <= 0xFFFF);
}

TEST(Display, ScreenDimensions) {
  // Verify screen dimensions
  ASSERT_EQUAL(320, SCREEN_WIDTH);
  ASSERT_EQUAL(240, SCREEN_HEIGHT);
}
```

---

## Integration Tests

### Test INT-1: End-to-End Macro Flow
**Objective**: Complete workflow from config to execution

**Steps**:
1. Enter configuration mode
2. Edit Macro 1 to "Integration Test"
3. Save macro
4. Return to main screen
5. Pair Bluetooth device
6. Select and send Macro 1

**Expected Results**:
- Complete workflow executes without errors
- Text "Integration Test" appears on connected device

**Pass Criteria**: Entire flow works seamlessly

---

### Test INT-2: Rapid Mode Switching
**Objective**: Verify stability with rapid mode changes

**Steps**:
1. Main screen → Config → Edit → Save → Config → Main
2. Repeat sequence 5 times rapidly

**Expected Results**:
- All transitions smooth
- No crashes or freezes
- UI renders correctly each time

**Pass Criteria**: Stable operation through rapid mode changes

---

## Edge Cases & Stress Tests

### Test EDGE-1: Very Long Macro
**Objective**: Test maximum macro length

**Steps**:
1. Configure Macro 1 with 500+ character string
2. Save and send macro

**Expected Results**:
- String saves correctly
- String transmits completely
- No buffer overflow or crash

**Pass Criteria**: Long strings handled correctly

---

### Test EDGE-2: Empty Macro
**Objective**: Test empty macro string

**Steps**:
1. Configure Macro 1 to empty string
2. Save
3. Attempt to send

**Expected Results**:
- Empty string saves
- No error on send (just does nothing)
- No crash

**Pass Criteria**: Empty macros handled gracefully

---

### Test EDGE-3: Special Character Edge Cases
**Objective**: Test problematic characters

**Steps**:
1. Test backslash: "\\"
2. Test quotes: \"'
3. Test null-like characters
4. Test unicode (if supported)

**Expected Results**:
- All characters handled correctly
- No escaping issues
- No crashes

**Pass Criteria**: All special characters work

---

### Test EDGE-4: Rapid Button Presses
**Objective**: Test debouncing and rapid input

**Steps**:
1. Rapidly press macro buttons
2. Rapidly press keyboard keys
3. Observe behavior

**Expected Results**:
- Debouncing prevents double-presses
- UI remains responsive
- No queue overflow

**Pass Criteria**: Stable under rapid input

---

### Test STRESS-1: Extended Operation
**Objective**: Verify 24-hour stability

**Steps**:
1. Power on device
2. Leave running for 24 hours
3. Periodically test functionality

**Expected Results**:
- Device remains operational
- No memory leaks
- All functions still work

**Pass Criteria**: Stable operation for 24+ hours

---

### Test STRESS-2: Repeated Saves
**Objective**: Test NVS write endurance

**Steps**:
1. Edit and save same macro 100 times
2. Power cycle
3. Verify macro still loads

**Expected Results**:
- All saves succeed
- No NVS corruption
- Data persists

**Pass Criteria**: Handles 100+ save operations

---

## Debugging Procedures

### Serial Monitor Debug Output

Enable Serial Monitor at 115200 baud to view debug messages:

```
ESP32 MacroPad Starting...
Initializing display...
Display initialized
Initializing Bluetooth...
Bluetooth keyboard started - waiting for connection
Loading macros from NVS...
Loaded macro0: Macro 1
Loaded macro1: Macro 2
Loaded macro2: Macro 3
Loaded macro3: Macro 4
Setup complete!
```

### Common Debug Messages

**Touch Events**:
```
Touch detected at: 150, 100
Macro button 0 pressed
Send button pressed
```

**Bluetooth Events**:
```
Bluetooth connected
Sending macro: Hello World
Macro sent!
```

**Configuration**:
```
Settings button pressed
Edit macro 1
Key pressed: h
Save pressed
Saving macro1: hello
Macro saved to NVS
```

### Debugging Tips

1. **Display Issues**:
   - Check `TFT_eSPI` configuration
   - Verify pin definitions
   - Test with example sketches first

2. **Touch Issues**:
   - Run calibration sketch
   - Adjust calibration values
   - Check TOUCH_CS pin

3. **Bluetooth Issues**:
   - Check library version
   - Verify BLE is enabled in Arduino IDE settings
   - Check power supply (USB recommended)

4. **Storage Issues**:
   - Verify partition scheme includes NVS
   - Check Serial Monitor for errors
   - Try erasing flash completely

### Performance Monitoring

Monitor these metrics:
- Touch response time (should be < 200ms)
- Screen refresh rate (should be smooth)
- Bluetooth transmission time
- Memory usage (check for leaks)

---

## Test Results Template

```
Test ID: [Test Number]
Test Name: [Test Name]
Date: [Date]
Tester: [Name]
Firmware Version: [Version]

Test Steps:
[List steps]

Expected Results:
[Expected outcome]

Actual Results:
[What actually happened]

Status: [PASS/FAIL/BLOCKED]

Notes:
[Any additional observations]
```

---

## Conclusion

This testing strategy provides comprehensive coverage of the ESP32 MacroPad firmware. Execute these tests systematically to ensure high quality and reliability. Document all results and update this document with any new test cases discovered during testing.
