# Pin Configuration Fix for 2.8inch ESP32-32E Display

## Issue Summary

**Problem**: White screen during display test phase and no response to touch input.

**Root Cause**: Incorrect pin assignments in the firmware. The code was using standard ESP32 HSPI/VSPI pins that don't match the actual hardware design of the 2.8inch ESP32-32E Display board (QD-TFT2803).

## Hardware Specification

The 2.8inch ESP32-32E Display uses a **unique pin configuration** with two separate SPI buses:

1. **Display (ILI9341)** uses VSPI pins
2. **Touch Controller (XPT2046)** uses HSPI pins

This is different from most ILI9341 displays where display and touch share the same SPI bus.

Reference: [QD-TFT2803 Specification v1.1](https://www.lcdwiki.com/res/E32R28T/QD-TFT2803_specification_v1.1.pdf)

## Pin Changes

### Display SPI (VSPI)

| Signal | Old Pin | New Pin | Notes |
|--------|---------|---------|-------|
| MOSI   | GPIO 23 | GPIO 13 | Changed to VSPI |
| MISO   | GPIO 19 | GPIO 12 | Changed to VSPI |
| SCLK   | GPIO 18 | GPIO 14 | Changed to VSPI |
| CS     | GPIO 15 | GPIO 15 | ✓ No change |
| DC     | GPIO 2  | GPIO 2  | ✓ No change |
| RST    | GPIO 4  | -1 (EN) | Shared with ESP32 reset |
| LED    | GPIO 21 | GPIO 21 | ✓ No change |

### Touch Controller (HSPI)

| Signal | Old Pin | New Pin | Notes |
|--------|---------|---------|-------|
| MOSI   | Shared  | GPIO 32 | Separate HSPI bus |
| MISO   | Shared  | GPIO 39 | Separate HSPI bus (input only) |
| SCLK   | Shared  | GPIO 25 | Separate HSPI bus |
| CS     | GPIO 5  | GPIO 33 | Changed |
| IRQ    | N/A     | GPIO 36 | Added (input only) |

## Code Changes

### 1. Pin Definitions (main.c)

**Before:**
```c
#define PIN_MOSI        23
#define PIN_MISO        19
#define PIN_SCLK        18
#define PIN_TFT_CS      15
#define PIN_TFT_DC      2
#define PIN_TFT_RST     4
#define PIN_TOUCH_CS    5
#define PIN_BACKLIGHT   21
```

**After:**
```c
// Display SPI pins (VSPI)
#define PIN_TFT_MOSI    13
#define PIN_TFT_MISO    12
#define PIN_TFT_SCLK    14
#define PIN_TFT_CS      15
#define PIN_TFT_DC      2
#define PIN_TFT_RST     -1  // Shared with EN
#define PIN_BACKLIGHT   21

// Touch controller pins (separate SPI)
#define PIN_TOUCH_MOSI  32
#define PIN_TOUCH_MISO  39
#define PIN_TOUCH_SCLK  25
#define PIN_TOUCH_CS    33
#define PIN_TOUCH_IRQ   36
```

### 2. SPI Bus Initialization (main.c)

**Before:**
- Single SPI bus (HSPI) shared by display and touch

**After:**
- VSPI for display (GPIO 12, 13, 14)
- HSPI for touch (GPIO 25, 32, 39)

### 3. Reset Pin Handling (main.c)

**Before:**
- Used GPIO 4 as dedicated reset pin

**After:**
- Uses -1 to indicate shared EN pin
- Skip hardware reset in `ili9341_reset()` when RST = -1
- Conditional GPIO configuration in `display_init()`

### 4. Touch IRQ Configuration (main.c)

**Added:**
- GPIO 36 configured as input with pull-up
- Ready for interrupt-based touch detection (currently polling)

## Why This Fix Works

### 1. Correct Display SPI Communication
- Display now uses the actual VSPI pins (12, 13, 14) that are physically connected on the PCB
- SPI commands and data can now reach the ILI9341 controller
- Screen will initialize properly and show graphics instead of white screen

### 2. Correct Touch SPI Communication
- Touch controller now uses separate HSPI pins (25, 32, 39) as designed
- No SPI bus conflicts between display and touch
- Touch readings will be accurate and responsive

### 3. Hardware Reset Compatibility
- Respects the shared EN/RST pin design
- Avoids inadvertently resetting the ESP32 when trying to reset the display
- Uses software reset command instead

### 4. Input-Only Pin Support
- GPIO 36 and 39 are correctly used as input-only pins
- No attempt to configure them as outputs (which would fail)

## Expected Behavior After Fix

### Display Test Sequence
✓ Display initializes successfully  
✓ Screen shows color patterns (red, green, blue, etc.)  
✓ Color bars, checkerboard, and other test patterns appear  
✓ No white screen

### Touch Input
✓ Touch controller initializes on correct SPI bus  
✓ Screen responds to touch during test sequence  
✓ Touch press detected and exits test mode  
✓ Touch coordinates read accurately

### Serial Monitor Output
```
I (xxx) MACROPAD: Display: Hardware: 2.8inch ESP32-32E Display (QD-TFT2803)
I (xxx) MACROPAD: Configuring VSPI for display (MOSI:13, MISO:12, SCLK:14)...
I (xxx) MACROPAD: VSPI initialized for display
I (xxx) MACROPAD: Configuring HSPI for touch (MOSI:32, MISO:39, SCLK:25)...
I (xxx) MACROPAD: HSPI initialized for touch controller
I (xxx) MACROPAD: Display: Control pins configured (RST: shared EN, DC: GPIO2)
I (xxx) MACROPAD: Touch: XPT2046 initialized (CS: GPIO33, IRQ: GPIO36, Clock: 2MHz)
```

## Testing the Fix

### 1. Build the Firmware
```bash
idf.py build
```

### 2. Flash to Device
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

### 3. Verify Display Test
- Power on the device
- Display should show color test patterns
- Touch the screen to exit test mode
- Device should transition to normal operation

### 4. Check Serial Output
- Monitor shows all initialization messages
- No SPI errors
- Touch controller detected
- Display initialized successfully

## Important Notes

### Pin Configuration is Hardware-Fixed

⚠️ **Warning**: For the 2.8inch ESP32-32E Display, these pins are **hardwired** on the PCB and cannot be changed in software without hardware modifications.

If you are using a **different display module**, you may need different pin configurations. Consult your display's documentation.

### Input-Only Pins

GPIO 36 and 39 are **input-only** pins on ESP32:
- Cannot be used as outputs
- Cannot use internal pull-down (only pull-up)
- Perfect for MISO and IRQ signals

### Shared Reset Pin

The display RST is connected to the ESP32 EN pin:
- Both ESP32 and display reset together on power-up
- Cannot toggle display reset without resetting ESP32
- Software reset (0x01 command) used instead

## Files Modified

1. **main/main.c**
   - Pin definitions updated
   - SPI initialization split into two buses
   - Reset pin handling made conditional
   - Touch IRQ pin configured
   - Documentation comments updated

2. **HARDWARE.md**
   - Pin mapping table updated
   - SPI bus configuration explained
   - Wiring instructions corrected
   - Added warning about fixed pins

3. **README.md**
   - Wiring diagram updated
   - Separate tables for display and touch
   - Note about hardware-fixed pins

## References

- [2.8inch ESP32-32E Display Wiki](https://www.lcdwiki.com/2.8inch_ESP32-32E_Display)
- [QD-TFT2803 Specification v1.1 PDF](https://www.lcdwiki.com/res/E32R28T/QD-TFT2803_specification_v1.1.pdf)
- [ILI9341 Datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- [XPT2046 Touch Controller Datasheet](https://datasheetspdf.com/pdf/746672/XPTEK/XPT2046/1)
- [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)

## Credits

Fix developed based on the QD-TFT2803 hardware specification and pin assignment table provided in the issue report.
