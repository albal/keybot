/*
 * ESP32 MacroPad - Bluetooth HID Macro Keyboard Firmware
 * 
 * Author: Generated for ESP32-WROOM-32 platform
 * License: MIT
 * 
 * Description:
 * Complete firmware for a custom Bluetooth HID macro keyboard using ESP32-WROOM-32
 * and ILI9341 TFT touchscreen display. Features 4 configurable macro buttons with
 * on-screen keyboard for configuration and persistent storage.
 * 
 * =============================================================================
 * GETTING STARTED
 * =============================================================================
 * 
 * Required Libraries (install via Arduino Library Manager):
 * 1. TFT_eSPI by Bodmer (for ILI9341 display and XPT2046 touch controller)
 *    - Install: Arduino IDE -> Tools -> Manage Libraries -> Search "TFT_eSPI"
 *    - Configure: Edit User_Setup.h in the TFT_eSPI library folder to set:
 *      #define ILI9341_DRIVER
 *      #define TFT_WIDTH  240
 *      #define TFT_HEIGHT 320
 *      Define the correct pins for your hardware (see pinout below)
 * 
 * 2. BleKeyboard by T-vK
 *    - Install: Arduino IDE -> Tools -> Manage Libraries -> Search "BleKeyboard"
 *    - Or download from: https://github.com/T-vK/ESP32-BLE-Keyboard
 * 
 * 3. Preferences (built-in with ESP32 Arduino core - no installation needed)
 * 
 * Board Setup:
 * - Board: "ESP32 Dev Module" or "ESP32 Wrover Module"
 * - Upload Speed: 921600
 * - Flash Frequency: 80MHz
 * - Partition Scheme: "Default 4MB with spiffs"
 * 
 * =============================================================================
 * PINOUT DIAGRAM - ESP32-WROOM-32 to ILI9341 TFT Display
 * =============================================================================
 * 
 * ESP32 Pin    ->    ILI9341/XPT2046 Pin
 * ---------         ------------------
 * GPIO 23      ->    TFT_MOSI (SDI/MOSI)
 * GPIO 19      ->    TFT_MISO (SDO/MISO)
 * GPIO 18      ->    TFT_SCLK (SCK)
 * GPIO 15      ->    TFT_CS (Chip Select for Display)
 * GPIO 2       ->    TFT_DC (Data/Command)
 * GPIO 4       ->    TFT_RST (Reset) - Optional, can use -1 and tie to ESP32 EN
 * GPIO 5       ->    TOUCH_CS (Chip Select for Touch Controller)
 * 
 * Power:
 * 3.3V         ->    VCC
 * GND          ->    GND
 * 
 * Optional:
 * GPIO 21      ->    TFT_LED (Backlight) - Or connect to 3.3V via resistor
 * 
 * Note: These pins are configurable below. Adjust based on your wiring.
 * SPI pins (MOSI, MISO, SCLK) use hardware SPI for better performance.
 * 
 * =============================================================================
 * CODE OVERVIEW
 * =============================================================================
 * 
 * Main Components:
 * 
 * 1. Display Management (TFT_eSPI)
 *    - Handles all display rendering
 *    - Touch input processing
 *    - UI elements drawing (buttons, keyboard, etc.)
 * 
 * 2. Bluetooth HID (BleKeyboard)
 *    - Manages Bluetooth connection
 *    - Sends keystrokes to connected device
 *    - Device name: "ESP32 MacroPad"
 * 
 * 3. Persistent Storage (Preferences)
 *    - Saves macro text strings to NVS (Non-Volatile Storage)
 *    - Keys: "macro1", "macro2", "macro3", "macro4"
 *    - Survives power cycles and firmware updates
 * 
 * Operating Modes:
 * 
 * A. PLAYBACK MODE (Main Screen)
 *    - Displays 4 large macro buttons + settings button
 *    - Two-step safeguard for sending macros:
 *      1. User touches a macro button (button highlights)
 *      2. "Send" button appears on opposite side
 *      3. User must press "Send" within 5 seconds
 *      4. Otherwise, selection is cancelled
 *    - Prevents accidental keystroke transmission
 * 
 * B. CONFIGURATION MODE
 *    - Accessible via "Settings" button
 *    - Shows 4 macro buttons for editing
 *    - Tapping a button opens on-screen keyboard
 *    - On-screen QWERTY keyboard features:
 *      * Full alphanumeric characters
 *      * Special characters (!@#$%^&*()_+-=[]{}etc.)
 *      * Shift, Backspace, and Save keys
 *      * Real-time text preview
 *    - Saves to NVS on "Save" button press
 * 
 * Data Flow:
 * - Startup -> Load macros from Preferences -> Display main screen
 * - Touch detected -> Process based on current mode
 * - Config mode -> Edit macro -> Save to Preferences
 * - Playback mode -> Select macro -> Confirm -> Send via Bluetooth
 * 
 * =============================================================================
 * TESTING STRATEGY
 * =============================================================================
 * 
 * Manual Test Cases:
 * 
 * 1. Display & Touch Test
 *    - Power on device, verify main screen appears
 *    - Touch each of the 4 macro buttons, verify highlight works
 *    - Verify "Send" button appears in opposite quadrant
 *    - Touch elsewhere or wait 5s, verify selection clears
 * 
 * 2. Bluetooth Connection Test
 *    - Pair with computer/phone via Bluetooth settings
 *    - Look for "ESP32 MacroPad" in device list
 *    - Verify successful connection indicator
 *    - Verify graceful reconnection after disconnect
 * 
 * 3. Macro Playback Test
 *    - Set up a text editor on connected device
 *    - Select Macro 1, press Send
 *    - Verify text appears correctly
 *    - Repeat for all 4 macros
 *    - Test with various character types (letters, numbers, symbols)
 * 
 * 4. Configuration Mode Test
 *    - Press Settings button, verify config screen appears
 *    - Select Macro 1 for editing
 *    - Verify on-screen keyboard displays
 *    - Type test string using keyboard
 *    - Test Shift key for uppercase/symbols
 *    - Test Backspace key
 *    - Press Save, verify return to config screen
 *    - Return to main screen, verify new text is saved
 * 
 * 5. Persistence Test
 *    - Configure all 4 macros with unique text
 *    - Power off the ESP32 completely
 *    - Power back on
 *    - Verify all macros retained their configured text
 * 
 * 6. Edge Cases
 *    - Test very long macro strings (>100 characters)
 *    - Test empty macro strings
 *    - Test macros with only special characters
 *    - Test rapid button presses
 *    - Test timeout behavior (5-second selection timeout)
 * 
 * Unit Test Examples (pseudo-code for reference):
 * 
 * TEST(PreferencesStorage, SaveAndLoadMacro) {
 *   String testString = "Test Macro 123!";
 *   saveMacro(0, testString);
 *   String loaded = loadMacro(0);
 *   ASSERT_EQUAL(testString, loaded);
 * }
 * 
 * TEST(TouchInput, ButtonHitDetection) {
 *   // Test if touch coordinates properly detect button press
 *   ASSERT_TRUE(isButtonPressed(50, 50, 0, 0, 100, 100));
 *   ASSERT_FALSE(isButtonPressed(150, 150, 0, 0, 100, 100));
 * }
 * 
 * TEST(MacroSelection, TimeoutBehavior) {
 *   selectMacro(0);
 *   delay(6000); // Wait longer than 5-second timeout
 *   ASSERT_EQUAL(getSelectedMacro(), -1); // Should be cleared
 * }
 * 
 * Debugging Tips:
 * - Use Serial.begin(115200) and Serial.println() for debugging
 * - Monitor Bluetooth connection status via bleKeyboard.isConnected()
 * - Check touch calibration if touch accuracy is poor
 * - Verify SPI connections if display doesn't work
 * 
 */

