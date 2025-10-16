# ESP32 MacroPad - Bluetooth HID Macro Keyboard

Firmware for an ESP32-WROOM-32 with ILI9341 touchscreen display to configure and playback macros via Bluetooth HID.

**Built with ESP-IDF** - Espressif IoT Development Framework for native ESP32 support and optimal performance.

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
- **Native ESP-IDF**: Uses Espressif's native framework for better performance

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

### ESP-IDF (Required)

1. **ESP-IDF v5.0 or later**
   - Follow the official getting started guide: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/
   - Install prerequisites: Python 3.8+, CMake 3.16+, Git
   - Install ESP-IDF toolchain for your platform (Windows, macOS, Linux)
   - Set up environment variables (`IDF_PATH`)

2. **System Requirements**:
   - Python 3.8 or later
   - CMake 3.16 or later
   - Git
   - ~2GB disk space for ESP-IDF and toolchain

## Installation & Setup

### Step 1: Install ESP-IDF

#### Linux/macOS:

1. **Install prerequisites**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   
   # macOS (with Homebrew)
   brew install cmake ninja dfu-util
   ```

2. **Clone ESP-IDF**:
   ```bash
   mkdir -p ~/esp
   cd ~/esp
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   git checkout v5.1  # or latest stable version
   ```

3. **Install ESP-IDF tools**:
   ```bash
   cd ~/esp/esp-idf
   ./install.sh esp32
   ```

4. **Set up environment**:
   ```bash
   . ~/esp/esp-idf/export.sh
   ```
   
   Add this to your shell profile (~/.bashrc or ~/.zshrc) for convenience:
   ```bash
   alias get_idf='. $HOME/esp/esp-idf/export.sh'
   ```

#### Windows:

1. Download and run the ESP-IDF Installer from:
   https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html

2. The installer will:
   - Install Python, Git, CMake, and other tools
   - Clone ESP-IDF repository
   - Set up environment variables
   - Create desktop shortcuts for ESP-IDF command prompt

### Step 2: Get the Project

1. **Clone this repository**:
   ```bash
   git clone https://github.com/albal/keybot.git
   cd keybot
   ```

### Step 3: Configure and Build

1. **Set up ESP-IDF environment** (if not already done):
   ```bash
   # Linux/macOS
   . ~/esp/esp-idf/export.sh
   
   # Windows - use ESP-IDF Command Prompt
   ```

2. **Configure the project** (optional):
   ```bash
   idf.py menuconfig
   ```
   
   In menuconfig, you can adjust:
   - Serial flasher config → Flash size
   - Component config → Bluetooth → Bluedroid options
   - Component config → ESP32-specific → CPU frequency

3. **Build the project**:
   ```bash
   idf.py build
   ```

### Step 4: Connect Hardware

1. Power off the ESP32
2. Connect the ILI9341 display according to the wiring diagram
3. Double-check all connections
4. Connect ESP32 to computer via USB

### Step 5: Flash Firmware

1. **Find your serial port**:
   ```bash
   # Linux/macOS
   ls /dev/tty*
   # Look for /dev/ttyUSB0 or /dev/ttyACM0
   
   # Windows
   # Use Device Manager to find COM port
   ```

2. **Flash the firmware**:
   ```bash
   idf.py -p /dev/ttyUSB0 flash
   ```
   
   Replace `/dev/ttyUSB0` with your actual port.

3. **Monitor output** (optional):
   ```bash
   idf.py -p /dev/ttyUSB0 monitor
   ```
   
   Press `Ctrl+]` to exit monitor.

4. **Flash and monitor in one command**:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

## Pre-built Firmware

If you don't want to build the firmware yourself, you can download pre-built firmware binaries from the GitHub Releases page.

### Downloading Pre-built Firmware

1. **Visit the Releases page**: https://github.com/albal/keybot/releases
2. **Download the latest release** - Look for the most recent version
3. **Download these files** from the Assets section:
   - `esp32_macropad_merged.bin` (easiest option)
   - OR all three files: `bootloader.bin`, `partition-table.bin`, `esp32_macropad.bin`
   - `FLASH_INSTRUCTIONS.txt` (optional - contains detailed instructions)

### Flashing Pre-built Firmware

#### Easy Method (Merged Binary)

1. **Install esptool.py**:
   ```bash
   pip install esptool
   ```

2. **Connect your ESP32** to your computer via USB

3. **Flash the merged binary**:
   ```bash
   esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
     write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB \
     0x0 esp32_macropad_merged.bin
   ```
   
   Replace `/dev/ttyUSB0` with your actual serial port:
   - Linux/macOS: Usually `/dev/ttyUSB0`, `/dev/ttyACM0`, or `/dev/cu.usbserial-*`
   - Windows: Usually `COM3`, `COM4`, etc.

#### Standard Method (Individual Files)

If you prefer to flash individual components:

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB \
  0x1000 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 esp32_macropad.bin
```

### Verifying the Flash

After flashing, verify the device is working:

```bash
# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor

# Or using screen/minicom
screen /dev/ttyUSB0 115200
```

Press the RESET button on the ESP32, and you should see boot messages including the firmware version.

**Note**: If flashing fails, try holding the BOOT button while connecting the USB cable, then release it when flashing starts.

### Firmware Version Information

The firmware includes automatic version information based on git tags and commits:

- **Released versions** show the tag name (e.g., `v0.1.2`)
- **Development builds** show the tag, commits since tag, and short SHA (e.g., `v0.1.2-3-gabc1234`)
- **Uncommitted changes** append `-dirty` to the version

You can see the version in two places:
1. **Serial logs**: At startup, the firmware logs `Firmware Version: <version>`
2. **Display**: The version appears on the main screen (when UI is fully implemented)

To check your firmware version:
```bash
idf.py -p /dev/ttyUSB0 monitor
```

Look for a line like:
```
I (xxx) MACROPAD: Firmware Version: v0.1.2
```

## Usage

### First Boot

1. The device will start in **Playback Mode**
2. Display shows 4 macro buttons labeled "M1", "M2", "M3", "M4"
3. Default text is "Macro 1", "Macro 2", etc.
4. Top-right shows Bluetooth status
5. Firmware version is displayed (when UI is fully implemented)

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

This project includes comprehensive automated testing to ensure code quality and reliability.

### Automated Unit Tests

The project includes a full suite of unit tests that validate:
- NVS storage operations
- State management logic
- Button touch detection
- String manipulation
- Timeout mechanisms
- Integration workflows

**Running Tests on CI**: Tests automatically run on every pull request via GitHub Actions.

**Running Tests on Hardware**:
```bash
cd test_app
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Tests will run automatically on boot and display results on the serial monitor.

### Manual Testing

See [TESTING.md](TESTING.md) for comprehensive testing documentation, including:
- Automated unit test guide
- Manual test cases for hardware
- Integration test scenarios
- Edge case validation
- Debugging procedures

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
