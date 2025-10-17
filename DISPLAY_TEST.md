# Display Test Sequence

## Overview

The ESP32 MacroPad includes a comprehensive display test sequence that runs automatically when the device starts. This test helps verify that the ILI9341 display and XPT2046 touch controller are functioning correctly.

## Purpose

The display test sequence serves several purposes:

1. **Hardware Verification**: Confirms that the display is properly connected and initialized
2. **Color Accuracy**: Tests all primary and secondary colors to verify RGB565 color rendering
3. **Pixel Integrity**: Uses patterns to detect dead pixels or display defects
4. **Touch Detection**: Validates that the touch controller is responsive
5. **User Feedback**: Provides immediate visual confirmation that the device is working

## Test Sequence

The display test runs through the following visual patterns in a continuous loop until the touchscreen is pressed:

### Test 1: Primary Colors
- **Red Screen** (1 second)
- **Green Screen** (1 second)
- **Blue Screen** (1 second)

Tests the three primary color channels individually.

### Test 2: Secondary Colors
- **Yellow Screen** (Red + Green, 1 second)
- **Cyan Screen** (Green + Blue, 1 second)
- **Magenta Screen** (Red + Blue, 1 second)

Tests color mixing and all three channels working together.

### Test 3: Grayscale
- **White Screen** (All channels at maximum, 1 second)
- **Gray Screen** (All channels at mid-level, 1 second)
- **Black Screen** (All channels off, 1 second)

Tests the full range of brightness levels.

### Test 4: Vertical Color Bars
Displays 8 vertical bars across the screen (2 seconds):
- Red, Green, Blue, Yellow, Cyan, Magenta, White, Black

Tests horizontal pixel rendering and color transitions.

### Test 5: Horizontal Color Bars
Displays 8 horizontal bars down the screen (2 seconds):
- Red, Green, Blue, Yellow, Cyan, Magenta, White, Black

Tests vertical pixel rendering and color transitions.

### Test 6: Checkerboard Pattern
Alternating white and black squares (40x40 pixels each, 2 seconds)

Tests pixel-level rendering and helps identify dead pixels.

### Test 7: Corner Rectangles
Four colored rectangles in the screen corners (2 seconds):
- Top-left: Red
- Top-right: Green
- Bottom-left: Blue
- Bottom-right: Yellow

Tests edge rendering and corner pixel accuracy.

### Test 8: Center Cross Pattern
White cross at the center of the screen (2 seconds):
- Vertical bar: 10 pixels wide, full height
- Horizontal bar: 10 pixels tall, full width

Tests centering and alignment accuracy.

## How to Use

1. **Power On**: Connect the ESP32 to power or upload the firmware
2. **Watch Tests**: The display test sequence will start automatically
3. **Observe Patterns**: Look for any visual defects, dead pixels, or color issues
4. **Exit Test**: Touch anywhere on the screen to exit the test sequence
5. **Normal Operation**: After touch is detected, the device switches to normal playback mode

## Touch Detection

The test uses the XPT2046 touch controller to detect when the screen is pressed:

- **Detection Method**: Reads the Z1 pressure value from the touch controller
- **Threshold**: Touch is detected when Z1 value exceeds 100 (on a 12-bit scale)
- **Polling**: Checks for touch between each test pattern
- **Debounce**: Waits for touch release before transitioning to normal mode

## Implementation Details

### Key Components

1. **Display Helper Functions**:
   - `ili9341_fill_screen()`: Fills entire screen with a color
   - `ili9341_fill_rect()`: Fills a rectangular area with a color
   - `ili9341_set_addr_window()`: Sets the pixel address window for drawing

2. **Touch Detection**:
   - `check_touch_pressed()`: Reads XPT2046 touch controller via SPI
   - Returns `true` if screen is being touched

3. **Test Sequence**:
   - `run_display_test()`: Main test loop that runs all visual patterns

### Technical Specifications

- **Display**: ILI9341 320x240 pixels, RGB565 color format (16-bit)
- **SPI Speed**: 26 MHz for display, 2 MHz for touch controller
- **Color Depth**: 65,536 colors (5-bit Red, 6-bit Green, 5-bit Blue)
- **Test Duration**: Each pattern displays for 1-2 seconds
- **Loop Behavior**: Tests repeat continuously until touch is detected

### Code Location

The display test implementation is located in `main/main.c`:

- **Lines 760-830**: Display helper functions
- **Lines 832-866**: Touch detection function
- **Lines 868-1058**: Display test sequence
- **Lines 1188-1230**: UI task integration

## Troubleshooting

### Display Not Showing Patterns

1. Check wiring connections (especially SPI pins: MOSI, MISO, SCLK, CS, DC, RST)
2. Verify power supply is stable (3.3V)
3. Check serial monitor for initialization errors
4. Confirm ILI9341 initialization completed successfully

### Touch Not Responding

1. Verify TOUCH_CS pin is connected (GPIO 5)
2. Check that touch shares MOSI, MISO, SCLK with display
3. Ensure touch controller is powered
4. Try pressing harder on the screen (resistive touch requires some pressure)
5. Check serial logs for touch controller initialization

### Colors Look Wrong

1. Verify RGB565 color format is correct
2. Check cable integrity (poor connections can cause color channel dropout)
3. Confirm orientation setting (landscape mode)
4. Test with different SPI speeds if colors appear garbled

### Test Exits Immediately

1. Check for false touch detection (noise on touch lines)
2. Verify touch controller threshold is appropriate (currently 100)
3. Ensure touch lines are not floating or shorted

## Customization

You can customize the display test by modifying the following in `main/main.c`:

### Change Test Duration
```c
vTaskDelay(pdMS_TO_TICKS(1000));  // Change 1000 to desired milliseconds
```

### Modify Touch Threshold
```c
return (z1 > 100);  // Change 100 to higher/lower value
```

### Add New Test Patterns
Add new test cases in the `run_display_test()` function between existing tests.

### Disable Display Test
Change the initial mode in the app_state structure:
```c
static app_state_t app_state = {
    .mode = MODE_PLAYBACK,  // Change from MODE_DISPLAY_TEST
    // ...
};
```

## References

- [LCD Wiki - 2.8inch ESP32-32E Display](https://www.lcdwiki.com/2.8inch_ESP32-32E_Display)
- [ILI9341 Datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- [XPT2046 Touch Controller](https://datasheetspdf.com/pdf/746672/XPTEK/XPT2046/1)
- [ESP-IDF SPI Master Driver Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html)

## See Also

- [HARDWARE.md](HARDWARE.md) - Hardware assembly and wiring guide
- [TESTING.md](TESTING.md) - Comprehensive testing documentation
- [DISPLAY_INIT_FIX.md](DISPLAY_INIT_FIX.md) - Display initialization details
