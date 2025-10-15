# Quick Start Guide - ESP32 MacroPad

Get your ESP32 MacroPad up and running in 15 minutes!

## What You Need

- ✅ ESP32-WROOM-32 development board
- ✅ ILI9341 2.8" or 3.2" touchscreen display
- ✅ 12 jumper wires
- ✅ USB cable
- ✅ Computer with Arduino IDE

## Step 1: Install Arduino IDE (5 minutes)

1. Download Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)
2. Install and open Arduino IDE
3. Go to **File → Preferences**
4. In "Additional Board Manager URLs", add:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
5. Click OK

## Step 2: Install ESP32 Board Support (2 minutes)

1. Go to **Tools → Board → Boards Manager**
2. Search for "ESP32"
3. Install "ESP32 by Espressif Systems"
4. Wait for installation to complete

## Step 3: Install Required Libraries (3 minutes)

1. Go to **Tools → Manage Libraries**
2. Search and install these:
   - **TFT_eSPI** by Bodmer
   - **ESP32 BLE Keyboard** by T-vK

## Step 4: Configure TFT_eSPI (2 minutes)

1. Find TFT_eSPI library folder:
   - Windows: `Documents\Arduino\libraries\TFT_eSPI\`
   - Mac: `~/Documents/Arduino/libraries/TFT_eSPI/`
   - Linux: `~/Arduino/libraries/TFT_eSPI/`

2. Open `User_Setup.h` in a text editor

3. Find and uncomment (remove `//` from):
   ```cpp
   #define ILI9341_DRIVER
   ```

4. Find and set these pin definitions:
   ```cpp
   #define TFT_MISO 19
   #define TFT_MOSI 23
   #define TFT_SCLK 18
   #define TFT_CS   15
   #define TFT_DC   2
   #define TFT_RST  4
   #define TOUCH_CS 5
   ```

5. Save and close

**TIP**: You can copy settings from `User_Setup_Example.h` in this repository!

## Step 5: Wire the Display (3 minutes)

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

## Step 6: Upload Firmware (2 minutes)

1. Open `keybot.ino` in Arduino IDE
2. Connect ESP32 to computer via USB
3. Select board: **Tools → Board → ESP32 Dev Module**
4. Select port: **Tools → Port → (your COM port)**
5. Click **Upload** button (→)
6. Wait for "Done uploading" message

## Step 7: Test! (30 seconds)

1. Display should light up and show the main screen
2. You should see:
   - "ESP32 MacroPad" title
   - 4 macro buttons (M1-M4)
   - "BT: Waiting..." in top-right
   - "SET" button in bottom-right

3. Touch the screen - it should respond!

## Step 8: Pair Bluetooth (1 minute)

1. On your computer/phone, open Bluetooth settings
2. Look for "**ESP32 MacroPad**"
3. Click to pair
4. Device should show "BT: Connected" (green)

## Step 9: Try a Macro! (30 seconds)

1. Open a text editor on your paired device
2. On the MacroPad, touch "M1" button
3. A green "SEND" button appears
4. Touch "SEND"
5. Text should appear in your editor!

## Step 10: Configure Macros (2 minutes)

1. Touch "SET" button (bottom-right)
2. Touch "Edit M1"
3. Use on-screen keyboard to type your macro
4. Press "SAVE"
5. Try sending it!

---

## 🎉 Success!

You now have a working Bluetooth macro keyboard! 

## What's Next?

- Configure all 4 macro buttons with your favorite shortcuts
- Mount it in an enclosure
- Try advanced features like long macros
- Check out the full [README.md](README.md) for more features

## Troubleshooting

### Display is blank
- Check all wire connections
- Verify you edited `User_Setup.h` correctly
- Check power (3.3V to VCC)
- Try the TFT_eSPI example sketches first

### Touch doesn't work
- Verify TOUCH_CS (GPIO 5) is connected
- Remove any protective film from screen
- Run touch calibration example from TFT_eSPI

### Can't find device in Bluetooth
- Check Serial Monitor (115200 baud) for errors
- Verify ESP32 BLE Keyboard library is installed
- Try different USB power source (not a hub)
- Check that Bluetooth is enabled in Arduino IDE settings

### Compile errors
- Make sure all libraries are installed
- Verify board is set to "ESP32 Dev Module"
- Check that ESP32 board support is installed
- Try updating to latest library versions

## Need More Help?

- Read the full [README.md](README.md)
- Check [HARDWARE.md](HARDWARE.md) for detailed wiring
- See [TESTING.md](TESTING.md) for test procedures
- Open an issue on [GitHub](https://github.com/albal/keybot/issues)

---

**Total Time**: ~15 minutes (if everything goes smoothly!)

**Difficulty**: 🔧 Beginner-friendly

**Skills Needed**: Basic computer use, following instructions, connecting wires

No soldering required! Perfect for your first ESP32 project.