// =============================================================================
// LIBRARY INCLUDES
// =============================================================================
#include <TFT_eSPI.h>           // Graphics and touch library for ILI9341
#include <SPI.h>                // SPI communication
#include <BleKeyboard.h>        // Bluetooth HID keyboard library
#include <Preferences.h>        // ESP32 NVS storage

// =============================================================================
// PIN DEFINITIONS
// =============================================================================
// Note: Modify these to match your hardware connections
// These should also be configured in TFT_eSPI's User_Setup.h

// Display pins (handled by TFT_eSPI, configured in User_Setup.h)
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4  // -1 if tied to ESP32 reset

// Touch controller pins
#define TOUCH_CS 5

// Optional backlight pin
#define TFT_LED  21  // -1 if not used

// =============================================================================
// DISPLAY CONFIGURATION
// =============================================================================
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// =============================================================================
// COLOR DEFINITIONS
// =============================================================================
#define COLOR_BG          0x0000  // Black background
#define COLOR_BUTTON      0x1082  // Dark blue
#define COLOR_BUTTON_SEL  0xF800  // Red (selected)
#define COLOR_BUTTON_SEND 0x07E0  // Green (send button)
#define COLOR_TEXT        0xFFFF  // White text
#define COLOR_SETTINGS    0x7BEF  // Light gray
#define COLOR_KEYBOARD    0x31A6  // Medium blue
#define COLOR_KEY_PRESSED 0xFD20  // Orange

