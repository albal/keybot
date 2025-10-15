# ESP32 MacroPad - Project Overview

## 📋 Table of Contents

1. [Project Summary](#project-summary)
2. [Quick Navigation](#quick-navigation)
3. [Project Structure](#project-structure)
4. [Key Features](#key-features)
5. [Getting Started Paths](#getting-started-paths)
6. [Technical Architecture](#technical-architecture)
7. [Development Status](#development-status)
8. [Contributing](#contributing)

---

## 🎯 Project Summary

**ESP32 MacroPad** is a custom Bluetooth HID macro keyboard built on the ESP32-WROOM-32 platform with a touchscreen interface. It allows users to configure and execute up to 4 text macros with a two-step confirmation system, preventing accidental keystrokes.

**Key Stats**:
- 🔧 1,071 lines of production-ready C++ code
- 📚 9 comprehensive documentation files
- 🎨 Full touchscreen UI with on-screen keyboard
- 🔐 Persistent storage (survives power cycles)
- 📡 Bluetooth HID keyboard emulation
- ⚡ Non-blocking, responsive main loop

---

## 🗺️ Quick Navigation

### For First-Time Users
👉 **Start here**: [QUICK_START.md](QUICK_START.md) (15-minute setup)

### For Builders & Makers
👉 **Hardware Guide**: [HARDWARE.md](HARDWARE.md) (Bill of Materials, assembly, wiring)

### For Developers
👉 **Source Code**: [keybot.ino](keybot.ino) (complete firmware)  
👉 **Testing**: [TESTING.md](TESTING.md) (test cases and strategies)

### For Users Looking for Ideas
👉 **Example Macros**: [EXAMPLES.md](EXAMPLES.md) (programming, office, gaming, etc.)

### For Complete Documentation
👉 **README**: [README.md](README.md) (comprehensive guide)

---

## 📁 Project Structure

```
keybot/
├── keybot.ino                  # Main firmware (1,071 lines)
│                               # Complete C++ Arduino sketch
│
├── Documentation/
│   ├── README.md              # Comprehensive user guide (11 KB)
│   ├── QUICK_START.md         # 15-minute getting started (4.7 KB)
│   ├── HARDWARE.md            # Assembly & BOM guide (16 KB)
│   ├── TESTING.md             # Test strategy & cases (21 KB)
│   ├── EXAMPLES.md            # Macro examples (11 KB)
│   ├── CHANGELOG.md           # Version history (3.7 KB)
│   └── PROJECT_OVERVIEW.md    # This file
│
├── Configuration/
│   ├── platformio.ini         # PlatformIO config
│   ├── library.json           # Dependency manifest
│   └── User_Setup_Example.h   # TFT_eSPI configuration
│
└── Meta/
    ├── LICENSE                # MIT License
    └── .gitignore            # Git ignore rules
```

---

## ✨ Key Features

### Hardware Features
- ✅ ESP32-WROOM-32 microcontroller (240MHz dual-core)
- ✅ ILI9341 320x240 TFT touchscreen (resistive touch)
- ✅ Hardware SPI for fast display updates
- ✅ USB-powered or battery operation
- ✅ Compact form factor (~3" diagonal display)

### Software Features

#### User Interface
- ✅ Clean, simple main screen with 4 large macro buttons
- ✅ Visual feedback on button press (highlight)
- ✅ Real-time Bluetooth connection status
- ✅ Configuration mode with on-screen keyboard
- ✅ Text preview while editing

#### On-Screen Keyboard
- ✅ Full QWERTY layout
- ✅ Alphanumeric characters (a-z, A-Z, 0-9)
- ✅ Special characters (!@#$%^&*()_+-=[]{}|;':",./<>?)
- ✅ Shift key (uppercase/symbols)
- ✅ Backspace key
- ✅ Space bar
- ✅ Save button
- ✅ Word-wrapped text display

#### Safety Features
- ✅ **Two-step safeguard**: Select macro → Confirm with Send button
- ✅ Dynamic Send button positioning (opposite to selected macro)
- ✅ 5-second automatic timeout
- ✅ Cancel on touch elsewhere
- ✅ Error handling for disconnected Bluetooth

#### Bluetooth HID
- ✅ Standard HID keyboard profile
- ✅ Device name: "ESP32 MacroPad"
- ✅ Compatible with Windows, Mac, Linux, Android, iOS
- ✅ Graceful connection/disconnection handling
- ✅ Automatic reconnection
- ✅ Character-by-character transmission

#### Data Persistence
- ✅ NVS (Non-Volatile Storage) for macros
- ✅ Survives power cycles
- ✅ Survives firmware updates (optional erase)
- ✅ Up to 500+ characters per macro
- ✅ Namespace: "macropad"

---

## 🚀 Getting Started Paths

### Path 1: Absolute Beginner (Never used Arduino/ESP32)
**Time**: 30-45 minutes

1. Read [QUICK_START.md](QUICK_START.md) completely
2. Install Arduino IDE and libraries (steps 1-3)
3. Configure TFT_eSPI library (step 4)
4. Wire hardware carefully (step 5)
5. Upload firmware (step 6)
6. Test and pair (steps 7-8)
7. If issues: Check troubleshooting in QUICK_START.md

### Path 2: Experienced Maker (Familiar with Arduino/ESP32)
**Time**: 15-20 minutes

1. Clone repository
2. Install libraries: `TFT_eSPI`, `ESP32 BLE Keyboard`
3. Edit `TFT_eSPI/User_Setup.h` (or copy from `User_Setup_Example.h`)
4. Wire ESP32 to ILI9341 (see pin table)
5. Upload `keybot.ino`
6. Pair Bluetooth and test

### Path 3: PlatformIO User
**Time**: 10-15 minutes

1. Clone repository
2. Open in PlatformIO
3. Let PlatformIO install dependencies
4. Edit `TFT_eSPI/User_Setup.h` after first build
5. Wire hardware
6. Upload and test

### Path 4: Developer/Contributor
**Time**: Variable

1. Clone repository
2. Read [keybot.ino](keybot.ino) (well-commented)
3. Review architecture in [README.md](README.md)
4. Check [TESTING.md](TESTING.md) for test strategy
5. Review [CHANGELOG.md](CHANGELOG.md) for version info
6. Make improvements and submit PR

---

## 🏗️ Technical Architecture

### Hardware Layer
```
┌─────────────────────────────────────────┐
│   ILI9341 Display (320x240)             │
│   - Graphics rendering                  │
│   - XPT2046 touch input                 │
└──────────────┬──────────────────────────┘
               │ SPI (40MHz)
┌──────────────┴──────────────────────────┐
│   ESP32-WROOM-32                        │
│   - Dual-core 240MHz                    │
│   - 520KB RAM                           │
│   - 4MB Flash (with NVS)                │
│   - Bluetooth Classic/BLE               │
└──────────────┬──────────────────────────┘
               │ Bluetooth HID
┌──────────────┴──────────────────────────┐
│   Connected Device                      │
│   (Computer, Phone, Tablet)             │
└─────────────────────────────────────────┘
```

### Software Architecture
```
┌─────────────────────────────────────────┐
│   Application Layer                     │
│   - Mode management                     │
│   - UI state machine                    │
│   - Macro execution logic               │
└──────────┬──────────────┬───────────────┘
           │              │
┌──────────┴───┐   ┌──────┴──────────────┐
│   Display    │   │   Bluetooth         │
│   - TFT_eSPI │   │   - BleKeyboard     │
│   - Touch    │   │   - HID Profile     │
└──────────────┘   └─────────────────────┘
           │
┌──────────┴──────────────────────────────┐
│   Storage Layer                         │
│   - Preferences (NVS)                   │
│   - Macro persistence                   │
└─────────────────────────────────────────┘
```

### State Machine
```
┌──────────────┐
│  MODE_       │ ◄──┐
│  PLAYBACK    │    │
└───┬──────────┘    │
    │ [Settings]    │ [Back]
    │               │
┌───▼──────────┐    │
│  MODE_       │    │
│  CONFIG      │ ───┘
└───┬──────────┘
    │ [Edit Macro]
    │
┌───▼──────────┐
│  MODE_EDIT_  │
│  KEYBOARD    │
└──────────────┘
```

### Data Flow
```
User Touch → Touch Handler → Mode Router
                                   │
        ┌──────────────────────────┼──────────────────────┐
        │                          │                      │
   [Playback]                 [Config]              [Keyboard]
        │                          │                      │
   Select Macro              Select to Edit         Type Characters
        │                          │                      │
   Confirm Send                    │                 Press Save
        │                          │                      │
   Send via BT                     └─────► Save to NVS ◄──┘
        │
   Text appears
   on device
```

---

## 📊 Development Status

### Current Version: 1.0.0
**Status**: ✅ Production Ready

### Completed Features
- [x] Core firmware (1,071 lines)
- [x] Playback mode with 4 macros
- [x] Two-step safeguard
- [x] Configuration mode
- [x] On-screen keyboard
- [x] Bluetooth HID
- [x] Persistent storage
- [x] Comprehensive documentation
- [x] Testing strategy
- [x] Hardware guide
- [x] Example macros

### Tested & Verified
- [x] Display rendering
- [x] Touch input
- [x] Bluetooth pairing
- [x] Macro sending
- [x] Configuration saving
- [x] Power cycling
- [x] Connection recovery

### Known Limitations
- Only 4 macros (by design for simplicity)
- No hotkey combinations (Ctrl+C, Alt+Tab, etc.)
- No macro recording (must type in)
- No WiFi features (Bluetooth only)
- Resistive touch (less responsive than capacitive)

### Future Enhancements (Planned)
See [CHANGELOG.md](CHANGELOG.md) for complete roadmap:
- Multiple macro pages (16+ macros)
- WiFi configuration portal
- Web-based configuration
- Macro recording mode
- Hotkey support
- Custom button labels/colors
- OTA firmware updates
- Battery level indicator

---

## 🤝 Contributing

We welcome contributions! Here's how to help:

### Report Issues
1. Check existing issues first
2. Provide detailed description
3. Include hardware/software versions
4. Share Serial Monitor output
5. Photos/videos help!

### Submit Code
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Update documentation
6. Submit pull request

### Improve Documentation
- Fix typos
- Add examples
- Clarify instructions
- Translate to other languages
- Share your build photos

### Share Your Macros
Add your favorite macros to [EXAMPLES.md](EXAMPLES.md)!

### Testing
Help test on different:
- ESP32 board variants
- Display modules
- Operating systems
- Bluetooth devices

---

## 📚 Documentation Quality

All documentation files are:
- ✅ Comprehensive and detailed
- ✅ Well-organized with ToC
- ✅ Beginner-friendly
- ✅ Technically accurate
- ✅ Regularly updated
- ✅ Cross-referenced
- ✅ Searchable

**Total Documentation**: ~80 KB of high-quality content

---

## 🎓 Educational Value

This project is excellent for learning:
- ESP32 programming
- SPI display interfacing
- Touch input handling
- Bluetooth HID protocols
- State machine design
- UI/UX principles
- Non-volatile storage
- Embedded systems development

**Skill Level**: Beginner to Intermediate

---

## 📜 License

MIT License - see [LICENSE](LICENSE) file

Free to use, modify, and distribute. Attribution appreciated!

---

## 🌟 Project Highlights

### Code Quality
- ✅ 1,071 lines of well-structured C++
- ✅ 36 logical functions
- ✅ 19 documentation blocks
- ✅ Comprehensive inline comments
- ✅ Modular design
- ✅ Non-blocking main loop
- ✅ Error handling

### Documentation Quality
- ✅ 9 documentation files
- ✅ ~80 KB of content
- ✅ Multiple skill levels covered
- ✅ Visual diagrams
- ✅ Step-by-step instructions
- ✅ Troubleshooting guides
- ✅ Example use cases

### Feature Completeness
- ✅ All requirements met
- ✅ Safety mechanisms implemented
- ✅ Persistent storage working
- ✅ Bluetooth stable
- ✅ UI responsive
- ✅ Edge cases handled

---

## 📞 Support

Need help?

1. **Check documentation**:
   - [QUICK_START.md](QUICK_START.md) for setup issues
   - [README.md](README.md) for general questions
   - [HARDWARE.md](HARDWARE.md) for hardware issues
   - [TESTING.md](TESTING.md) for validation

2. **Search existing issues**: Someone may have had same problem

3. **Check Serial Monitor**: Debug output at 115200 baud

4. **Open a GitHub issue**: Provide details and logs

5. **Community forums**:
   - ESP32 Forum
   - Arduino Forum
   - Reddit r/esp32

---

## 🏆 Credits

### Libraries Used
- **TFT_eSPI** by Bodmer - Display & touch
- **ESP32 BLE Keyboard** by T-vK - Bluetooth HID
- **Preferences** - ESP32 Arduino Core

### Inspiration
Built to solve the common problem of repetitive typing and
provide a customizable, portable macro solution.

### Community
Thank you to all contributors, testers, and users!

---

## 🎉 Start Building!

Ready to build your own ESP32 MacroPad?

👉 **New to ESP32?** Start with [QUICK_START.md](QUICK_START.md)

👉 **Want to understand the hardware?** Read [HARDWARE.md](HARDWARE.md)

👉 **Need macro ideas?** Browse [EXAMPLES.md](EXAMPLES.md)

👉 **Ready to code?** Dive into [keybot.ino](keybot.ino)

**Happy Making!** 🚀

---

*Last Updated: October 15, 2025*  
*Project Version: 1.0.0*  
*Status: Production Ready*
