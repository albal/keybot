# ESP32 MacroPad - Bluetooth HID Macro Keyboard

Firmware for an ESP32-WROOM-32 with ILI9341 touchscreen display to configure and playback macros via Bluetooth HID.

## Overview

This project implements a custom Bluetooth HID macro keyboard using an ESP32-WROOM-32 microcontroller and a 320x240 TFT LCD touchscreen. The device functions as a wireless macro pad with four configurable buttons that can send pre-configured text strings to any connected computer as keyboard input.

### Key Features

- **4 Configurable Macro Buttons**: Each button can store a custom text string
- **Bluetooth HID Support**: Connects as a standard Bluetooth keyboard
- **Touchscreen Interface**: Intuitive touch-based UI with on-screen keyboard
- **Two-Step Safeguard**: Prevents accidental macro execution
- **Persistent Storage**: Macros saved to ESP32 NVS (survives power cycles)
- **Configuration Mode**: Easy-to-use on-screen QWERTY keyboard for editing macros
- **Real-time Bluetooth Status**: Visual indicator shows connection status

## Hardware Requirements

### Components

1. **ESP32-WROOM-32 Module** (or compatible ESP32 Dev Board)
2. **ILI9341 TFT LCD Display** (320x240 with touch controller XPT2046)
3. **Connecting Wires** (for SPI connections)
4. **USB Cable** (for programming and power)

### Wiring Diagram

Connect the ILI9341 display to the ESP32 as follows:

| ESP32 Pin | ILI9341 Pin | Function |
|-----------|-------------|----------|
| GPIO 23   | SDI/MOSI    | SPI Data Out |
| GPIO 19   | SDO/MISO    | SPI Data In |
| GPIO 18   | SCK         | SPI Clock |
| GPIO 15   | TFT_CS      | Display Chip Select |
| GPIO 2    | DC          | Data/Command |
| GPIO 4    | RESET       | Reset (optional) |
| GPIO 5    | T_CS        | Touch Chip Select |
| 3.3V      | VCC         | Power |
| GND       | GND         | Ground |
| GPIO 21   | LED         | Backlight (optional) |

**Note**: These pins are configurable in the code. Adjust as needed for your hardware.

## Software Requirements

### Option 1: Arduino IDE

1. **Arduino IDE** (version 1.8.x or 2.x)
2. **ESP32 Board Support**:
   - Add to Board Manager URLs: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Install "ESP32 by Espressif Systems"

3. **Required Libraries** (install via Library Manager):
   - **TFT_eSPI** by Bodmer (v2.5.43 or later)
   - **ESP32 BLE Keyboard** by T-vK (v0.3.2 or later)
   - **Preferences** (built-in with ESP32 core)

### Option 2: PlatformIO

1. **PlatformIO IDE** (VS Code extension or CLI)
2. Dependencies are automatically managed via `platformio.ini`

## Installation & Setup

### Step 1: Install Required Software

#### Using Arduino IDE:

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add ESP32 board manager URL
4. Go to **Tools → Board → Board Manager**
5. Search and install "ESP32"
6. Go to **Tools → Manage Libraries**
7. Search and install:
   - "TFT_eSPI"
   - "ESP32 BLE Keyboard"

#### Using PlatformIO:

1. Install PlatformIO IDE extension for VS Code
2. Open project folder
3. PlatformIO will automatically install dependencies

### Step 2: Configure TFT_eSPI Library

The TFT_eSPI library requires configuration for your specific display:

#### Arduino IDE:

1. Navigate to Arduino library folder:
   - Windows: `Documents\Arduino\libraries\TFT_eSPI\`
   - Mac/Linux: `~/Arduino/libraries/TFT_eSPI/`

2. Edit `User_Setup.h` or create a custom setup file

3. Enable these settings:
   ```cpp
   #define ILI9341_DRIVER
   #define TFT_WIDTH  240
   #define TFT_HEIGHT 320
   
   #define TFT_MOSI 23
   #define TFT_MISO 19
   #define TFT_SCLK 18
   #define TFT_CS   15
   #define TFT_DC   2
   #define TFT_RST  4
   #define TOUCH_CS 5
   
   #define LOAD_GLCD
   #define LOAD_FONT2
   #define LOAD_FONT4
   #define LOAD_FONT6
   #define LOAD_FONT7
   #define LOAD_FONT8
   #define SMOOTH_FONT
   
   #define SPI_FREQUENCY  40000000
   #define SPI_READ_FREQUENCY  20000000
   #define SPI_TOUCH_FREQUENCY  2500000
   ```

#### PlatformIO:

Configuration notes are included in `platformio.ini`. Edit the library's `User_Setup.h` after first build.

### Step 3: Upload Firmware

#### Arduino IDE:

1. Open `keybot.ino`
2. Select board: **Tools → Board → ESP32 Dev Module**
3. Select port: **Tools → Port → (your COM port)**
4. Click **Upload**

#### PlatformIO:

1. Open project in VS Code
2. Click **PlatformIO: Upload** (arrow icon)
3. Or use command: `pio run --target upload`

### Step 4: Connect Hardware

1. Power off the ESP32
2. Connect the ILI9341 display according to the wiring diagram
3. Double-check all connections
4. Power on the ESP32

## Usage

### First Boot

1. The device will start in **Playback Mode**
2. Display shows 4 macro buttons labeled "M1", "M2", "M3", "M4"
3. Default text is "Macro 1", "Macro 2", etc.
4. Top-right shows Bluetooth status

### Pairing with Computer

1. On your computer/phone, open Bluetooth settings
2. Look for device named "**ESP32 MacroPad**"
3. Click to pair
4. Status on device should change to "BT: Connected"

### Using Macros (Playback Mode)

1. Touch one of the 4 macro buttons
2. The button will highlight in red
3. A green "SEND" button appears on the opposite side
4. Touch "SEND" to execute the macro
5. Text will be typed on the connected device
6. Touch anywhere else or wait 5 seconds to cancel

**Two-Step Safeguard**: This prevents accidental macro execution. You must explicitly confirm by pressing the "SEND" button.

### Configuring Macros

1. Touch the "SET" button in bottom-right corner
2. You'll enter **Configuration Mode**
3. Touch any of the 4 macro buttons to edit
4. An on-screen keyboard appears
5. Type your desired macro text:
   - Use alphanumeric keys
   - Press "SHIFT" for uppercase/symbols
   - Press "SPACE" for space
   - Press "BKSP" to delete
6. Press "SAVE" when finished
7. Touch "BACK" to return to playback mode

### On-Screen Keyboard Features

- **Full QWERTY layout** with numbers and special characters
- **Shift key**: Toggle between lowercase/uppercase and symbols
- **Space bar**: Add spaces
- **Backspace**: Delete last character
- **Save button**: Save macro and return to config screen
- **Text preview**: See your text as you type

## Features & Functionality

### Main Screen (Playback Mode)

- 4 large, clearly-labeled macro buttons
- Settings button for configuration access
- Real-time Bluetooth connection status
- Visual feedback on button selection
- Safety mechanism with confirmation step

### Configuration Mode

- Select any macro to edit
- Full on-screen keyboard
- Text preview area
- Persistent storage (survives reboot)

### Bluetooth HID

- Advertises as "ESP32 MacroPad"
- Standard HID keyboard profile
- Compatible with Windows, Mac, Linux, Android, iOS
- Automatic reconnection
- Graceful handling of connection/disconnection

### Storage

- Uses ESP32 NVS (Non-Volatile Storage)
- Macros persist across power cycles
- No SD card required
- Namespace: "macropad"
- Keys: "macro0", "macro1", "macro2", "macro3"

## Troubleshooting

### Display Not Working

1. Check all wiring connections
2. Verify power supply (3.3V for display)
3. Ensure TFT_eSPI library is configured correctly
4. Check that correct pins are defined in `User_Setup.h`
5. Try adjusting SPI frequency (lower if unstable)

### Touch Not Responsive

1. Verify TOUCH_CS pin is correct
2. Check touch calibration values in code
3. You may need to calibrate for your specific display:
   ```cpp
   uint16_t calData[5] = {275, 3620, 264, 3532, 1};
   tft.setTouch(calData);
   ```
4. Run TFT_eSPI touch calibration examples to get values

### Bluetooth Issues

1. Ensure BLE Keyboard library is installed correctly
2. Check that device is not already paired with another host
3. Verify ESP32 has sufficient power (USB power recommended)
4. Check Serial Monitor for connection messages
5. Try forgetting device and re-pairing

### Macros Not Saving

1. Check Serial Monitor for error messages
2. Verify Preferences library is available
3. Ensure partition scheme has NVS space
4. Try erasing flash and re-uploading

### Serial Debugging

Enable Serial Monitor at 115200 baud to see debug messages:
- Touch events
- Button presses
- Macro save/load operations
- Bluetooth connection status

## Code Structure

The firmware is organized into logical sections:

- **Setup Functions**: Display, Bluetooth, storage initialization
- **Display Functions**: Screen rendering for different modes
- **Touch Handlers**: Process touch input based on current mode
- **Macro Functions**: Send, save, and load macros
- **Helper Functions**: Utility functions for buttons, text, etc.

### Main Components

1. **TFT_eSPI**: Handles display and touch input
2. **BleKeyboard**: Manages Bluetooth HID functionality
3. **Preferences**: Persistent storage in NVS
4. **State Machine**: Switches between Playback, Config, and Edit modes

## Customization

### Changing Pin Assignments

Edit these definitions at the top of `keybot.ino`:
```cpp
#define TFT_MOSI 23
#define TFT_MISO 19
// ... etc
```

Also update `User_Setup.h` in TFT_eSPI library folder.

### Adjusting Colors

Modify color definitions:
```cpp
#define COLOR_BG          0x0000  // Background
#define COLOR_BUTTON      0x1082  // Button color
// ... etc
```

### Changing Button Layout

Modify these constants:
```cpp
#define MACRO_BTN_WIDTH   140
#define MACRO_BTN_HEIGHT  90
#define MACRO_BTN_MARGIN  10
```

### Adding More Macros

1. Increase array size: `String macros[4]` → `String macros[X]`
2. Update loop counters in load/save functions
3. Adjust button layout calculations

### Modifying Keyboard Layout

Edit the keyboard row definitions:
```cpp
const char* keyboardRows[KEYBOARD_ROWS] = {
  "1234567890",
  "qwertyuiop",
  // ... etc
};
```

## Testing

See the comprehensive testing strategy in the code comments, including:

- Manual test cases
- Unit test examples
- Edge case scenarios
- Debugging tips

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Credits

- **TFT_eSPI Library**: Bodmer
- **ESP32 BLE Keyboard Library**: T-vK
- **ESP32 Arduino Core**: Espressif Systems

## Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Check existing issues for solutions
- Review code comments for detailed documentation
