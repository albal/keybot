# Quick Start Guide - ESP32 MacroPad

Get your ESP32 MacroPad up and running in 30 minutes using ESP-IDF!

## What You Need

- ✅ ESP32-WROOM-32 development board
- ✅ ILI9341 2.8" or 3.2" touchscreen display
- ✅ 12 jumper wires
- ✅ USB cable
- ✅ Computer with ESP-IDF installed

## Step 1: Install ESP-IDF (10-15 minutes)

### Linux/macOS:

1. **Install prerequisites**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   
   # macOS
   brew install cmake ninja dfu-util
   ```

2. **Get ESP-IDF**:
   ```bash
   mkdir -p ~/esp
   cd ~/esp
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   git checkout v5.1
   ./install.sh esp32
   ```

3. **Set environment**:
   ```bash
   . ~/esp/esp-idf/export.sh
   ```

### Windows:

1. Download ESP-IDF Tools Installer from:
   https://dl.espressif.com/dl/esp-idf/

2. Run installer - it handles everything automatically

3. Use "ESP-IDF Command Prompt" from Start Menu

## Step 2: Get the Project (1 minute)

```bash
git clone https://github.com/albal/keybot.git
cd keybot
```

## Step 3: Wire the Display (3 minutes)

Connect wires from ESP32 to Display:

| ESP32 Pin | → | Display Pin |
|-----------|---|-------------|
| 3.3V | → | VCC |
| GND | → | GND |
| GPIO 23 | → | MOSI/SDI |
| GPIO 19 | → | MISO/SDO |
| GPIO 18 | → | SCK |
| GPIO 15 | → | CS |
| GPIO 2 | → | DC |
| GPIO 4 | → | RESET |
| GPIO 5 | → | T_CS |
| GPIO 21 | → | LED (optional) |

**⚠️ IMPORTANT**: 
- Connect 3.3V to VCC (not 5V!)
- Double-check GND is connected

## Step 4: Build and Flash (5 minutes)

1. **Set up environment** (if not already done):
   ```bash
   . ~/esp/esp-idf/export.sh  # Linux/macOS
   # or use ESP-IDF Command Prompt on Windows
   ```

2. **Navigate to project**:
   ```bash
   cd keybot
   ```

3. **Build the project**:
   ```bash
   idf.py build
   ```

4. **Flash to ESP32**:
   ```bash
   # Linux/macOS (replace /dev/ttyUSB0 with your port)
   idf.py -p /dev/ttyUSB0 flash monitor
   
   # Windows (replace COM3 with your port)
   idf.py -p COM3 flash monitor
   ```

5. **Monitor output**:
   - You'll see the bootloader and application logs
   - Press `Ctrl+]` to exit monitor

## Step 5: Test! (30 seconds)

1. After flashing, check the serial monitor output
2. You should see:
   - "ESP32 MacroPad Starting..."
   - "Initializing NVS..."
   - "Initializing GPIO..."
   - "Initializing SPI..."
   - "Initialization complete!"

3. Display should show main UI (when fully implemented)

## Step 6: Verify Functionality

Check the serial monitor for initialization messages:
```
I (xxx) MACROPAD: ESP32 MacroPad Starting...
I (xxx) MACROPAD: ESP-IDF Version: v5.x
I (xxx) MACROPAD: Initializing NVS...
I (xxx) MACROPAD: NVS initialized
I (xxx) MACROPAD: Initializing GPIO...
I (xxx) MACROPAD: GPIO initialized
I (xxx) MACROPAD: Initializing SPI...
I (xxx) MACROPAD: SPI bus initialized
I (xxx) MACROPAD: Loading macros from NVS...
I (xxx) MACROPAD: Initialization complete!
```

---

## 🎉 Basic Setup Complete!

You now have the ESP-IDF project structure set up and running!

## Understanding ESP-IDF Structure

Your project now has:
```
keybot/
├── CMakeLists.txt          # Root build config
├── main/
│   ├── CMakeLists.txt      # Main component config
│   └── main.c              # Main application
├── components/             # Custom components (optional)
└── sdkconfig               # Project configuration (generated)
```

## Next Steps

The current implementation provides the framework. To complete the project:

1. **Implement Display Driver**: Add ILI9341 and XPT2046 support
2. **Implement BLE HID**: Use Bluedroid stack for keyboard emulation
3. **Implement UI**: Add touch handling and screen rendering
4. **Test**: Follow TESTING.md procedures

## ESP-IDF Resources

- **Official Documentation**: https://docs.espressif.com/projects/esp-idf/
- **API Reference**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/
- **Examples**: 
  - BLE HID: `examples/bluetooth/bluedroid/ble/ble_hid_device_demo`
  - SPI Display: `examples/peripherals/spi_master`
  - LCD: `examples/peripherals/lcd/` 

## What's Next?

- Configure all 4 macro buttons with your favorite shortcuts
- Mount it in an enclosure
- Try advanced features like long macros
- Check out the full [README.md](README.md) for more features

## Troubleshooting

### Build Errors

**Error: IDF_PATH not set**
```bash
# Solution: Set up environment
. ~/esp/esp-idf/export.sh
```

**Error: CMake not found**
```bash
# Install CMake
sudo apt-get install cmake  # Linux
brew install cmake          # macOS
```

**Error: Python packages missing**
```bash
# Install Python requirements
cd ~/esp/esp-idf
./install.sh esp32
```

### Flash Errors

**Error: Failed to connect to ESP32**
- Check USB cable connection
- Try different USB port
- Press and hold BOOT button while flashing
- Check if correct port is specified

**Error: Permission denied on /dev/ttyUSB0**
```bash
# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER
# Log out and log back in
```

### Runtime Issues

**ESP32 keeps resetting**
- Check serial monitor for crash logs
- Verify power supply (use USB 2.0 port or powered hub)
- Check for short circuits in wiring

**No output in monitor**
- Verify correct baud rate (115200 default)
- Try: `idf.py -p /dev/ttyUSB0 monitor`
- Press EN/RESET button on ESP32

## ESP-IDF Commands Quick Reference

```bash
# Set environment
. ~/esp/esp-idf/export.sh

# Full clean build
idf.py fullclean
idf.py build

# Configure project
idf.py menuconfig

# Flash and monitor
idf.py -p PORT flash monitor

# Erase flash
idf.py -p PORT erase-flash

# Monitor only
idf.py -p PORT monitor

# Check IDF version
idf.py --version
```

## Need More Help?

- Read the full [README.md](README.md)
- Check [HARDWARE.md](HARDWARE.md) for detailed wiring
- See [TESTING.md](TESTING.md) for test procedures
- ESP-IDF Documentation: https://docs.espressif.com/projects/esp-idf/
- ESP32 Forum: https://esp32.com
- Open an issue on [GitHub](https://github.com/albal/keybot/issues)

---

**Total Time**: ~30 minutes (first time with ESP-IDF setup)

**Difficulty**: 🔧🔧 Intermediate (requires command line comfort)

**Skills Needed**: Command line usage, following instructions, basic C knowledge

**Why ESP-IDF?**
- Native ESP32 support
- Better performance and efficiency
- More control over hardware
- Industry-standard framework
- Comprehensive documentation