// =============================================================================
// UI CONSTANTS
// =============================================================================
// Main screen button layout (2x2 grid)
#define MACRO_BTN_WIDTH   140
#define MACRO_BTN_HEIGHT  90
#define MACRO_BTN_MARGIN  10

// Settings button
#define SETTINGS_BTN_SIZE 40

// Send button
#define SEND_BTN_WIDTH    100
#define SEND_BTN_HEIGHT   50

// Keyboard layout
#define KEY_WIDTH         26
#define KEY_HEIGHT        30
#define KEY_SPACING       2
#define KEYBOARD_ROWS     4
#define KEYBOARD_START_Y  120

// =============================================================================
// OPERATING MODES
// =============================================================================
enum Mode {
  MODE_PLAYBACK,      // Main screen - playback mode
  MODE_CONFIG,        // Configuration mode - select macro to edit
  MODE_EDIT_KEYBOARD  // Editing mode - on-screen keyboard active
};

// =============================================================================
// GLOBAL OBJECTS
// =============================================================================
TFT_eSPI tft = TFT_eSPI();                    // Display object
BleKeyboard bleKeyboard("ESP32 MacroPad");    // Bluetooth keyboard
Preferences preferences;                       // NVS storage

// =============================================================================
// STATE VARIABLES
// =============================================================================
Mode currentMode = MODE_PLAYBACK;              // Current operating mode
int selectedMacro = -1;                        // Currently selected macro (0-3, -1 = none)
unsigned long selectionTime = 0;               // Time when macro was selected
bool sendButtonVisible = false;                // Is send button currently shown?
int editingMacro = -1;                         // Which macro is being edited (-1 = none)
String macros[4];                              // Text for each macro button
String editBuffer = "";                        // Buffer for text being edited
bool shiftActive = false;                      // Is shift key currently active?

// Touch state
uint16_t touchX = 0, touchY = 0;               // Last touch coordinates
bool touched = false;                          // Is screen currently touched?

// Keyboard layout
const char* keyboardRows[KEYBOARD_ROWS] = {
  "1234567890",                   // Row 0: Numbers
  "qwertyuiop",                   // Row 1: QWERTY top row
  "asdfghjkl",                    // Row 2: QWERTY middle row
  "zxcvbnm"                       // Row 3: QWERTY bottom row
};

const char* keyboardRowsShift[KEYBOARD_ROWS] = {
  "!@#$%^&*()",                   // Row 0: Shifted numbers
  "QWERTYUIOP",                   // Row 1: Uppercase
  "ASDFGHJKL",                    // Row 2: Uppercase
  "ZXCVBNM"                       // Row 3: Uppercase
};

// Special keys on row 4
const char* specialKeys = "[];',./";
const char* specialKeysShift = "{}:\"<>?";

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================
// Initialization
void setupDisplay();
void setupBluetooth();
void loadMacros();

// Display functions
void drawMainScreen();
void drawConfigScreen();
void drawKeyboard();
void drawTextInputArea();
void clearScreen();

// Touch handling
void handleTouch();
void handlePlaybackTouch(uint16_t x, uint16_t y);
void handleConfigTouch(uint16_t x, uint16_t y);
void handleKeyboardTouch(uint16_t x, uint16_t y);

// Macro functions
void sendMacro(int macroIndex);
void saveMacro(int macroIndex, String text);
String loadMacro(int macroIndex);

// Helper functions
bool isTouchInButton(uint16_t x, uint16_t y, uint16_t bx, uint16_t by, uint16_t bw, uint16_t bh);
void drawButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String label, uint16_t color);
void drawCenteredText(String text, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t size);
void resetSelection();

// =============================================================================
// ARDUINO SETUP
// =============================================================================
void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("ESP32 MacroPad Starting...");

  // Initialize display
  setupDisplay();
  
  // Initialize Bluetooth
  setupBluetooth();
  
  // Load saved macros from NVS
  loadMacros();
  
  // Draw initial screen
  drawMainScreen();
  
  Serial.println("Setup complete!");
}

// =============================================================================
// ARDUINO MAIN LOOP
// =============================================================================
void loop() {
  // Handle touch input
  handleTouch();
  
  // Check for selection timeout (5 seconds)
  if (selectedMacro >= 0 && sendButtonVisible) {
    if (millis() - selectionTime > 5000) {
      Serial.println("Selection timeout - clearing");
      resetSelection();
      drawMainScreen();
    }
  }
  
  // Small delay to prevent excessive polling
  delay(10);
}

