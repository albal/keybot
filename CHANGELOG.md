# Changelog

All notable changes to the ESP32 MacroPad project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-10-15

### Added
- Initial release of ESP32 MacroPad firmware
- Main playback mode with 4 configurable macro buttons
- Two-step safeguard mechanism (select + confirm) to prevent accidental macro execution
- Configuration mode with on-screen QWERTY keyboard
- Full alphanumeric and special character support
- Shift key for uppercase letters and alternate symbols
- Backspace and Space keys
- Persistent storage using ESP32 NVS (Preferences library)
- Bluetooth HID keyboard functionality via BleKeyboard library
- Real-time Bluetooth connection status indicator
- Visual feedback for button presses and selections
- 5-second automatic timeout for macro selection
- Touch-based UI using ILI9341 320x240 TFT display
- XPT2046 touch controller support
- Serial debug output for troubleshooting
- Comprehensive inline code documentation
- Complete pinout diagram in code comments
- Detailed README with setup instructions
- Extensive testing strategy documentation (TESTING.md)
- PlatformIO configuration (platformio.ini)
- Library dependencies management (library.json)
- TFT_eSPI configuration example (User_Setup_Example.h)
- Hardware wiring guide

### Features
- Device advertises as "ESP32 MacroPad"
- Support for macros up to 500+ characters
- Graceful handling of Bluetooth connection/disconnection
- Automatic reconnection capability
- Non-blocking main loop for responsive UI
- Word-wrapped text display in edit mode
- Dynamic Send button positioning (opposite to selected macro)
- Color-coded UI elements for better user experience

### Technical
- Written in C++ using Arduino framework
- Compatible with ESP32-WROOM-32 and similar ESP32 modules
- SPI-based display communication for fast refresh rates
- Debounced touch input to prevent double-presses
- Modular code structure with logical function organization
- Configurable pin assignments
- Customizable colors and layout

### Documentation
- Getting Started guide
- Pin connection diagram
- Library installation instructions
- Configuration instructions for TFT_eSPI
- Usage guide with step-by-step instructions
- Troubleshooting section
- Manual and unit test examples
- Edge case and stress test scenarios

## [Unreleased]

### Planned Features
- Touch calibration UI for easier setup
- Multiple macro pages (4x4 = 16 total macros)
- WiFi configuration portal
- Web-based configuration interface
- Macro recording mode (learn from actual keystrokes)
- Macro playback speed adjustment
- Hotkey combinations support (Ctrl+C, Alt+Tab, etc.)
- Macro categories/folders
- Import/export macros via file
- Customizable button labels
- Custom button colors per macro
- Sleep mode to save power
- Battery level indicator (for battery-powered builds)
- Over-the-air (OTA) firmware updates

### Potential Improvements
- Touch calibration helper on first boot
- Visual theme customization
- Multi-language keyboard layouts
- Macro variables (date, time, clipboard, etc.)
- Conditional macros (if/then logic)
- Macro chaining (execute multiple macros in sequence)
- Delay/wait commands in macros
- Mouse movement/click support (HID Mouse)
- Profiles for different computers/applications

## Version History

### Version Numbering
- **Major version** (X.0.0): Significant changes, potential breaking changes
- **Minor version** (0.X.0): New features, backward compatible
- **Patch version** (0.0.X): Bug fixes, minor improvements

[1.0.0]: https://github.com/albal/keybot/releases/tag/v1.0.0
