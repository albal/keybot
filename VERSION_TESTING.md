# Version Information Testing Guide

This document describes how to test and verify the version information feature.

## Overview

The firmware now includes automatic version information that:
- Shows the git tag (e.g., `v0.1.2`) when building from a tagged release
- Shows the short commit SHA (e.g., `abc1234`) when no tag is available
- Displays this information in the serial logs at startup
- Will display it on the screen once the UI is fully implemented

## How It Works

### Build-Time Generation

1. During the CMake configuration phase, the build system runs `git describe --tags --always --dirty`
2. This generates a version string based on the current git state:
   - Tagged release: `v0.1.2`
   - Commits after tag: `v0.1.2-3-gabc1234` (3 commits after v0.1.2, commit abc1234)
   - Uncommitted changes: Appends `-dirty`
   - No git or no tags: Falls back to `git rev-parse --short HEAD` or "unknown"
3. The version string is written to `build/generated/version.h`
4. This header is included in `main.c` and provides the `KEYBOT_VERSION` macro

### Runtime Display

1. At startup, the firmware logs the version to the serial console
2. The version is also referenced in the display drawing code for future UI implementation

## Testing Procedures

### Test 1: Build from Tagged Release

**Prerequisites:**
- ESP-IDF v5.0 or later installed
- Git repository with tags

**Steps:**
1. Checkout a tagged version:
   ```bash
   git checkout v0.1.2
   ```

2. Build the firmware:
   ```bash
   idf.py build
   ```

3. Check the build output for version information:
   ```bash
   grep "Keybot version:" build/CMakeCache.txt
   ```

4. Verify the generated header:
   ```bash
   cat build/generated/version.h
   ```
   Expected content:
   ```c
   #define KEYBOT_VERSION "v0.1.2"
   ```

5. Flash and monitor:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

6. Verify the log output shows:
   ```
   I (xxx) MACROPAD: ESP32 MacroPad Starting...
   I (xxx) MACROPAD: Firmware Version: v0.1.2
   I (xxx) MACROPAD: ESP-IDF Version: v5.x
   ```

**Expected Result:** Version shows the tag name `v0.1.2`

---

### Test 2: Build from Development Branch

**Prerequisites:**
- ESP-IDF v5.0 or later installed
- Git repository on a development branch

**Steps:**
1. Make sure you're on a development branch with commits after the last tag
2. Build the firmware:
   ```bash
   idf.py build
   ```

3. Check the generated version:
   ```bash
   cat build/generated/version.h
   ```
   Expected content (example):
   ```c
   #define KEYBOT_VERSION "v0.1.2-3-gabc1234"
   ```
   This means 3 commits after v0.1.2, short SHA is abc1234

4. Flash and monitor:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

5. Verify the log shows the full version string with commit count and SHA

**Expected Result:** Version shows tag + commit count + short SHA (e.g., `v0.1.2-3-gabc1234`)

---

### Test 3: Build with Uncommitted Changes

**Prerequisites:**
- ESP-IDF v5.0 or later installed
- Git repository with local modifications

**Steps:**
1. Make a change to any file (e.g., add a comment to main.c)
2. Do NOT commit the change
3. Build the firmware:
   ```bash
   idf.py build
   ```

4. Check the generated version:
   ```bash
   cat build/generated/version.h
   ```
   Expected to see `-dirty` suffix:
   ```c
   #define KEYBOT_VERSION "v0.1.2-3-gabc1234-dirty"
   ```

5. Flash and monitor to verify the log shows the `-dirty` suffix

**Expected Result:** Version includes `-dirty` suffix to indicate uncommitted changes

---

### Test 4: GitHub Actions Build

**Prerequisites:**
- Push to main branch or create a tag

**Steps:**
1. View the GitHub Actions workflow run
2. Check the "Build ESP32 firmware" step output
3. Look for the CMake status message:
   ```
   -- Keybot version: v0.1.2
   ```

4. Download the firmware artifacts from the workflow run
5. The version should be embedded in the firmware

**Expected Result:** GitHub Actions successfully detects and uses version information

---

### Test 5: Build from Release Tag

**Prerequisites:**
- ESP-IDF v5.0 or later installed
- Repository with a release tag pushed to GitHub

**Steps:**
1. Create and push a new tag:
   ```bash
   git tag v0.2.0
   git push origin v0.2.0
   ```

2. GitHub Actions should automatically create a release
3. Download the firmware from the release page
4. Flash the firmware:
   ```bash
   esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
     write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB \
     0x0 keybot_merged.bin
   ```

5. Monitor the serial output:
   ```bash
   idf.py -p /dev/ttyUSB0 monitor
   ```

6. Verify the version matches the tag (v0.2.0)

**Expected Result:** Released firmware shows the exact tag version

---

### Test 6: Build Without Git

**Prerequisites:**
- ESP-IDF v5.0 or later installed
- Source code without git repository (e.g., from a zip download)

**Steps:**
1. Remove the .git directory or download source as zip
2. Build the firmware:
   ```bash
   idf.py build
   ```

3. Check the generated version:
   ```bash
   cat build/generated/version.h
   ```
   Expected content:
   ```c
   #define KEYBOT_VERSION "unknown"
   ```

4. Verify the firmware still builds and runs correctly

**Expected Result:** Version falls back to "unknown" when git is not available

---

## Display Verification (Future)

Once the UI is fully implemented with text rendering:

1. Flash the firmware with version information
2. Power on the device
3. Look at the main screen
4. Verify the version is displayed (typically at the bottom or top corner)
5. The version should match what's shown in the serial logs

**Expected Result:** Version is visible on the display and matches the log output

---

## Troubleshooting

### Version shows as "unknown"

**Possible causes:**
- Git is not installed or not in PATH
- Not running from a git repository
- .git directory is missing

**Solution:**
- Ensure git is installed and accessible
- Clone the repository instead of downloading as zip
- Verify .git directory exists

### Version doesn't include tag

**Possible causes:**
- No tags exist in the repository
- Tags weren't fetched (shallow clone)
- Building from a commit before any tags

**Solution:**
- Create and push tags following semantic versioning
- Fetch all history: `git fetch --unshallow` or `git fetch --tags`
- Ensure GitHub Actions uses `fetch-depth: 0` in checkout

### Version not updating after changes

**Possible causes:**
- CMake cache not regenerated
- Build directory not cleaned

**Solution:**
- Clean and rebuild:
  ```bash
  idf.py fullclean
  idf.py build
  ```

---

## Integration with CI/CD

The version system automatically integrates with GitHub Actions:

1. **Fetch-depth: 0** in checkout ensures full git history is available
2. CMake runs `git describe` during configuration
3. Version is embedded in the binary at build time
4. Release binaries automatically include the correct version
5. Pre-release builds show the commit SHA for tracking

This ensures every build can be traced back to its exact source code state.
