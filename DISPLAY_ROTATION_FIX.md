# Display Rotation and Touch Screen Fix

## Issue Summary
The display was showing content rotated incorrectly and needed to be rotated 90 degrees clockwise. Additionally, the touch screen was not responding to input properly.

## Root Causes

### 1. Display Rotation Issue
The ILI9341 display controller's MADCTL (Memory Access Control) register was configured with the wrong value:
- **Previous value**: `0x48` (binary: 0b01001000)
- **Issue**: This orientation was incorrect for the hardware setup

### 2. Touch Screen Input Issue
The touch controller (XPT2046) implementation was incomplete:
- Only checked if the screen was pressed (pressure detection)
- Did not read X/Y coordinates
- No coordinate logging for debugging

## Solutions Implemented

### 1. Display Rotation Fix
Changed the MADCTL register value to rotate the display 90 degrees clockwise:

**File**: `main/main.c` (lines 729-735)

```c
// Before:
uint8_t madctl[] = {0x48}; // Landscape mode

// After:
uint8_t madctl[] = {0x28}; // Landscape mode, rotated 90 degrees clockwise
```

**Technical Details**:
- MADCTL register bits control display orientation:
  - Bit 7 (MY): Row Address Order
  - Bit 6 (MX): Column Address Order
  - Bit 5 (MV): Row/Column Exchange
  - Bit 3 (BGR): RGB-BGR Order
- New value `0x28` = `0b00101000`:
  - MY=0, MX=0, MV=1, BGR=1
  - This exchanges rows and columns (MV=1) for 90° rotation

### 2. Touch Screen Input Fix
Implemented proper touch coordinate reading from the XPT2046 controller:

**Added Functions** (lines 918-989):

1. **`xpt2046_read(uint8_t command)`**
   - Low-level SPI communication with touch controller
   - Reads 12-bit raw values from XPT2046
   - Handles bit shifting and masking

2. **`read_touch_coordinates(uint16_t *x, uint16_t *y)`**
   - Reads X and Y coordinates from touch controller
   - First checks pressure to verify touch is valid
   - Averages two readings to reduce noise
   - Returns `true` if touch is detected with valid coordinates
   - XPT2046 commands used:
     - `0xB1`: Read Z1 (pressure)
     - `0xD0`: Read X position
     - `0x90`: Read Y position

3. **Updated `handle_touch_task()`** (lines 1288-1322)
   - Now actively reads and logs touch coordinates
   - Detects touch press and release events
   - Logs coordinates only when they change significantly
   - Polling rate: 20Hz (every 50ms)
   - TODO: Coordinate calibration and mapping to screen pixels

## XPT2046 Touch Controller Details

### Communication Protocol
- **Interface**: SPI (same bus as display, separate CS pin)
- **SPI Speed**: 2 MHz
- **Data Format**: 12-bit values
- **Commands**: 8-bit command byte followed by 2 bytes of response

### Command Format
Commands are sent as 8-bit values with the following format:
- Bit 7: Start bit (always 1)
- Bits 6-4: Channel select (X, Y, Z1, Z2)
- Bit 3: 12-bit/8-bit mode (1 = 12-bit)
- Bit 2: Single-ended/Differential (0 = differential)
- Bits 1-0: Power mode

### Touch Detection
- Pressure threshold: Z1 > 100 indicates touch
- Raw coordinates are 12-bit values (0-4095)
- Averaging reduces noise from resistive touch sensor

## Expected Behavior After Fix

### Display Orientation
- Display content is rotated 90 degrees clockwise from previous orientation
- Visual content (colors, patterns, UI elements) now appear in correct orientation
- Resolution remains 320x240 pixels
- Color format remains RGB565 (16-bit)

### Touch Screen Response
When you touch the screen, you should see in the serial monitor:
```
I (xxxxx) MACROPAD: Touch detected - Raw coordinates: X=1234, Y=2345
I (xxxxx) MACROPAD: Touch detected - Raw coordinates: X=1250, Y=2360
...
I (xxxxx) MACROPAD: Touch released
```

**Coordinate Ranges**:
- X: 0-4095 (12-bit raw value from XPT2046)
- Y: 0-4095 (12-bit raw value from XPT2046)
- These will need calibration to map to screen pixels (0-319 for X, 0-239 for Y)

## Testing Instructions

### 1. Build and Flash Firmware
```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Build the firmware
cd /path/to/keybot
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash monitor
```

### 2. Verify Display Rotation
1. Power on the device
2. Watch the display test sequence that runs automatically
3. Verify that:
   - Color fills (red, green, blue, etc.) fill the entire screen properly
   - Color bars are oriented correctly
   - Checkerboard pattern appears square (not stretched)
   - Corner rectangles appear in the actual corners
   - Center cross is centered

