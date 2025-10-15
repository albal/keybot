# Contributing to ESP32 MacroPad

First off, thank you for considering contributing to ESP32 MacroPad! It's people like you that make this project better for everyone.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [How Can I Contribute?](#how-can-i-contribute)
3. [Development Setup](#development-setup)
4. [Coding Standards](#coding-standards)
5. [Commit Guidelines](#commit-guidelines)
6. [Pull Request Process](#pull-request-process)
7. [Testing Guidelines](#testing-guidelines)
8. [Documentation](#documentation)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inspiring community for all. Please be respectful and considerate of others.

### Our Standards

**Positive behaviors**:
- Using welcoming and inclusive language
- Being respectful of differing viewpoints
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

**Unacceptable behaviors**:
- Trolling, insulting/derogatory comments, and personal attacks
- Public or private harassment
- Publishing others' private information without permission
- Other conduct which could reasonably be considered inappropriate

---

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates.

**When reporting a bug, include**:
- Clear, descriptive title
- Detailed steps to reproduce
- Expected behavior vs. actual behavior
- Hardware specifications (ESP32 board, display model)
- Firmware version
- Serial Monitor output
- Photos/videos if relevant

**Bug Report Template**:
```markdown
**Environment:**
- ESP32 Board: [e.g., ESP32-WROOM-32]
- Display: [e.g., 2.8" ILI9341]
- Arduino IDE Version: [e.g., 1.8.19]
- Library Versions: [TFT_eSPI x.x.x, BleKeyboard x.x.x]
- OS: [e.g., Windows 10]

**Description:**
[Clear description of the bug]

**Steps to Reproduce:**
1. 
2. 
3. 

**Expected Behavior:**
[What you expected to happen]

**Actual Behavior:**
[What actually happened]

**Serial Monitor Output:**
```
[Paste relevant output]
```

**Additional Context:**
[Any other relevant information]
```

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues.

**When suggesting enhancements**:
- Use a clear, descriptive title
- Provide detailed description of the proposed feature
- Explain why this would be useful
- Consider implementation challenges
- Include mockups/diagrams if relevant

### Contributing Code

We love code contributions! Here are the types of contributions we're looking for:

**High Priority**:
- Bug fixes
- Performance improvements
- Security enhancements
- Documentation improvements
- Test coverage

**Medium Priority**:
- New features from roadmap (see CHANGELOG.md)
- UI/UX improvements
- Code refactoring
- Additional examples

**Low Priority (but still welcome)**:
- Advanced features
- Experimental functionality
- Alternative implementations

---

## Development Setup

### Prerequisites

1. **ESP-IDF v5.0 or later** installed and configured
2. **Python 3.8+** with required packages
3. **CMake 3.16+** for building
4. **Git** for version control
5. **Serial terminal** for monitoring (built into ESP-IDF)

### ESP-IDF Installation

Follow the official guide: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

Quick setup:
```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32
. ./export.sh
```

### Setting Up Development Environment

1. **Fork the repository**:
   ```bash
   # On GitHub, click "Fork" button
   ```

2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/keybot.git
   cd keybot
   ```

3. **Add upstream remote**:
   ```bash
   git remote add upstream https://github.com/albal/keybot.git
   ```

4. **Create a branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

5. **Set up ESP-IDF environment**:
   ```bash
   . ~/esp/esp-idf/export.sh
   ```

6. **Build the project**:
   ```bash
   cd keybot
   idf.py build
   ```

7. **Make your changes**:
   - Edit `main/main.c` or other source files
   - Update CMakeLists.txt if adding new files
   - Test thoroughly

8. **Commit and push**:
   ```bash
   git add .
   git commit -m "Description of changes"
   git push origin feature/your-feature-name
   ```

8. **Create Pull Request**:
   - Go to your fork on GitHub
   - Click "New Pull Request"
   - Fill in the template

---

## Coding Standards

### C++ Style Guide

**General Rules**:
- Use 2 spaces for indentation (not tabs)
- Maximum line length: 100 characters
- Use meaningful variable and function names
- Add comments for complex logic
- Follow existing code style

**Naming Conventions**:
```cpp
// Constants: UPPER_CASE
#define SCREEN_WIDTH 320
const int MAX_MACROS = 4;

// Variables: camelCase
int selectedMacro = -1;
bool sendButtonVisible = false;

// Functions: camelCase
void drawMainScreen() { }
void handleTouch() { }

// Classes: PascalCase (if adding any)
class MacroManager { };

// Enums: PascalCase
enum Mode {
  MODE_PLAYBACK,
  MODE_CONFIG
};
```

**Code Organization**:
```cpp
// 1. Header comments
// 2. Includes
// 3. Defines
// 4. Global variables
// 5. Function declarations
// 6. setup()
// 7. loop()
// 8. Function implementations
```

**Comments**:
```cpp
// Use single-line comments for brief explanations
int x = 0;  // Inline comments when needed

/**
 * Use multi-line comments for function documentation
 * 
 * @param macroIndex Index of macro (0-3)
 * @param text Text string to save
 */
void saveMacro(int macroIndex, String text) {
  // Implementation
}
```

**Error Handling**:
```cpp
// Always check bounds
if (macroIndex < 0 || macroIndex > 3) {
  Serial.println("Invalid macro index");
  return;
}

// Check Bluetooth connection
if (!bleKeyboard.isConnected()) {
  Serial.println("Bluetooth not connected");
  return;
}
```

### Arduino-Specific Guidelines

**Setup vs Loop**:
```cpp
// setup(): Run once at startup
void setup() {
  // Initialize hardware
  // Load configuration
  // Draw initial screen
}

// loop(): Run continuously, keep it non-blocking
void loop() {
  // Check inputs
  // Update state
  // Process events
  // No long delays!
}
```

**Avoid**:
```cpp
// Don't use long delays in loop()
delay(5000);  // BAD

// Instead use millis()
if (millis() - lastTime > 5000) {  // GOOD
  // Do something
}

// Don't use blocking reads
while (!Serial.available());  // BAD

// Instead check and return
if (Serial.available()) {  // GOOD
  // Read data
}
```

---

## Commit Guidelines

### Commit Message Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, no code change)
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Maintenance tasks

**Examples**:
```
feat(keyboard): Add support for function keys

Implement F1-F12 function keys in on-screen keyboard.
Keys are accessible via Shift+number row.

Closes #42
```

```
fix(bluetooth): Handle disconnection gracefully

Previously crashed when Bluetooth disconnected during send.
Now checks connection status before sending.

Fixes #38
```

```
docs(readme): Add troubleshooting section

Added common issues and solutions for:
- Display not working
- Touch not responsive
- Bluetooth pairing issues
```

---

## Pull Request Process

### Before Submitting

1. **Test thoroughly**:
   - Build successfully
   - Test all affected functionality
   - Test on actual hardware if possible
   - Check Serial Monitor for errors

2. **Update documentation**:
   - Update README.md if needed
   - Update CHANGELOG.md
   - Add comments to complex code
   - Update EXAMPLES.md if adding examples

3. **Follow coding standards**:
   - Consistent indentation
   - Meaningful names
   - Proper comments
   - Error handling

4. **Keep changes focused**:
   - One feature/fix per PR
   - Don't mix unrelated changes
   - Keep diff as small as possible

### PR Template

When creating a PR, include:

```markdown
## Description
[Clear description of what this PR does]

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Related Issue
Fixes #[issue number]
Relates to #[issue number]

## Testing
- [ ] Tested on hardware
- [ ] All tests pass
- [ ] No new warnings

**Test environment:**
- ESP32 Board: 
- Display: 
- Arduino IDE Version: 

## Screenshots
[If UI changes, include before/after screenshots]

## Checklist
- [ ] My code follows the project's style guidelines
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have updated the documentation accordingly
- [ ] My changes generate no new warnings
- [ ] I have tested this on actual hardware
```

### Review Process

1. **Automated checks** (if configured):
   - Compilation check
   - Code style check

2. **Manual review**:
   - Code quality
   - Testing adequacy
   - Documentation completeness
   - Impact on existing features

3. **Approval**:
   - At least one maintainer approval required
   - All discussions resolved

4. **Merge**:
   - Maintainer will merge
   - Usually squash merge for clean history

---

## Testing Guidelines

### Manual Testing

Before submitting, test these scenarios:

**Display & Touch**:
- [ ] Display renders correctly
- [ ] Touch is responsive
- [ ] All UI elements visible
- [ ] Text is readable

**Bluetooth**:
- [ ] Device is discoverable
- [ ] Pairing works
- [ ] Connection stable
- [ ] Reconnection works
- [ ] Disconnection handled

**Macro Functions**:
- [ ] All 4 macros work
- [ ] Text sends correctly
- [ ] Special characters work
- [ ] Long macros work
- [ ] Empty macros handled

**Configuration**:
- [ ] Can enter config mode
- [ ] Can edit macros
- [ ] Keyboard works
- [ ] Save persists
- [ ] Can return to main screen

**Persistence**:
- [ ] Macros survive power cycle
- [ ] Settings retained

### Unit Testing (Future)

When adding unit tests:
```cpp
// Test function naming: test_<function>_<scenario>
void test_saveMacro_validIndex() {
  String testString = "Test";
  saveMacro(0, testString);
  String loaded = loadMacro(0);
  assert(testString == loaded);
}

void test_saveMacro_invalidIndex() {
  String testString = "Test";
  saveMacro(-1, testString);
  // Should handle gracefully
}
```

### Test Hardware Configurations

If possible, test on:
- Different ESP32 boards (DevKit, WROOM, WROVER)
- Different display sizes (2.4", 2.8", 3.2")
- Different computers (Windows, Mac, Linux)
- Different Bluetooth devices (phones, tablets)

---

## Documentation

### Documentation Standards

**All documentation should be**:
- Clear and concise
- Well-organized
- Beginner-friendly
- Technically accurate
- Up-to-date

### Updating Documentation

When making changes:

1. **Code changes** → Update inline comments
2. **New features** → Update README.md
3. **Bug fixes** → Update CHANGELOG.md
4. **API changes** → Update function documentation
5. **New examples** → Add to EXAMPLES.md
6. **Hardware changes** → Update HARDWARE.md

### Documentation Files

| File | Purpose | When to Update |
|------|---------|----------------|
| README.md | Main documentation | New features, major changes |
| CHANGELOG.md | Version history | Every release |
| QUICK_START.md | Getting started | Setup process changes |
| HARDWARE.md | Assembly guide | Hardware changes |
| TESTING.md | Test strategy | New test cases |
| EXAMPLES.md | Example macros | New examples |
| CONTRIBUTING.md | This file | Contribution process changes |

---

## Feature Requests

### Proposing New Features

1. **Check roadmap**: See CHANGELOG.md for planned features
2. **Search issues**: May already be discussed
3. **Create issue**: Use "Feature Request" template
4. **Discuss**: Engage with maintainers and community
5. **Implement**: Fork and create PR

### Feature Request Template

```markdown
## Feature Description
[Clear description of the feature]

## Use Case
[Why is this useful? Who would use it?]

## Proposed Implementation
[How could this be implemented?]

## Alternatives Considered
[What other approaches did you consider?]

## Additional Context
[Mockups, diagrams, related features]
```

---

## Community

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests
- **Pull Requests**: Code contributions, reviews
- **Discussions**: General questions, ideas
- **ESP32 Forum**: Hardware/platform questions
- **Arduino Forum**: Arduino-specific questions

### Getting Help

**For development questions**:
1. Check existing documentation
2. Search closed issues
3. Ask in GitHub Discussions
4. Reach out to maintainers

**For usage questions**:
1. Check README.md and QUICK_START.md
2. Review HARDWARE.md for assembly
3. Check TESTING.md for diagnostics
4. Open an issue if still stuck

---

## Recognition

Contributors will be:
- Listed in commit history
- Mentioned in release notes
- Added to Contributors section (if significant contribution)
- Appreciated and thanked! 🎉

---

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

## Questions?

Don't hesitate to ask! We're here to help.

- Open an issue labeled "question"
- Comment on related issues
- Reach out to maintainers

Thank you for contributing to ESP32 MacroPad! 🚀

---

*Last Updated: October 15, 2025*
