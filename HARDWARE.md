# Hardware Guide for ESP32 MacroPad

This guide provides detailed information about the hardware components, assembly, and optional modifications for the ESP32 MacroPad project.

## Table of Contents

1. [Bill of Materials](#bill-of-materials)
2. [Hardware Requirements](#hardware-requirements)
3. [Assembly Instructions](#assembly-instructions)
4. [Wiring Details](#wiring-details)
5. [Optional Components](#optional-components)
6. [Enclosure Ideas](#enclosure-ideas)
7. [Power Options](#power-options)
8. [Troubleshooting Hardware](#troubleshooting-hardware)

---

## Bill of Materials

### Required Components

| Component | Specifications | Quantity | Estimated Cost | Notes |
|-----------|---------------|----------|----------------|-------|
| ESP32-WROOM-32 | ESP32 Development Board | 1 | $5-15 | Any ESP32 dev board works |
| ILI9341 Display | 320x240 TFT LCD, 2.8" or 3.2" | 1 | $10-20 | Must include XPT2046 touch |
| Jumper Wires | Male-to-Female, ~20cm | 12 | $2-5 | For prototyping |
| USB Cable | Micro-USB or USB-C | 1 | $2-5 | Match your ESP32 board |
| Breadboard | Half or full size | 1 | $3-8 | Optional for prototyping |

**Total Estimated Cost**: $22-53 USD

### Optional Components

| Component | Purpose | Estimated Cost |
|-----------|---------|----------------|
| Perfboard or PCB | Permanent assembly | $2-10 |
| 3D Printed Enclosure | Protection and aesthetics | $5-15 (material) |
| LiPo Battery (3.7V) | Portable operation | $8-20 |
| TP4056 Charging Module | Battery charging | $1-3 |
| Power Switch | On/off control | $1-2 |
| Push Button | Physical reset/boot | $0.50-1 |
| Resistors (330Ω) | LED current limiting | $0.10 each |

---

## Hardware Requirements

### ESP32 Development Board

**Recommended Models**:
- ESP32-WROOM-32 DevKitC
- ESP32 DevKit V1 (DOIT)
- ESP32-WROVER Module (with PSRAM for future expansion)
- NodeMCU-32S

**Minimum Requirements**:
- ESP32 chipset (not ESP8266 or ESP32-S2)
- Bluetooth support (classic or BLE)
- USB programming interface
- At least 4MB flash memory
- Minimum 12 GPIO pins available

**Why ESP32?**
- Dual-core processor for smooth multitasking
- Built-in Bluetooth Classic and BLE
- Hardware SPI for fast display communication
- Ample memory for code and data
- Low cost and wide availability

### ILI9341 TFT Display Module

**Specifications**:
- Resolution: 320x240 pixels
- Controller: ILI9341
- Touch Controller: XPT2046 (resistive touch)
- Interface: SPI
- Voltage: 3.3V or 5V (check your specific module)
- Size: 2.4", 2.8", or 3.2" diagonal

**Important**: 
- Must include touch functionality (XPT2046 controller)
- Ensure it has a separate touch CS pin
- 4-wire resistive touch is standard

**What to Look For**:
- SD card slot (optional, not used in this project)
- LED backlight control pin (recommended)
- 3.3V compatible (important for ESP32)

### Connection Wires

**For Prototyping**:
- 12x Female-to-Female jumper wires (if both have pins)
- OR 12x Female-to-Male jumper wires (if display has holes)
- Length: 10-20cm recommended

**For Permanent Assembly**:
- 22 AWG or 24 AWG hookup wire
- Wire strippers
- Soldering iron and solder
- Heat shrink tubing (optional)

---

## Assembly Instructions

### Step 1: Prepare Components

1. **Unpack all components** and verify you have everything
2. **Inspect ESP32 board** for any obvious damage
3. **Check display module** - look for:
   - No cracked screen
   - All pins present
   - Touch layer intact (no bubbles or separation)

### Step 2: Test ESP32 Board

Before connecting the display, test the ESP32:

1. Connect ESP32 to computer via USB
2. Open Arduino IDE
3. Upload a simple "Blink" sketch to verify board works
4. Check Serial Monitor at 115200 baud

### Step 3: Identify Display Pins

Locate these pins on your ILI9341 module:

**Display Interface**:
- VCC (or VIN) - Power supply
- GND - Ground
- CS - Chip Select for display
- RESET (or RST) - Reset pin
- DC (or D/C, A0) - Data/Command select
- SDI (or MOSI, DIN) - SPI Data In
- SCK (or SCLK, CLK) - SPI Clock
- LED (or BL, LEDA) - Backlight control
- SDO (or MISO, DOUT) - SPI Data Out

**Touch Interface**:
- T_CLK - Touch clock (shares with SCK)
- T_CS - Touch chip select
- T_DIN - Touch data in (shares with MOSI)
- T_DO - Touch data out (shares with MISO)
- T_IRQ - Touch interrupt (optional, not used)

### Step 4: Connect Wires

Follow this wiring carefully:

#### Power Connections
```
ESP32 3.3V    -->    Display VCC
ESP32 GND     -->    Display GND
```

**Important**: Most ILI9341 modules accept 3.3V. Some tolerate 5V, but check your module's specifications!

#### SPI Display Connections
```
ESP32 GPIO 13 -->    Display SDI (MOSI)
ESP32 GPIO 12 -->    Display SDO (MISO)
ESP32 GPIO 14 -->    Display SCK (Clock)
ESP32 GPIO 15 -->    Display CS (Chip Select)
ESP32 GPIO 2  -->    Display DC (Data/Command)
ESP32 EN      -->    Display RST (Reset) - shared with ESP32
```

#### Touch Controller Connections
```
ESP32 GPIO 32 -->    Touch T_DIN (MOSI)
ESP32 GPIO 39 -->    Touch T_DO (MISO)
ESP32 GPIO 25 -->    Touch T_CLK (Clock)
ESP32 GPIO 33 -->    Touch T_CS (Chip Select)
ESP32 GPIO 36 -->    Touch T_IRQ (Interrupt)
(Note: Touch uses separate SPI bus from display)
```

#### Optional Backlight
```
ESP32 GPIO 21 -->    Display LED
OR
Display LED   -->    3.3V (always on)
```

### Step 5: Double-Check Connections

Use a multimeter to verify:
1. No short circuits between VCC and GND
2. All connections are secure
3. No loose wires touching each other

### Step 6: Upload Firmware

1. Open `keybot.ino` in Arduino IDE
2. Configure TFT_eSPI library (see README.md)
3. Select correct board and port
4. Upload firmware
5. Open Serial Monitor (115200 baud)

### Step 7: First Boot

Watch for:
- Display powers on
- UI appears (may need a few seconds)
- Touch is responsive
- Serial Monitor shows initialization messages

---

## Wiring Details

### Pin Mapping Table

**For 2.8inch ESP32-32E Display (QD-TFT2803):**

| Function | ESP32 GPIO | Display Pin | Touch Pin | Notes |
|----------|------------|-------------|-----------|-------|
| Power (3.3V) | 3.3V | VCC | - | Check voltage! |
| Ground | GND | GND | - | Common ground |
| **Display SPI (VSPI)** |  |  |  |  |
| SPI MOSI | GPIO 13 | SDI/MOSI | - | Display data out |
| SPI MISO | GPIO 12 | SDO/MISO | - | Display data in |
| SPI Clock | GPIO 14 | SCK | - | Display clock |
| Display CS | GPIO 15 | CS | - | Display select (active low) |
| Data/Command | GPIO 2 | DC | - | High: data, Low: command |
| Reset | EN | RST | - | Shared with ESP32 reset |
| Backlight | GPIO 21 | LED | - | High: on, Low: off |
| **Touch SPI (HSPI)** |  |  |  | **Separate bus!** |
| SPI MOSI | GPIO 32 | - | T_DIN | Touch data out |
| SPI MISO | GPIO 39 | - | T_DO | Touch data in (input only) |
| SPI Clock | GPIO 25 | - | T_CLK | Touch clock |
| Touch CS | GPIO 33 | - | T_CS | Touch select (active low) |
| Touch IRQ | GPIO 36 | - | T_IRQ | Touch interrupt (input only) |

**Important Notes:**
- Display and touch use **separate SPI buses** on this hardware
- Display uses VSPI pins (GPIO 12, 13, 14)
- Touch uses HSPI pins (GPIO 25, 32, 39)
- GPIO 36 and 39 are input-only pins on ESP32
- Reset pin is shared with ESP32 EN pin (hardware design)

### SPI Bus Configuration

**IMPORTANT:** The 2.8inch ESP32-32E Display uses **two separate SPI buses**:

1. **VSPI for Display (ILI9341)**:
   - MOSI: GPIO 13
   - MISO: GPIO 12
   - SCLK: GPIO 14
   - CS: GPIO 15

2. **HSPI for Touch (XPT2046)**:
   - MOSI: GPIO 32
   - MISO: GPIO 39 (input only)
   - SCLK: GPIO 25
   - CS: GPIO 33

This is different from most other ILI9341 displays that share a single SPI bus. The hardware manufacturer designed it this way for the 2.8inch ESP32-32E board.

### Alternative Pin Configurations

**WARNING:** For the 2.8inch ESP32-32E Display, pin configurations are **FIXED** by the hardware design. The display PCB has hardwired connections to specific ESP32 pins. You **cannot** change these pins without hardware modifications.

**Fixed Display Pins (VSPI)**:
- MOSI: GPIO 13 (hardwired)
- MISO: GPIO 12 (hardwired)
- SCLK: GPIO 14 (hardwired)
- CS: GPIO 15 (hardwired)
- DC: GPIO 2 (hardwired)
- RST: EN pin (hardwired, shared with ESP32 reset)
- LED: GPIO 21 (hardwired)

**Fixed Touch Pins (HSPI)**:
- MOSI: GPIO 32 (hardwired)
- MISO: GPIO 39 (hardwired, input only)
- SCLK: GPIO 25 (hardwired)
- CS: GPIO 33 (hardwired)
- IRQ: GPIO 36 (hardwired, input only)

**For other display modules**, you may be able to configure different pins, but keep these considerations:

**Hardware SPI Pins** (fastest, recommended):
- MOSI: GPIO 23
- MISO: GPIO 19
- SCLK: GPIO 18

**Flexible Pins** (can be changed):
- CS (display): Any GPIO
- DC: Any GPIO
- RST: Any GPIO (or -1 to tie to ESP32 EN)
- Touch CS: Any GPIO (avoid input-only pins)
- Backlight: Any GPIO (PWM-capable for dimming)

**Pins to Avoid on ESP32**:
- GPIO 0: Boot mode selection
- GPIO 2: Boot mode selection (but OK after boot)
- GPIO 5: Boot mode selection (but OK after boot)
- GPIO 12: Boot voltage selection (conflicts with some boards)
- GPIO 15: Boot mode selection (but OK for display CS)
- GPIO 34-39: Input only, can't drive outputs

### Wiring Best Practices

1. **Keep wires short** - Reduces noise and signal integrity issues
2. **Twist power and ground wires** - Reduces interference
3. **Don't run wires parallel to antenna** - Can affect WiFi/BT
4. **Use consistent color coding**:
   - Red: Power (3.3V)
   - Black: Ground
   - Other colors: Signals
5. **Strain relief** - Secure wires so they don't pull on solder joints
6. **Test continuity** - Use multimeter before powering up

---

## Optional Components

### Backlight Control

Control display brightness with PWM:

```cpp
// In setup():
ledcSetup(0, 5000, 8);  // Channel 0, 5kHz, 8-bit
ledcAttachPin(TFT_LED, 0);
ledcWrite(0, 128);  // 50% brightness (0-255)
```

### Battery Power

For portable operation:

**Components Needed**:
- 3.7V LiPo battery (500-2000 mAh)
- TP4056 charging module
- Slide switch
- JST connector (if not included with battery)

**Wiring**:
```
Battery (+) --> Switch --> TP4056 (OUT+) --> ESP32 VIN
Battery (-) --> TP4056 (OUT-) --> ESP32 GND
USB 5V --> TP4056 (IN+)
USB GND --> TP4056 (IN-)
```

**Battery Life Estimates**:
- 500 mAh: ~2 hours
- 1000 mAh: ~4 hours
- 2000 mAh: ~8 hours

(Actual life depends on usage, Bluetooth activity, display brightness)

### Physical Buttons

Add physical buttons for reset or boot:

**Reset Button**:
```
GPIO EN --> Button --> GND
```

**Boot Button** (for firmware upload):
```
GPIO 0 --> Button --> GND
```

### Status LEDs

Add visual indicators:

**Bluetooth Status LED**:
```cpp
#define LED_BT 22
pinMode(LED_BT, OUTPUT);
digitalWrite(LED_BT, bleKeyboard.isConnected() ? HIGH : LOW);
```

### SD Card (Future Expansion)

Many displays include SD card slots. Could be used for:
- Storing more macros
- Configuration files
- Logging keystrokes sent
- Firmware updates

---

## Enclosure Ideas

### 3D Printed Enclosure

**Design Considerations**:
- Cutout for display
- Access to USB port
- Mounting posts for ESP32 board
- Ventilation holes (ESP32 generates some heat)
- Space for wiring
- Optional: Battery compartment
- Optional: Mounting holes for VESA or desk mount

**3D Printing Settings**:
- Material: PLA or PETG
- Layer height: 0.2mm
- Infill: 20%
- Supports: Yes (for overhangs)

**Design Resources**:
- Thingiverse
- Printables
- GrabCAD
- Fusion 360 (free for hobbyists)
- TinkerCAD (browser-based, easy)

### DIY Enclosure Materials

- **Project box** (electronics hobby box)
- **Laser-cut acrylic** (2-3mm thick)
- **Wood** (balsa, plywood)
- **Cardboard** (quick prototype)

### Mounting Options

- **Desktop stand**: Angled for easy viewing/touching
- **Wall mount**: Screw holes in back
- **VESA mount**: 75x75mm or 100x100mm pattern
- **Magnetic mount**: Attach to metal surfaces
- **Velcro strips**: Temporary mounting

---

## Power Options

### USB Power (Recommended)

**Advantages**:
- Always available
- Reliable
- No battery maintenance
- Higher current capability

**Disadvantages**:
- Requires cable
- Not portable
- Tied to computer/charger

**Current Draw**:
- ESP32: 80-160 mA (idle/active)
- Display: 50-150 mA
- Bluetooth: 30-100 mA
- **Total**: ~200-400 mA typical

### Battery Power

**Advantages**:
- Portable
- No cables
- Can be carried between computers

**Disadvantages**:
- Requires charging
- Limited runtime
- Battery degrades over time
- Slightly more complex

**Recommended Batteries**:
- 500 mAh: Compact, 2-3 hours
- 1000 mAh: Good balance, 4-5 hours
- 2000 mAh: Long runtime, 8-10 hours

### Power Saving Tips

1. **Reduce display brightness**
2. **Sleep mode** when not in use (future feature)
3. **Turn off WiFi** (not used in this project)
4. **Optimize Bluetooth** (use BLE instead of Classic)
5. **CPU frequency** scaling (reduce from 240MHz to 160MHz)

---

## Troubleshooting Hardware

### Display Not Working

**Symptoms**: Blank screen, white screen, random pixels

**Checks**:
1. ✓ Verify power connections (3.3V to VCC, GND to GND)
2. ✓ Check all SPI connections (MOSI, MISO, SCLK, CS, DC, RST)
3. ✓ Verify TFT_eSPI configuration matches your wiring
4. ✓ Try slower SPI frequency (20MHz instead of 40MHz)
5. ✓ Check if display is 3.3V or 5V compatible
6. ✓ Test with TFT_eSPI example sketches first

**Solutions**:
- Re-seat all connections
- Use shorter wires
- Add 0.1µF capacitor between VCC and GND (near display)
- Check for cold solder joints
- Verify display is not damaged

### Touch Not Responding

**Symptoms**: Touch doesn't register or is inaccurate

**Checks**:
1. ✓ Verify TOUCH_CS pin connection
2. ✓ Check touch shares MOSI, MISO, SCLK with display
3. ✓ Run touch calibration example
4. ✓ Check for protective film on display (remove it!)
5. ✓ Try different touch pressure

**Solutions**:
- Adjust calibration values in code
- Clean display surface
- Check for damaged touch layer
- Verify T_IRQ not required (some modules need it grounded)

### Bluetooth Issues

**Symptoms**: Device not discoverable, won't pair

**Checks**:
1. ✓ Verify BleKeyboard library installed
2. ✓ Check ESP32 has sufficient power (try different USB port)
3. ✓ Verify Bluetooth enabled in Arduino IDE (Tools menu)
4. ✓ Check Serial Monitor for error messages
5. ✓ Try pairing with different devices

**Solutions**:
- Use USB charger (not computer USB hub)
- Add 100µF capacitor near ESP32 power pins
- Update BleKeyboard library to latest version
- Factory reset ESP32 (erase flash completely)

### Intermittent Resets

**Symptoms**: ESP32 randomly resets, brown-out detector triggers

**Causes**:
- Insufficient power supply
- Voltage drops during Bluetooth transmission
- Display backlight surge current

**Solutions**:
- Use powered USB hub or wall adapter (2A rated)
- Add bulk capacitance (470µF or 1000µF) near ESP32
- Reduce display brightness
- Check for short circuits

### Hot Components

**Normal**:
- ESP32 warm to touch (40-50°C) during operation
- Display warm on back side

**Problem**:
- ESP32 too hot to touch (>70°C)
- Display very hot
- Voltage regulator very hot

**Solutions**:
- Check for short circuits
- Verify correct voltage (3.3V not 5V if required)
- Add heatsink to voltage regulator
- Improve ventilation in enclosure

---

## Advanced Hardware Modifications

### Performance Upgrades

1. **Use ESP32-WROVER** (8MB PSRAM) for future expansion
2. **Higher resolution display** (480x320)
3. **Capacitive touch** (more responsive than resistive)
4. **Faster SPI** (up to 80MHz if stable)

### Feature Additions

1. **Physical macro buttons** (GPIO inputs with interrupts)
2. **Rotary encoder** (for scrolling, adjusting settings)
3. **NeoPixel LED strip** (visual feedback)
4. **Buzzer/Speaker** (audio feedback)
5. **Accelerometer** (shake to activate, orientation detection)
6. **External antenna** (better Bluetooth range)

### Professional Build

1. **Custom PCB** (cleaner, more reliable)
2. **SMD components** (smaller footprint)
3. **Injection molded enclosure**
4. **Laser-etched labels**
5. **Silicone button overlays**
6. **Acrylic cover** (screen protection)

---

## Safety Notes

⚠️ **Important Safety Information**:

1. **Electrostatic Discharge (ESD)**:
   - Touch grounded metal before handling components
   - Work on ESD-safe mat if available
   - Avoid carpet and synthetic clothing

2. **Soldering Safety**:
   - Use in well-ventilated area
   - Don't breathe solder fumes
   - Keep iron away from cables and skin
   - Unplug when not in use

3. **Battery Safety**:
   - Don't short circuit battery
   - Don't puncture or crush battery
   - Charge in fire-safe area
   - Dispose of damaged batteries properly
   - Use proper charging circuit (TP4056)

4. **General**:
   - Don't exceed voltage ratings
   - Check polarity before connecting
   - Use appropriate wire gauge
   - Secure all connections
   - Test with multimeter before power-on

---

## Resources

### Where to Buy Components

**General Electronics**:
- Adafruit
- SparkFun
- Digi-Key
- Mouser
- Newark/Element14

**Affordable Options**:
- AliExpress
- Banggood
- eBay
- Amazon

**Local**:
- Electronics hobby shops
- Maker spaces
- University surplus stores

### Datasheets

- [ESP32 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [ILI9341 Datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- [XPT2046 Datasheet](https://datasheetspdf.com/pdf/746672/XPTEK/XPT2046/1)

### Community Support

- [ESP32 Forum](https://esp32.com/)
- [Arduino Forum](https://forum.arduino.cc/)
- [Reddit r/esp32](https://www.reddit.com/r/esp32/)
- [GitHub Issues](https://github.com/albal/keybot/issues)

---

This hardware guide should help you successfully build your ESP32 MacroPad. If you encounter issues not covered here, please open an issue on the GitHub repository!