// =============================================================================
// INITIALIZATION FUNCTIONS
// =============================================================================

/**
 * Initialize the TFT display and touch controller
 */
void setupDisplay() {
  Serial.println("Initializing display...");
  
  tft.init();
  tft.setRotation(1);  // Landscape orientation (320x240)
  tft.fillScreen(COLOR_BG);
  
  // Optional: Set backlight pin if defined
  #if TFT_LED >= 0
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);  // Turn on backlight
  #endif
  
  // Calibrate touch (you may need to adjust these values for your display)
  uint16_t calData[5] = {275, 3620, 264, 3532, 1};
  tft.setTouch(calData);
  
  Serial.println("Display initialized");
}

/**
 * Initialize Bluetooth keyboard
 */
void setupBluetooth() {
  Serial.println("Initializing Bluetooth...");
  bleKeyboard.begin();
  Serial.println("Bluetooth keyboard started - waiting for connection");
}

/**
 * Load all macro strings from persistent storage
 */
void loadMacros() {
  Serial.println("Loading macros from NVS...");
  
  preferences.begin("macropad", false);  // Read-only mode
  
  for (int i = 0; i < 4; i++) {
    String key = "macro" + String(i);
    macros[i] = preferences.getString(key.c_str(), "Macro " + String(i + 1));
    Serial.print("Loaded ");
    Serial.print(key);
    Serial.print(": ");
    Serial.println(macros[i]);
  }
  
  preferences.end();
}

// =============================================================================
// DISPLAY FUNCTIONS
// =============================================================================

/**
 * Draw the main playback screen with 4 macro buttons
 */
void drawMainScreen() {
  clearScreen();
  currentMode = MODE_PLAYBACK;
  
  // Draw title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(10, 5);
  tft.print("ESP32 MacroPad");
  
  // Draw Bluetooth status
  tft.setTextSize(1);
  tft.setCursor(220, 10);
  if (bleKeyboard.isConnected()) {
    tft.setTextColor(COLOR_BUTTON_SEND);
    tft.print("BT: Connected");
  } else {
    tft.setTextColor(COLOR_BUTTON_SEL);
    tft.print("BT: Waiting...");
  }
  
  // Calculate button positions (2x2 grid)
  uint16_t startX = MACRO_BTN_MARGIN;
  uint16_t startY = 35;
  
  // Draw 4 macro buttons
  for (int i = 0; i < 4; i++) {
    int row = i / 2;
    int col = i % 2;
    uint16_t x = startX + col * (MACRO_BTN_WIDTH + MACRO_BTN_MARGIN);
    uint16_t y = startY + row * (MACRO_BTN_HEIGHT + MACRO_BTN_MARGIN);
    
    uint16_t color = (selectedMacro == i) ? COLOR_BUTTON_SEL : COLOR_BUTTON;
    drawButton(x, y, MACRO_BTN_WIDTH, MACRO_BTN_HEIGHT, "M" + String(i + 1), color);
    
    // Draw truncated macro text below button label
    tft.setTextSize(1);
    tft.setTextColor(COLOR_TEXT);
    String displayText = macros[i];
    if (displayText.length() > 20) {
      displayText = displayText.substring(0, 17) + "...";
    }
    uint16_t textX = x + (MACRO_BTN_WIDTH - displayText.length() * 6) / 2;
    tft.setCursor(textX, y + MACRO_BTN_HEIGHT - 15);
    tft.print(displayText);
  }
  
  // Draw Send button if a macro is selected
  if (sendButtonVisible && selectedMacro >= 0) {
    // Position send button opposite to selected macro
    int row = selectedMacro / 2;
    int col = selectedMacro % 2;
    uint16_t sendX = (col == 0) ? SCREEN_WIDTH - SEND_BTN_WIDTH - 10 : 10;
    uint16_t sendY = (row == 0) ? SCREEN_HEIGHT - SEND_BTN_HEIGHT - 10 : 35;
    
    drawButton(sendX, sendY, SEND_BTN_WIDTH, SEND_BTN_HEIGHT, "SEND", COLOR_BUTTON_SEND);
  }
  
  // Draw Settings button in bottom right corner
  uint16_t settingsX = SCREEN_WIDTH - SETTINGS_BTN_SIZE - 5;
  uint16_t settingsY = SCREEN_HEIGHT - SETTINGS_BTN_SIZE - 5;
  drawButton(settingsX, settingsY, SETTINGS_BTN_SIZE, SETTINGS_BTN_SIZE, "SET", COLOR_SETTINGS);
}

