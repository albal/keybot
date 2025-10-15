# Changelog

All notable changes to the ESP32 MacroPad project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
