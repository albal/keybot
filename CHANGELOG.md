# Changelog

All notable changes to the ESP32 MacroPad project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **Touchscreen calibration feature** for accurate touch input
  - Two-point calibration system (top-left and bottom-right corners)
  - Calibration data stored in NVS flash for persistence
  - Automatic calibration prompt on first boot (after display test)
  - Manual calibration mode accessible via 10-second press (new)
  - Calibration applies axis swapping for landscape mode orientation
  - Visual crosshair targets and progress indicators during calibration
  - Success confirmation message after calibration completion
- **Bluetooth pairing button** in Bluetooth Configuration Mode
  - Blue "PAIR" button added to initiate Bluetooth pairing
  - Shows "Pairing mode active..." message when pressed
  - Located above the CLEAR FLASH button for better visibility

### Changed
- **Updated touch press duration thresholds** for better usability
  - 5 seconds: Enter configuration mode (unchanged)
  - **10 seconds: Enter calibration mode** (NEW - was not available)
  - **20 seconds: Enter Bluetooth config mode** (changed from 10 seconds)
- Touch coordinate mapping now uses calibration data when available
  - Falls back to default mapping if no calibration data exists
  - Properly handles axis swapping (raw_y → screen_x, raw_x → screen_y)
- Clear flash operation in BT config now also resets calibration data
- Button layout in Bluetooth Configuration Mode adjusted for new PAIR button
  - PAIR button positioned at y=150
  - CLEAR FLASH button repositioned to y=190
  - Both buttons standardized to 120x35 pixel size

### Fixed
- **Y-axis touchscreen orientation** - removed incorrect Y-axis inversion
  - Touch Y-axis now maps directly without inversion: `screen_y = map_touch_y(raw_x)`
  - Fixes upside-down Y-axis behavior reported in issue
- Fixed display rotation - display now rotates 90 degrees clockwise to correct orientation
  - Changed ILI9341 MADCTL register from 0x48 to 0x28
  - Updated display initialization logs to reflect new orientation
- Implemented touch screen coordinate reading from XPT2046 controller
  - Added xpt2046_read() function for raw SPI communication with touch controller
  - Added read_touch_coordinates() function to read X/Y positions with averaging
  - Updated touch handling task to actively read and log coordinates
  - Added touch press/release detection
  - Reduced touch polling to 20Hz to avoid log spam
- Version information system that displays firmware version in logs and on display
  - Shows git tag version when built from a tagged release (e.g., v0.1.2)
  - Shows short commit SHA when no tag is available (e.g., abc1234)
  - Auto-generated at build time from git repository information
  - Logged at firmware startup for debugging and support
  - Displayed on main screen (when UI is fully implemented)
- Touch coordinate reading functionality
  - Reads 12-bit raw values from XPT2046 (0-4095)
  - Averages two readings to reduce noise
  - Logs touch events with coordinates for debugging
  - Pressure threshold detection (Z1 > 100)
- Comprehensive documentation for display rotation and touch fixes (DISPLAY_ROTATION_FIX.md)
  - Technical details about MADCTL register and XPT2046 commands
  - Testing and troubleshooting guides
  - Future calibration implementation notes

## [1.0.0] - 2025-10-15

### Added
- Initial ESP-IDF framework implementation of ESP32 MacroPad
- Native ESP-IDF support (v5.0+) for better performance
- CMake build system configuration
- Main playback mode framework with 4 configurable macro buttons
- Framework for two-step safeguard mechanism (select + confirm)
- Framework for configuration mode with on-screen QWERTY keyboard
- NVS Flash API integration for persistent macro storage
- FreeRTOS tasks for UI and touch handling
- SPI bus initialization for ILI9341 and XPT2046 controllers
- GPIO configuration with backlight control
- Framework for Bluetooth HID using Bluedroid stack
- ESP_LOG integration for debugging and monitoring
- Comprehensive inline code documentation
- Complete pinout diagram in code comments
- Detailed README with ESP-IDF setup instructions
- Extensive testing strategy documentation (TESTING.md)
- CMakeLists.txt for ESP-IDF build system
- Hardware wiring guide

### Features (Framework)
- Device name: "ESP32 MacroPad"
- Support for macros up to 512 characters (MAX_MACRO_LEN)
- NVS namespace "macropad" for data persistence
- State machine for mode management (Playback/Config/Edit)
- Task-based architecture for responsive operation
- Configurable pin assignments via defines

### Technical
- Written in C for ESP-IDF framework
- Compatible with ESP32-WROOM-32 and similar ESP32 modules
- FreeRTOS-based task scheduling
- SPI Master driver for display communication
- NVS Flash API for persistent storage
- Modular code structure with logical function organization
- Configurable pin assignments
- ESP-IDF v5.0+ required

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

### Fixed
- Implemented ILI9341 display initialization sequence to fix blank white screen issue
- Added comprehensive console logging for display initialization steps (40+ log messages)
- Display now properly initializes with hardware/software reset, power control, and configuration
- Display turns on in landscape mode with RGB565 color format after initialization

### Added
- Complete ILI9341 command definitions for display control
- SPI device configuration for display controller
- Hardware reset sequence with proper timing delays
- Display configuration commands: power control, VCOM, gamma correction, pixel format
- Detailed console feedback showing initialization progress and status
- Reference to LCD Wiki documentation in code comments

### Changed
- Enhanced display_init() function from stub to full implementation
- Updated documentation to reflect implemented vs. planned features
- Added implementation status section in main.c showing completed and TODO items

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