/**
 * Draw the configuration screen with macro selection
 */
void drawConfigScreen() {
  clearScreen();
  currentMode = MODE_CONFIG;
  
  // Draw title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(10, 5);
  tft.print("Configure Macros");
  
  // Draw instruction
  tft.setTextSize(1);
  tft.setCursor(10, 25);
  tft.print("Tap a button to edit its macro");
  
  // Draw 4 macro buttons
  uint16_t startX = MACRO_BTN_MARGIN;
  uint16_t startY = 45;
  
  for (int i = 0; i < 4; i++) {
    int row = i / 2;
    int col = i % 2;
    uint16_t x = startX + col * (MACRO_BTN_WIDTH + MACRO_BTN_MARGIN);
    uint16_t y = startY + row * (MACRO_BTN_HEIGHT + MACRO_BTN_MARGIN);
    
    drawButton(x, y, MACRO_BTN_WIDTH, MACRO_BTN_HEIGHT, "Edit M" + String(i + 1), COLOR_BUTTON);
    
    // Show current macro text
    tft.setTextSize(1);
    tft.setTextColor(COLOR_TEXT);
    String displayText = macros[i];
    if (displayText.length() > 18) {
      displayText = displayText.substring(0, 15) + "...";
    }
    uint16_t textX = x + (MACRO_BTN_WIDTH - displayText.length() * 6) / 2;
    tft.setCursor(textX, y + MACRO_BTN_HEIGHT - 15);
    tft.print(displayText);
  }
  
  // Draw Back button
  drawButton(SCREEN_WIDTH - 80, SCREEN_HEIGHT - 35, 70, 30, "BACK", COLOR_SETTINGS);
}

/**
 * Draw the on-screen keyboard for text input
 */
void drawKeyboard() {
  clearScreen();
  currentMode = MODE_EDIT_KEYBOARD;
  
  // Draw title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.print("Edit Macro ");
  tft.print(editingMacro + 1);
  
  // Draw text input area
  drawTextInputArea();
  
  // Draw keyboard rows
  uint16_t startX = 5;
  uint16_t y = KEYBOARD_START_Y;
  
  for (int row = 0; row < KEYBOARD_ROWS; row++) {
    const char* keys = shiftActive ? keyboardRowsShift[row] : keyboardRows[row];
    int numKeys = strlen(keys);
    
    // Center the row
    uint16_t rowWidth = numKeys * (KEY_WIDTH + KEY_SPACING);
    uint16_t x = (SCREEN_WIDTH - rowWidth) / 2;
    
    for (int i = 0; i < numKeys; i++) {
      char keyChar[2] = {keys[i], '\0'};
      drawButton(x, y, KEY_WIDTH, KEY_HEIGHT, String(keyChar), COLOR_KEYBOARD);
      x += KEY_WIDTH + KEY_SPACING;
    }
    
    y += KEY_HEIGHT + KEY_SPACING;
  }
  
  // Draw special character row
  const char* specials = shiftActive ? specialKeysShift : specialKeys;
  int numSpecials = strlen(specials);
  uint16_t x = (SCREEN_WIDTH - numSpecials * (KEY_WIDTH + KEY_SPACING)) / 2;
  
  for (int i = 0; i < numSpecials; i++) {
    char keyChar[2] = {specials[i], '\0'};
    drawButton(x, y, KEY_WIDTH, KEY_HEIGHT, String(keyChar), COLOR_KEYBOARD);
    x += KEY_WIDTH + KEY_SPACING;
  }
  
  y += KEY_HEIGHT + KEY_SPACING;
  
  // Draw control keys (Shift, Space, Backspace, Save)
  x = 5;
  uint16_t shiftColor = shiftActive ? COLOR_KEY_PRESSED : COLOR_KEYBOARD;
  drawButton(x, y, 50, KEY_HEIGHT, "SHIFT", shiftColor);
  x += 55;
  
  drawButton(x, y, 100, KEY_HEIGHT, "SPACE", COLOR_KEYBOARD);
  x += 105;
  
  drawButton(x, y, 60, KEY_HEIGHT, "BKSP", COLOR_KEYBOARD);
  x += 65;
  
  drawButton(x, y, 85, KEY_HEIGHT, "SAVE", COLOR_BUTTON_SEND);
}

/**
 * Draw the text input area showing current edit buffer
 */