### 3. Verify Touch Screen
1. During the display test, touch the screen at various locations
2. Monitor the serial output for touch coordinates
3. Verify that:
   - Touch is detected when you press the screen
   - Coordinates are logged with reasonable values (not all zeros)
   - "Touch released" message appears when you lift your finger
   - Coordinates change appropriately as you move your finger
   - No false touches when not touching the screen

### 4. Exit Display Test
1. Touch anywhere on the screen during the test sequence
2. The device should exit test mode and switch to normal operation
3. Verify that "Display Test Complete - Touch detected!" appears in logs

## Serial Monitor Output

### Display Initialization
```
I (xxx) MACROPAD: Display: Setting orientation (landscape, 90 degrees clockwise)...
I (xxx) MACROPAD: Display: Orientation set to landscape mode (90 degrees clockwise)
```

### Touch Events
```
I (xxx) MACROPAD: Touch task started
I (xxx) MACROPAD: Touch detected - Raw coordinates: X=2048, Y=2048
I (xxx) MACROPAD: Touch detected - Raw coordinates: X=2055, Y=2060
I (xxx) MACROPAD: Touch released
```

## Calibration (Future Work)

The current implementation reads raw coordinates from the touch controller. For full UI functionality, calibration is needed to:

1. **Map raw coordinates to screen pixels**:
   - Raw X (0-4095) → Screen X (0-319)
   - Raw Y (0-4095) → Screen Y (0-239)

2. **Account for hardware variations**:
   - Different displays may have different touch ranges
   - Calibration involves touching known positions and recording values

3. **Implement coordinate transformation**:
   ```c
   screen_x = ((raw_x - cal_x_min) * 320) / (cal_x_max - cal_x_min);
   screen_y = ((raw_y - cal_y_min) * 240) / (cal_y_max - cal_y_min);
   ```

4. **Handle rotation**:
   - Coordinates may need to be swapped and/or inverted based on display orientation
   - For 90° rotation: swap X/Y and possibly invert

## Technical References

### ILI9341 MADCTL Register
- Datasheet: [ILI9341](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- Section: Memory Data Access Control (0x36)
- Common orientations:
  - `0x48`: Portrait (default)
  - `0x28`: Landscape (90° clockwise) ← Our new value
  - `0x88`: Portrait (180°)
  - `0xE8`: Landscape (270° clockwise / 90° counter-clockwise)

### XPT2046 Touch Controller
- Datasheet: [XPT2046](https://datasheetspdf.com/pdf/746672/XPTEK/XPT2046/1)
- Interface: SPI
- Resolution: 12-bit (0-4095)
- Sample Rate: Up to 125 kHz
- Channels: X, Y, Z1, Z2, Temperature, Battery

## Files Modified

1. **main/main.c**:
   - Line 733: Changed MADCTL value from `0x48` to `0x28`
   - Lines 918-942: Added `xpt2046_read()` function
   - Lines 944-975: Added `read_touch_coordinates()` function
   - Lines 977-989: Updated `check_touch_pressed()` to use new helper
   - Lines 1288-1322: Enhanced `handle_touch_task()` to read and log coordinates
   - Line 320: Added `read_touch_coordinates()` function declaration

## Troubleshooting

### Display Still Rotated Incorrectly
- Verify the firmware was flashed successfully
- Check serial monitor for "Orientation set to landscape mode (90 degrees clockwise)"
- If rotation is still wrong, try different MADCTL values:
  - `0x88` for 180° rotation
  - `0xE8` for 270° rotation

### Touch Not Responding
1. Check wiring connections:
   - Touch CS: GPIO 33
   - Touch MOSI: GPIO 32
   - Touch MISO: GPIO 39
   - Touch CLK: GPIO 25
   - Touch IRQ: GPIO 36
2. Verify touch controller initialization in logs
3. Try pressing harder (resistive touch requires pressure)
4. Check that Z1 values are being read (should be > 100 when touched)

### Touch Coordinates All Zeros or Invalid
1. Verify SPI bus initialization for touch (HSPI)
2. Check that touch controller is powered (3.3V)
3. Verify touch CS pin is correct (GPIO 33)
4. Test with a different SPI speed (currently 2 MHz)

## Summary

This fix addresses both issues reported:
1. ✅ **Display rotation corrected** by changing MADCTL from 0x48 to 0x28
2. ✅ **Touch screen now responds** with coordinate reading implementation

The device will now:
- Display content in the correct orientation (90° clockwise rotation)
- Detect touch events and log coordinates
- Provide feedback via serial monitor for debugging
- Support future UI interaction once coordinate calibration is added

## Next Steps (Not in Scope)

For complete touch functionality, consider implementing:
1. Touch coordinate calibration routine
2. Coordinate mapping to screen pixels
3. Button press detection (checking if touch is within button bounds)
4. Gesture support (swipe, long press, etc.)
5. Multi-touch support (if hardware supports)