void drawTextInputArea() {
  // Draw input box
  uint16_t boxX = 5;
  uint16_t boxY = 20;
  uint16_t boxW = SCREEN_WIDTH - 10;
  uint16_t boxH = 90;
  
  tft.fillRect(boxX, boxY, boxW, boxH, TFT_BLACK);
  tft.drawRect(boxX, boxY, boxW, boxH, COLOR_TEXT);
  
  // Draw text with word wrapping
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  
  uint16_t cursorX = boxX + 5;
  uint16_t cursorY = boxY + 5;
  uint16_t lineHeight = 18;
  uint16_t maxWidth = boxW - 10;
  uint16_t charWidth = 12;  // Approximate width for size 2
  
  int startIdx = 0;
  while (startIdx < editBuffer.length()) {
    // Calculate how many characters fit on this line
    int charsPerLine = maxWidth / charWidth;
    int endIdx = min(startIdx + charsPerLine, (int)editBuffer.length());
    
    // Don't cut words in half - find last space
    if (endIdx < editBuffer.length()) {
      int lastSpace = editBuffer.lastIndexOf(' ', endIdx);
      if (lastSpace > startIdx) {
        endIdx = lastSpace + 1;
      }
    }
    
    // Draw the line
    String line = editBuffer.substring(startIdx, endIdx);
    tft.setCursor(cursorX, cursorY);
    tft.print(line);
    
    // Move to next line
    cursorY += lineHeight;
    startIdx = endIdx;
    
    // Stop if we run out of space
    if (cursorY > boxY + boxH - lineHeight) {
      break;
    }
  }
  
  // Draw cursor
  tft.fillRect(cursorX + (editBuffer.length() % 20) * charWidth, cursorY, 2, 14, COLOR_TEXT);
}

/**
 * Clear the entire screen
 */
void clearScreen() {
  tft.fillScreen(COLOR_BG);
}

// =============================================================================
// TOUCH HANDLING FUNCTIONS
// =============================================================================

/**
 * Main touch handler - dispatches to mode-specific handlers
 */
void handleTouch() {
  uint16_t x, y;
  touched = tft.getTouch(&x, &y);
  
  if (!touched) {
    return;  // No touch detected
  }
  
  // Debounce - wait for release
  delay(200);
  
  // Store touch coordinates
  touchX = x;
  touchY = y;
  
  Serial.print("Touch detected at: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);
  
  // Dispatch to appropriate handler based on mode
  switch (currentMode) {
    case MODE_PLAYBACK:
      handlePlaybackTouch(x, y);
      break;
    case MODE_CONFIG:
      handleConfigTouch(x, y);
      break;
    case MODE_EDIT_KEYBOARD:
      handleKeyboardTouch(x, y);
      break;
  }
}

/**
 * Handle touch in playback mode
 */
void handlePlaybackTouch(uint16_t x, uint16_t y) {
  // Check Settings button
  uint16_t settingsX = SCREEN_WIDTH - SETTINGS_BTN_SIZE - 5;
  uint16_t settingsY = SCREEN_HEIGHT - SETTINGS_BTN_SIZE - 5;
  if (isTouchInButton(x, y, settingsX, settingsY, SETTINGS_BTN_SIZE, SETTINGS_BTN_SIZE)) {
    Serial.println("Settings button pressed");
    resetSelection();
    drawConfigScreen();
    return;
  }
  
  // Check Send button if visible
  if (sendButtonVisible && selectedMacro >= 0) {
    int row = selectedMacro / 2;
    int col = selectedMacro % 2;
    uint16_t sendX = (col == 0) ? SCREEN_WIDTH - SEND_BTN_WIDTH - 10 : 10;
    uint16_t sendY = (row == 0) ? SCREEN_HEIGHT - SEND_BTN_HEIGHT - 10 : 35;
    
    if (isTouchInButton(x, y, sendX, sendY, SEND_BTN_WIDTH, SEND_BTN_HEIGHT)) {
      Serial.println("Send button pressed");
      sendMacro(selectedMacro);
      resetSelection();
      drawMainScreen();
      return;
    }
  }
  
  // Check macro buttons
  uint16_t startX = MACRO_BTN_MARGIN;
  uint16_t startY = 35;
  
  for (int i = 0; i < 4; i++) {
    int row = i / 2;
    int col = i % 2;
    uint16_t btnX = startX + col * (MACRO_BTN_WIDTH + MACRO_BTN_MARGIN);
    uint16_t btnY = startY + row * (MACRO_BTN_HEIGHT + MACRO_BTN_MARGIN);
    
    if (isTouchInButton(x, y, btnX, btnY, MACRO_BTN_WIDTH, MACRO_BTN_HEIGHT)) {
      Serial.print("Macro button ");
      Serial.print(i);
      Serial.println(" pressed");
      
      selectedMacro = i;
      selectionTime = millis();
      sendButtonVisible = true;
      drawMainScreen();
      return;
    }
  }
  
  // Touch elsewhere - cancel selection
  if (selectedMacro >= 0) {
    Serial.println("Touch elsewhere - canceling selection");
    resetSelection();
    drawMainScreen();
  }
}

/**
 * Handle touch in configuration mode
 */
void handleConfigTouch(uint16_t x, uint16_t y) {
  // Check Back button
  if (isTouchInButton(x, y, SCREEN_WIDTH - 80, SCREEN_HEIGHT - 35, 70, 30)) {
    Serial.println("Back button pressed");
    drawMainScreen();
    return;
  }
  
  // Check macro buttons
  uint16_t startX = MACRO_BTN_MARGIN;
  uint16_t startY = 45;
  
  for (int i = 0; i < 4; i++) {
    int row = i / 2;
    int col = i % 2;
    uint16_t btnX = startX + col * (MACRO_BTN_WIDTH + MACRO_BTN_MARGIN);
    uint16_t btnY = startY + row * (MACRO_BTN_HEIGHT + MACRO_BTN_MARGIN);
    
    if (isTouchInButton(x, y, btnX, btnY, MACRO_BTN_WIDTH, MACRO_BTN_HEIGHT)) {
      Serial.print("Edit macro ");
      Serial.println(i);
      
      editingMacro = i;
      editBuffer = macros[i];
      shiftActive = false;
      drawKeyboard();
      return;
    }
  }
}

/**
 * Handle touch on the on-screen keyboard
 */
void handleKeyboardTouch(uint16_t x, uint16_t y) {
  uint16_t startY = KEYBOARD_START_Y;
  
  // Check keyboard rows
  for (int row = 0; row < KEYBOARD_ROWS; row++) {
    const char* keys = shiftActive ? keyboardRowsShift[row] : keyboardRows[row];
    int numKeys = strlen(keys);
    uint16_t rowWidth = numKeys * (KEY_WIDTH + KEY_SPACING);
    uint16_t startX = (SCREEN_WIDTH - rowWidth) / 2;
    
    if (y >= startY && y <= startY + KEY_HEIGHT) {
      for (int i = 0; i < numKeys; i++) {
        uint16_t keyX = startX + i * (KEY_WIDTH + KEY_SPACING);
        if (x >= keyX && x <= keyX + KEY_WIDTH) {
          char keyChar = keys[i];
          Serial.print("Key pressed: ");
          Serial.println(keyChar);
          
          editBuffer += keyChar;
          
          // Auto-disable shift after one character
          if (shiftActive) {
            shiftActive = false;
          }
          
          drawKeyboard();
          return;
        }
      }
    }
    
    startY += KEY_HEIGHT + KEY_SPACING;
  }
  
  // Check special character row
  const char* specials = shiftActive ? specialKeysShift : specialKeys;
  int numSpecials = strlen(specials);
  uint16_t rowWidth = numSpecials * (KEY_WIDTH + KEY_SPACING);
  uint16_t startX = (SCREEN_WIDTH - rowWidth) / 2;
  
  if (y >= startY && y <= startY + KEY_HEIGHT) {
    for (int i = 0; i < numSpecials; i++) {
      uint16_t keyX = startX + i * (KEY_WIDTH + KEY_SPACING);
      if (x >= keyX && x <= keyX + KEY_WIDTH) {
        char keyChar = specials[i];
        Serial.print("Special key pressed: ");
        Serial.println(keyChar);
        
        editBuffer += keyChar;
        
        // Auto-disable shift after one character
        if (shiftActive) {
          shiftActive = false;
        }
        
        drawKeyboard();
        return;
      }
    }
  }
  
  startY += KEY_HEIGHT + KEY_SPACING;
  
  // Check control keys
  uint16_t ctrlX = 5;
  
  // Shift
  if (isTouchInButton(x, y, ctrlX, startY, 50, KEY_HEIGHT)) {
    Serial.println("Shift pressed");
    shiftActive = !shiftActive;
    drawKeyboard();
    return;
  }
  ctrlX += 55;
  
  // Space
  if (isTouchInButton(x, y, ctrlX, startY, 100, KEY_HEIGHT)) {
    Serial.println("Space pressed");
    editBuffer += ' ';
    drawKeyboard();
    return;
  }
  ctrlX += 105;
  
  // Backspace
  if (isTouchInButton(x, y, ctrlX, startY, 60, KEY_HEIGHT)) {
    Serial.println("Backspace pressed");
    if (editBuffer.length() > 0) {
      editBuffer = editBuffer.substring(0, editBuffer.length() - 1);
    }
    drawKeyboard();
    return;
  }
  ctrlX += 65;
  
  // Save
  if (isTouchInButton(x, y, ctrlX, startY, 85, KEY_HEIGHT)) {
    Serial.println("Save pressed");
    saveMacro(editingMacro, editBuffer);
    macros[editingMacro] = editBuffer;
    editingMacro = -1;
    editBuffer = "";
    shiftActive = false;
    drawConfigScreen();
    return;
  }
}

// =============================================================================
// MACRO FUNCTIONS
// =============================================================================

/**
 * Send a macro string via Bluetooth keyboard
 */
void sendMacro(int macroIndex) {
  if (macroIndex < 0 || macroIndex > 3) {
    Serial.println("Invalid macro index");
    return;
  }
  
  if (!bleKeyboard.isConnected()) {
    Serial.println("Bluetooth not connected - cannot send macro");
    
    // Show error message
    tft.fillRect(50, 100, 220, 40, COLOR_BUTTON_SEL);
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);
    tft.setCursor(60, 110);
    tft.print("BT Not Connected!");
    delay(2000);
    
    return;
  }
  
  String text = macros[macroIndex];
  Serial.print("Sending macro: ");
  Serial.println(text);
  
  // Show sending indicator
  tft.fillRect(80, 100, 160, 40, COLOR_BUTTON_SEND);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(95, 110);
  tft.print("Sending...");
  
  // Send each character
  for (int i = 0; i < text.length(); i++) {
    bleKeyboard.print(text[i]);
    delay(10);  // Small delay between characters
  }
  
  Serial.println("Macro sent!");
  
  // Show success message
  delay(500);
  tft.fillRect(80, 100, 160, 40, COLOR_BUTTON_SEND);
  tft.setCursor(110, 110);
  tft.print("Sent!");
  delay(1000);
}

/**
 * Save a macro string to persistent storage
 */
void saveMacro(int macroIndex, String text) {
  if (macroIndex < 0 || macroIndex > 3) {
    Serial.println("Invalid macro index");
    return;
  }
  
  String key = "macro" + String(macroIndex);
  Serial.print("Saving ");
  Serial.print(key);
  Serial.print(": ");
  Serial.println(text);
  
  preferences.begin("macropad", false);  // Read-write mode
  preferences.putString(key.c_str(), text);
  preferences.end();
  
  Serial.println("Macro saved to NVS");
}

/**
 * Load a macro string from persistent storage
 */
String loadMacro(int macroIndex) {
  if (macroIndex < 0 || macroIndex > 3) {
    return "";
  }
  
  String key = "macro" + String(macroIndex);
  preferences.begin("macropad", false);
  String text = preferences.getString(key.c_str(), "Macro " + String(macroIndex + 1));
  preferences.end();
  
  return text;
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * Check if touch coordinates are within a button's bounds
 */
bool isTouchInButton(uint16_t x, uint16_t y, uint16_t bx, uint16_t by, uint16_t bw, uint16_t bh) {
  return (x >= bx && x <= bx + bw && y >= by && y <= by + bh);
}

/**
 * Draw a button with label
 */
void drawButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String label, uint16_t color) {
  // Draw button background
  tft.fillRoundRect(x, y, w, h, 5, color);
  tft.drawRoundRect(x, y, w, h, 5, COLOR_TEXT);
  
  // Draw label centered
  drawCenteredText(label, x, y, w, h, 2);
}

/**
 * Draw centered text within a rectangular area
 */
void drawCenteredText(String text, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t size) {
  tft.setTextSize(size);
  tft.setTextColor(COLOR_TEXT);
  
  // Calculate text bounds (approximate)
  int16_t textWidth = text.length() * 6 * size;
  int16_t textHeight = 8 * size;
  
  // Center the text
  int16_t textX = x + (w - textWidth) / 2;
  int16_t textY = y + (h - textHeight) / 2;
  
  tft.setCursor(textX, textY);
  tft.print(text);
}

/**
 * Reset macro selection state
 */
void resetSelection() {
  selectedMacro = -1;
  sendButtonVisible = false;
  selectionTime = 0;
}
