/*
 * ESP32 MacroPad - Bluetooth HID Macro Keyboard Firmware (ESP-IDF Version)
 * 
 * Author: Generated for ESP32-WROOM-32 platform using ESP-IDF
 * License: MIT
 * 
 * Description:
 * Complete firmware for a custom Bluetooth HID macro keyboard using ESP32-WROOM-32
 * and ILI9341 TFT touchscreen display. Features 4 configurable macro buttons with
 * on-screen keyboard for configuration and persistent storage.
 * 
 * This implementation uses ESP-IDF (Espressif IoT Development Framework) instead
 * of Arduino framework for better performance and native ESP32 support.
 * 
 * =============================================================================
 * GETTING STARTED WITH ESP-IDF
 * =============================================================================
 * 
 * Prerequisites:
 * 1. ESP-IDF v5.0 or later installed
 *    - Follow: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/
 *    - Install all prerequisites (Python, CMake, toolchain)
 *    - Set up IDF_PATH environment variable
 * 
 * 2. Hardware setup:
 *    - ESP32-WROOM-32 development board
 *    - ILI9341 320x240 TFT touchscreen with XPT2046 touch controller
 *    - USB cable for programming
 * 
 * Building and Flashing:
 * 1. Navigate to project directory
 * 2. Set ESP-IDF environment: `. $HOME/esp/esp-idf/export.sh`
 * 3. Configure project: `idf.py menuconfig`
 * 4. Build: `idf.py build`
 * 5. Flash: `idf.py -p /dev/ttyUSB0 flash monitor`
 * 
 * Project Structure:
 * esp32_macropad/
 * ├── CMakeLists.txt          # Root build configuration
 * ├── main/
 * │   ├── CMakeLists.txt      # Main component build config
 * │   ├── main.c              # This file - main application
 * │   ├── display.c           # Display driver and UI (to be created)
 * │   ├── bluetooth.c         # BLE HID implementation (to be created)
 * │   └── storage.c           # NVS storage (to be created)
 * └── components/             # External components (optional)
 * 
 * =============================================================================
 * PINOUT DIAGRAM - ESP32-WROOM-32E to 2.8inch ESP32-32E Display (QD-TFT2803)
 * =============================================================================
 * 
 * Reference: https://www.lcdwiki.com/res/E32R28T/QD-TFT2803_specification_v1.1.pdf
 * 
 * IMPORTANT: This hardware uses SEPARATE SPI buses for display and touch!
 * 
 * Display (ILI9341) - Uses VSPI:
 * ESP32 Pin    ->    Display Pin     ->    Function
 * ---------         -------------          --------
 * GPIO 13      ->    SDI (MOSI)      ->    SPI Data Out
 * GPIO 12      ->    SDO (MISO)      ->    SPI Data In
 * GPIO 14      ->    SCK             ->    SPI Clock
 * GPIO 15      ->    CS              ->    Chip Select (active low)
 * GPIO 2       ->    DC (D/C)        ->    Data/Command (High: data, Low: command)
 * EN           ->    RST (Reset)     ->    Shared reset with ESP32 (use -1 in code)
 * GPIO 21      ->    LED             ->    Backlight (High: on, Low: off)
 * 
 * Touch Controller (XPT2046) - Uses HSPI (separate bus!):
 * ESP32 Pin    ->    Touch Pin       ->    Function
 * ---------         -----------            --------
 * GPIO 32      ->    T_DIN (MOSI)    ->    SPI Data Out
 * GPIO 39      ->    T_DO (MISO)     ->    SPI Data In (input only pin)
 * GPIO 25      ->    T_CLK (SCLK)    ->    SPI Clock
 * GPIO 33      ->    T_CS            ->    Chip Select (active low)
 * GPIO 36      ->    T_IRQ           ->    Touch Interrupt (active low, input only)
 * 
 * Power:
 * 3.3V         ->    VCC
 * GND          ->    GND
 * 
 * NOTE: Touch pins 36 and 39 are input-only GPIO pins on ESP32
 * 
 * =============================================================================
 * CODE OVERVIEW (ESP-IDF Architecture)
 * =============================================================================
 * 
 * Main Components:
 * 
 * 1. FreeRTOS Tasks:
 *    - UI Task: Handles display rendering and touch input
 *    - BLE Task: Manages Bluetooth HID connection and sending
 *    - Event handlers for system events
 * 
 * 2. Display Management:
 *    - Uses ESP-IDF SPI Master driver
 *    - ILI9341 initialization sequence implemented
 *    - Hardware and software reset on startup
 *    - Detailed console logging for initialization steps
 *    - Power control, gamma correction, and pixel format setup
 *    - XPT2046 touch controller on separate SPI bus
 *    - Reference: https://www.lcdwiki.com/2.8inch_ESP32-32E_Display
 *    - Datasheet: https://www.lcdwiki.com/res/E32R28T/QD-TFT2803_specification_v1.1.pdf
 * 
 * 3. Bluetooth HID:
 *    - Uses ESP-IDF Bluedroid stack
 *    - HID Device profile implementation
 *    - Keyboard report descriptor
 *    - Connection state management
 * 
 * 4. Persistent Storage:
 *    - NVS (Non-Volatile Storage) Flash API
 *    - Namespace: "macropad"
 *    - Keys: "macro0", "macro1", "macro2", "macro3"
 * 
 * Operating Modes:
 * 
 * A. PLAYBACK MODE (Main Screen)
 *    - Displays 4 large macro buttons + settings button
 *    - Two-step safeguard for sending macros
 *    - Touch detection with debouncing
 * 
 * B. CONFIGURATION MODE
 *    - Shows 4 macro buttons for editing
 *    - On-screen QWERTY keyboard
 *    - Real-time text preview
 *    - Saves to NVS on completion
 * 
 * =============================================================================
 * TESTING STRATEGY
 * =============================================================================
 * 
 * ESP-IDF provides comprehensive testing tools:
 * 
 * 1. Unit Testing:
 *    - Use ESP-IDF Unity test framework
 *    - Place tests in test/ directory
 *    - Run with: `idf.py test`
 * 
 * 2. Integration Testing:
 *    - Use esp_event for event-driven testing
 *    - Mock Bluetooth and SPI interfaces
 *    - Verify state transitions
 * 
 * 3. Hardware Testing:
 *    - Monitor output: `idf.py monitor`
 *    - Check logs with ESP_LOGI, ESP_LOGW, ESP_LOGE
 *    - Use JTAG debugging for complex issues
 * 
 * 4. Manual Testing:
 *    - Follow test cases in TESTING.md
 *    - Verify display rendering
 *    - Test Bluetooth pairing and data transmission
 *    - Validate NVS persistence across reboots
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "version.h"

// Logging tag
static const char *TAG = "MACROPAD";

// =============================================================================
// PIN DEFINITIONS - 2.8inch ESP32-32E Display (QD-TFT2803)
// =============================================================================
// Reference: https://www.lcdwiki.com/res/E32R28T/QD-TFT2803_specification_v1.1.pdf
//
// Display and Touch use separate SPI buses on this hardware!
// Display uses VSPI pins, Touch uses dedicated pins

// Display SPI pins (VSPI)
#define PIN_TFT_MOSI    13  // LCD SPI MOSI
#define PIN_TFT_MISO    12  // LCD SPI MISO
#define PIN_TFT_SCLK    14  // LCD SPI SCLK
#define PIN_TFT_CS      15  // LCD CS (low level effective)
#define PIN_TFT_DC      2   // LCD DC (High: data, Low: command)
#define PIN_TFT_RST     -1  // LCD RST (shares EN pin with ESP32, use -1 to skip)
#define PIN_BACKLIGHT   21  // LCD backlight (high: on, low: off)

// Touch controller pins (separate SPI)
#define PIN_TOUCH_MOSI  32  // Touch SPI MOSI
#define PIN_TOUCH_MISO  39  // Touch SPI MISO (input only pin)
#define PIN_TOUCH_SCLK  25  // Touch SPI SCLK
#define PIN_TOUCH_CS    33  // Touch CS (low level effective)
#define PIN_TOUCH_IRQ   36  // Touch interrupt (input only, low: touch detected)

// SPI hosts
#define DISPLAY_SPI_HOST    VSPI_HOST  // Display uses VSPI
#define TOUCH_SPI_HOST      HSPI_HOST  // Touch uses HSPI

// =============================================================================
// DISPLAY CONFIGURATION
// =============================================================================

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

// Colors (RGB565 format)
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_DARKBLUE  0x1082
#define COLOR_GRAY      0x7BEF
#define COLOR_ORANGE    0xFD20
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F

// ILI9341 Commands
#define ILI9341_SWRESET     0x01
#define ILI9341_SLPOUT      0x11
#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A
#define ILI9341_FRMCTR1     0xB1
#define ILI9341_DFUNCTR     0xB6
#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7
#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1

// =============================================================================
// APPLICATION CONFIGURATION
// =============================================================================

#define NUM_MACROS      4
#define MAX_MACRO_LEN   512
#define NVS_NAMESPACE   "macropad"

// Button layout configuration
#define BUTTON_MARGIN   10

// Touch press duration thresholds (in milliseconds)
#define SHORT_PRESS_MS      100     // Minimum for valid press
#define CONFIG_PRESS_MS     5000    // 5 seconds for config mode
#define BT_CONFIG_PRESS_MS  10000   // 10 seconds for BT config
#define SELECTION_TIMEOUT_MS 5000   // 5 seconds timeout for macro selection

// Keyboard configuration
#define KEYBOARD_ROWS 3
#define KEYBOARD_MAX_COLS 10
#define KEY_WIDTH 28
#define KEY_HEIGHT 30
#define KEY_MARGIN 2
#define KEYBOARD_START_Y 80

// =============================================================================
// OPERATING MODES
// =============================================================================

typedef enum {
    MODE_DISPLAY_TEST,  // Display test mode on startup
    MODE_PLAYBACK,      // Main screen - playback mode
    MODE_CONFIG,        // Configuration mode - select macro to edit
    MODE_EDIT_KEYBOARD, // Editing mode - on-screen keyboard active
    MODE_BT_CONFIG      // Bluetooth configuration mode
} app_mode_t;

// Keyboard page types
typedef enum {
    KB_PAGE_ALPHA_LOWER,  // Lowercase letters
    KB_PAGE_ALPHA_UPPER,  // Uppercase letters
    KB_PAGE_NUMBERS,      // Numbers and common symbols
    KB_PAGE_SYMBOLS       // Additional symbols
} keyboard_page_t;

// Button structure for tracking touch areas
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t color;
    const char* label;
} button_t;

// =============================================================================
// GLOBAL STATE
// =============================================================================

typedef struct {
    app_mode_t mode;
    int selected_macro;
    bool send_button_visible;
    uint32_t selection_time;
    int editing_macro;
    char edit_buffer[MAX_MACRO_LEN];
    bool shift_active;
    char macros[NUM_MACROS][MAX_MACRO_LEN];
    bool ble_connected;
    
    // Touch state tracking
    uint32_t touch_start_time;
    bool touch_active;
    uint16_t touch_start_x;
    uint16_t touch_start_y;
    
    // Button positions (calculated during draw)
    button_t macro_buttons[NUM_MACROS];
    button_t confirm_button;
    
    // Keyboard state
    keyboard_page_t keyboard_page;
    int edit_buffer_len;
} app_state_t;

static app_state_t app_state = {
    .mode = MODE_DISPLAY_TEST,
    .selected_macro = -1,
    .send_button_visible = false,
    .selection_time = 0,
    .editing_macro = -1,
    .shift_active = false,
    .ble_connected = false,
    .touch_active = false,
    .touch_start_time = 0,
    .touch_start_x = 0,
    .touch_start_y = 0,
    .keyboard_page = KB_PAGE_ALPHA_LOWER,
    .edit_buffer_len = 0
};

// SPI device handle for display
static spi_device_handle_t display_spi;

// SPI device handle for touch controller
static spi_device_handle_t touch_spi;

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

// System initialization
static void init_nvs(void);
static void init_gpio(void);
static void init_spi(void);

// Storage functions
static void load_macros(void);
static void save_macro(int index, const char *text);

// Display functions (to be implemented in display.c)
static void display_init(void);
static void draw_main_screen(void);
static void draw_config_screen(void);
static void draw_keyboard(void);
static void draw_bt_config_screen(void);

// Display helper functions
static void ili9341_fill_screen(uint16_t color);
static void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
static void ili9341_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void ili9341_draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, const char* label);

// Display test functions
static void run_display_test(void);
static bool check_touch_pressed(void);
static bool read_touch_coordinates(uint16_t *x, uint16_t *y);

// Bluetooth functions (to be implemented in bluetooth.c)
static void ble_init(void);
static void ble_send_text(const char *text);

// Touch handling
static void handle_touch_task(void *pvParameters);
static bool is_point_in_button(uint16_t x, uint16_t y, const button_t *button);
static int get_touched_macro_button(uint16_t x, uint16_t y);
static void handle_playback_touch(uint16_t x, uint16_t y, uint32_t press_duration);
static void handle_config_touch(uint16_t x, uint16_t y);
static void handle_keyboard_touch(uint16_t x, uint16_t y);
static void handle_bt_config_touch(uint16_t x, uint16_t y);

// Main tasks
static void ui_task(void *pvParameters);

// =============================================================================
// MAIN APPLICATION ENTRY POINT
// =============================================================================

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 MacroPad Starting...");
    ESP_LOGI(TAG, "Firmware Version: %s", KEYBOT_VERSION);
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());
    
    // Initialize NVS for persistent storage
    init_nvs();
    
    // Initialize GPIO
    init_gpio();
    
    // Initialize SPI for display and touch
    init_spi();
    
    // Load saved macros from NVS
    load_macros();
    
    // Initialize display
    display_init();
    
    // Initialize Bluetooth HID
    ble_init();
    
    // Create UI task
    xTaskCreate(ui_task, "ui_task", 4096, NULL, 5, NULL);
    
    // Create touch handling task
    xTaskCreate(handle_touch_task, "touch_task", 2048, NULL, 4, NULL);
    
    ESP_LOGI(TAG, "Initialization complete!");
    
    // Main loop runs in FreeRTOS tasks, app_main can return
}

// =============================================================================
// INITIALIZATION FUNCTIONS
// =============================================================================

/**
 * Initialize NVS Flash for persistent storage
 */
static void init_nvs(void)
{
    ESP_LOGI(TAG, "Initializing NVS...");
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_LOGI(TAG, "NVS initialized");
}

/**
 * Initialize GPIO pins
 */
static void init_gpio(void)
{
    ESP_LOGI(TAG, "Initializing GPIO...");
    
    // Configure backlight pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_BACKLIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Turn on backlight
    gpio_set_level(PIN_BACKLIGHT, 1);
    
    ESP_LOGI(TAG, "GPIO initialized");
}

/**
 * Initialize SPI buses for display and touch
 * Display and touch use separate SPI buses on the 2.8inch ESP32-32E hardware
 */
static void init_spi(void)
{
    ESP_LOGI(TAG, "Initializing SPI buses...");
    
    // Initialize VSPI for display
    ESP_LOGI(TAG, "Configuring VSPI for display (MOSI:%d, MISO:%d, SCLK:%d)...", 
             PIN_TFT_MOSI, PIN_TFT_MISO, PIN_TFT_SCLK);
    spi_bus_config_t display_bus_cfg = {
        .mosi_io_num = PIN_TFT_MOSI,
        .miso_io_num = PIN_TFT_MISO,
        .sclk_io_num = PIN_TFT_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SCREEN_WIDTH * SCREEN_HEIGHT * 2
    };
    
    esp_err_t ret = spi_bus_initialize(DISPLAY_SPI_HOST, &display_bus_cfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "VSPI initialized for display");
    
    // Initialize HSPI for touch controller
    ESP_LOGI(TAG, "Configuring HSPI for touch (MOSI:%d, MISO:%d, SCLK:%d)...", 
             PIN_TOUCH_MOSI, PIN_TOUCH_MISO, PIN_TOUCH_SCLK);
    spi_bus_config_t touch_bus_cfg = {
        .mosi_io_num = PIN_TOUCH_MOSI,
        .miso_io_num = PIN_TOUCH_MISO,
        .sclk_io_num = PIN_TOUCH_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32  // Touch needs much smaller transfers
    };
    
    ret = spi_bus_initialize(TOUCH_SPI_HOST, &touch_bus_cfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "HSPI initialized for touch controller");
    
    ESP_LOGI(TAG, "SPI buses initialized successfully");
}

// =============================================================================
// STORAGE FUNCTIONS
// =============================================================================

/**
 * Load all macros from NVS
 */
static void load_macros(void)
{
    ESP_LOGI(TAG, "Loading macros from NVS...");
    
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "NVS namespace not found, using defaults");
        // Set default macro texts
        for (int i = 0; i < NUM_MACROS; i++) {
            snprintf(app_state.macros[i], MAX_MACRO_LEN, "Macro %d", i + 1);
        }
        return;
    }
    
    // Load each macro
    for (int i = 0; i < NUM_MACROS; i++) {
        char key[16];
        snprintf(key, sizeof(key), "macro%d", i);
        
        size_t required_size = MAX_MACRO_LEN;
        err = nvs_get_str(nvs_handle, key, app_state.macros[i], &required_size);
        
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            // Default value if not found
            snprintf(app_state.macros[i], MAX_MACRO_LEN, "Macro %d", i + 1);
            ESP_LOGW(TAG, "Macro %d not found, using default", i);
        } else if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error reading macro %d: %s", i, esp_err_to_name(err));
            snprintf(app_state.macros[i], MAX_MACRO_LEN, "Macro %d", i + 1);
        } else {
            ESP_LOGI(TAG, "Loaded macro %d: %.40s%s", i, app_state.macros[i],
                     strlen(app_state.macros[i]) > 40 ? "..." : "");
        }
    }
    
    nvs_close(nvs_handle);
}

/**
 * Save a macro to NVS
 */
static void save_macro(int index, const char *text)
{
    if (index < 0 || index >= NUM_MACROS) {
        ESP_LOGE(TAG, "Invalid macro index: %d", index);
        return;
    }
    
    ESP_LOGI(TAG, "Saving macro %d to NVS...", index);
    
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
        return;
    }
    
    char key[16];
    snprintf(key, sizeof(key), "macro%d", index);
    
    err = nvs_set_str(nvs_handle, key, text);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving macro: %s", esp_err_to_name(err));
    } else {
        // Commit changes
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error committing NVS: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Macro %d saved successfully", index);
            // Update local copy
            strncpy(app_state.macros[index], text, MAX_MACRO_LEN - 1);
            app_state.macros[index][MAX_MACRO_LEN - 1] = '\0';
        }
    }
    
    nvs_close(nvs_handle);
}

// =============================================================================
// DISPLAY FUNCTIONS (Stub implementations - to be completed)
// =============================================================================

/**
 * Send command to ILI9341 display
 */
static void ili9341_send_cmd(uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // D/C needs to be 0 for command
    ret = spi_device_polling_transmit(display_spi, &t);
    ESP_ERROR_CHECK(ret);
}

/**
 * Send data to ILI9341 display
 */
static void ili9341_send_data(const uint8_t *data, int len)
{
    if (len == 0) return;
    
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // D/C needs to be 1 for data
    ret = spi_device_polling_transmit(display_spi, &t);
    ESP_ERROR_CHECK(ret);
}

/**
 * Pre-transfer callback for SPI - sets DC line
 */
static void ili9341_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_TFT_DC, dc);
}

/**
 * Reset the ILI9341 display
 */
static void ili9341_reset(void)
{
    ESP_LOGI(TAG, "Display: Performing hardware reset...");
    
    // Check if reset pin is configured (not -1)
    if (PIN_TFT_RST >= 0) {
        gpio_set_level(PIN_TFT_RST, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(PIN_TFT_RST, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_LOGI(TAG, "Display: Hardware reset complete");
    } else {
        ESP_LOGI(TAG, "Display: Skipping hardware reset (RST pin shared with ESP32 EN)");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * Initialize the display
 * 
 * ILI9341 initialization sequence based on datasheet and
 * reference: https://www.lcdwiki.com/2.8inch_ESP32-32E_Display
 */
static void display_init(void)
{
    ESP_LOGI(TAG, "Display: Starting ILI9341 initialization...");
    ESP_LOGI(TAG, "Display: Hardware: 2.8inch ESP32-32E Display (QD-TFT2803)");
    
    // Configure RST and DC pins as outputs (only if RST is not -1)
    ESP_LOGI(TAG, "Display: Configuring control pins...");
    if (PIN_TFT_RST >= 0) {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << PIN_TFT_RST) | (1ULL << PIN_TFT_DC),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&io_conf);
        ESP_LOGI(TAG, "Display: Control pins configured (RST: GPIO%d, DC: GPIO%d)", PIN_TFT_RST, PIN_TFT_DC);
    } else {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << PIN_TFT_DC),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&io_conf);
        ESP_LOGI(TAG, "Display: Control pins configured (RST: shared EN, DC: GPIO%d)", PIN_TFT_DC);
    }
    
    // Add SPI device for display
    ESP_LOGI(TAG, "Display: Adding SPI device...");
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000,  // 26 MHz
        .mode = 0,                            // SPI mode 0
        .spics_io_num = PIN_TFT_CS,           // CS pin
        .queue_size = 7,                      // Queue 7 transactions at a time
        .pre_cb = ili9341_spi_pre_transfer_callback,  // Callback to handle D/C line
    };
    esp_err_t ret = spi_bus_add_device(DISPLAY_SPI_HOST, &devcfg, &display_spi);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Display: SPI device added (CS: GPIO%d, Clock: 26MHz)", PIN_TFT_CS);
    
    // Hardware reset
    ili9341_reset();
    
    // Software reset
    ESP_LOGI(TAG, "Display: Sending software reset command (0x01)...");
    ili9341_send_cmd(ILI9341_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    ESP_LOGI(TAG, "Display: Software reset complete");
    
    // Sleep out
    ESP_LOGI(TAG, "Display: Waking display from sleep (0x11)...");
    ili9341_send_cmd(ILI9341_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(150));
    ESP_LOGI(TAG, "Display: Sleep mode exited");
    
    // Power control A
    ESP_LOGI(TAG, "Display: Configuring power control...");
    ili9341_send_cmd(0xCB);
    uint8_t power_a[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
    ili9341_send_data(power_a, 5);
    
    // Power control B
    ili9341_send_cmd(0xCF);
    uint8_t power_b[] = {0x00, 0xC1, 0x30};
    ili9341_send_data(power_b, 3);
    
    // Driver timing control A
    ili9341_send_cmd(0xE8);
    uint8_t timing_a[] = {0x85, 0x00, 0x78};
    ili9341_send_data(timing_a, 3);
    
    // Driver timing control B
    ili9341_send_cmd(0xEA);
    uint8_t timing_b[] = {0x00, 0x00};
    ili9341_send_data(timing_b, 2);
    
    // Power on sequence control
    ili9341_send_cmd(0xED);
    uint8_t power_seq[] = {0x64, 0x03, 0x12, 0x81};
    ili9341_send_data(power_seq, 4);
    
    // Pump ratio control
    ili9341_send_cmd(0xF7);
    uint8_t pump[] = {0x20};
    ili9341_send_data(pump, 1);
    ESP_LOGI(TAG, "Display: Power control configured");
    
    // Power control 1
    ESP_LOGI(TAG, "Display: Setting power levels...");
    ili9341_send_cmd(ILI9341_PWCTR1);
    uint8_t pwr1[] = {0x23};
    ili9341_send_data(pwr1, 1);
    
    // Power control 2
    ili9341_send_cmd(ILI9341_PWCTR2);
    uint8_t pwr2[] = {0x10};
    ili9341_send_data(pwr2, 1);
    
    // VCOM control 1
    ili9341_send_cmd(ILI9341_VMCTR1);
    uint8_t vcom1[] = {0x3e, 0x28};
    ili9341_send_data(vcom1, 2);
    
    // VCOM control 2
    ili9341_send_cmd(ILI9341_VMCTR2);
    uint8_t vcom2[] = {0x86};
    ili9341_send_data(vcom2, 1);
    ESP_LOGI(TAG, "Display: Power levels set");
    
    // Memory access control (rotation/orientation)
    ESP_LOGI(TAG, "Display: Setting orientation (landscape, 90 degrees clockwise)...");
    ili9341_send_cmd(ILI9341_MADCTL);
    uint8_t madctl[] = {0x28}; // Landscape mode, rotated 90 degrees clockwise
    ili9341_send_data(madctl, 1);
    ESP_LOGI(TAG, "Display: Orientation set to landscape mode (90 degrees clockwise)");
    
    // Pixel format
    ESP_LOGI(TAG, "Display: Setting pixel format (RGB565)...");
    ili9341_send_cmd(ILI9341_PIXFMT);
    uint8_t pixfmt[] = {0x55}; // 16 bits/pixel
    ili9341_send_data(pixfmt, 1);
    ESP_LOGI(TAG, "Display: Pixel format set to RGB565 (16-bit color)");
    
    // Frame rate control
    ESP_LOGI(TAG, "Display: Configuring frame rate...");
    ili9341_send_cmd(ILI9341_FRMCTR1);
    uint8_t frmctr[] = {0x00, 0x18};
    ili9341_send_data(frmctr, 2);
    
    // Display function control
    ili9341_send_cmd(ILI9341_DFUNCTR);
    uint8_t dfunctr[] = {0x08, 0x82, 0x27};
    ili9341_send_data(dfunctr, 3);
    ESP_LOGI(TAG, "Display: Frame rate configured");
    
    // Gamma function disable
    ESP_LOGI(TAG, "Display: Configuring gamma correction...");
    ili9341_send_cmd(0xF2);
    uint8_t gamma_dis[] = {0x00};
    ili9341_send_data(gamma_dis, 1);
    
    // Gamma curve selected
    ili9341_send_cmd(ILI9341_GAMMASET);
    uint8_t gamma_curve[] = {0x01};
    ili9341_send_data(gamma_curve, 1);
    
    // Positive gamma correction
    ili9341_send_cmd(ILI9341_GMCTRP1);
    uint8_t gmctrp1[] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                         0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
    ili9341_send_data(gmctrp1, 15);
    
    // Negative gamma correction
    ili9341_send_cmd(ILI9341_GMCTRN1);
    uint8_t gmctrn1[] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                         0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
    ili9341_send_data(gmctrn1, 15);
    ESP_LOGI(TAG, "Display: Gamma correction configured");
    
    // Clear display to black
    ESP_LOGI(TAG, "Display: Clearing screen to black...");
    
    // Set column address (0 to 319)
    ili9341_send_cmd(ILI9341_CASET);
    uint8_t caset[] = {0x00, 0x00, 0x01, 0x3F};
    ili9341_send_data(caset, 4);
    
    // Set page address (0 to 239)
    ili9341_send_cmd(ILI9341_PASET);
    uint8_t paset[] = {0x00, 0x00, 0x00, 0xEF};
    ili9341_send_data(paset, 4);
    
    // Write to RAM
    ili9341_send_cmd(ILI9341_RAMWR);
    
    // Send black pixels (simplified - just first few to show it's working)
    uint8_t black_pixel[] = {0x00, 0x00}; // Black in RGB565
    for (int i = 0; i < 100; i++) {
        ili9341_send_data(black_pixel, 2);
    }
    ESP_LOGI(TAG, "Display: Screen cleared");
    
    // Display on
    ESP_LOGI(TAG, "Display: Turning on display (0x29)...");
    ili9341_send_cmd(ILI9341_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "Display: Display is now ON");
    
    ESP_LOGI(TAG, "Display: ILI9341 initialization complete!");
    ESP_LOGI(TAG, "Display: Resolution: %dx%d pixels", SCREEN_WIDTH, SCREEN_HEIGHT);
    ESP_LOGI(TAG, "Display: Color depth: 16-bit (RGB565)");
    ESP_LOGI(TAG, "Display: Orientation: Landscape");
    
    // Initialize touch controller
    ESP_LOGI(TAG, "Touch: Initializing XPT2046 touch controller...");
    ESP_LOGI(TAG, "Touch: Using separate HSPI bus (MOSI:%d, MISO:%d, SCLK:%d)", 
             PIN_TOUCH_MOSI, PIN_TOUCH_MISO, PIN_TOUCH_SCLK);
    
    spi_device_interface_config_t touch_cfg = {
        .clock_speed_hz = 2 * 1000 * 1000,  // 2 MHz for touch (slower than display)
        .mode = 0,                           // SPI mode 0
        .spics_io_num = PIN_TOUCH_CS,       // Touch CS pin
        .queue_size = 1,
        .flags = SPI_DEVICE_NO_DUMMY,
    };
    ret = spi_bus_add_device(TOUCH_SPI_HOST, &touch_cfg, &touch_spi);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Touch: XPT2046 initialized (CS: GPIO%d, IRQ: GPIO%d, Clock: 2MHz)", 
             PIN_TOUCH_CS, PIN_TOUCH_IRQ);
    
    // Configure touch IRQ pin as input (optional, for interrupt-based touch detection)
    gpio_config_t touch_irq_conf = {
        .pin_bit_mask = (1ULL << PIN_TOUCH_IRQ),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,  // XPT2046 IRQ is active low
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE  // Not using interrupts yet, just polling
    };
    gpio_config(&touch_irq_conf);
    ESP_LOGI(TAG, "Touch: IRQ pin configured for polling");
}

/**
 * Set address window for display operations
 */
static void ili9341_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // Column address set
    ili9341_send_cmd(ILI9341_CASET);
    uint8_t caset[] = {
        (uint8_t)(x0 >> 8),
        (uint8_t)(x0 & 0xFF),
        (uint8_t)(x1 >> 8),
        (uint8_t)(x1 & 0xFF)
    };
    ili9341_send_data(caset, 4);
    
    // Page address set
    ili9341_send_cmd(ILI9341_PASET);
    uint8_t paset[] = {
        (uint8_t)(y0 >> 8),
        (uint8_t)(y0 & 0xFF),
        (uint8_t)(y1 >> 8),
        (uint8_t)(y1 & 0xFF)
    };
    ili9341_send_data(paset, 4);
    
    // Write to RAM
    ili9341_send_cmd(ILI9341_RAMWR);
}

/**
 * Fill a rectangular area with a color
 */
static void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    if (x + w > SCREEN_WIDTH) w = SCREEN_WIDTH - x;
    if (y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
    
    ili9341_set_addr_window(x, y, x + w - 1, y + h - 1);
    
    // Prepare color bytes (RGB565 is big-endian)
    uint8_t color_high = (uint8_t)(color >> 8);
    uint8_t color_low = (uint8_t)(color & 0xFF);
    
    // Send color data for each pixel
    // For efficiency, we'll send in chunks
    uint32_t total_pixels = (uint32_t)w * h;
    uint8_t buffer[512]; // Send 256 pixels at a time
    
    // Fill buffer with color pattern
    for (int i = 0; i < 512; i += 2) {
        buffer[i] = color_high;
        buffer[i + 1] = color_low;
    }
    
    // Send data in chunks
    while (total_pixels > 0) {
        uint32_t chunk_size = (total_pixels > 256) ? 256 : total_pixels;
        ili9341_send_data(buffer, chunk_size * 2);
        total_pixels -= chunk_size;
    }
}

/**
 * Fill entire screen with a color
 */
static void ili9341_fill_screen(uint16_t color)
{
    ili9341_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
}

/**
 * Draw a simple button with a label
 * For now, just draws a filled rectangle (text rendering to be added later)
 */
static void ili9341_draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, const char* label)
{
    // Draw filled rectangle for button
    ili9341_fill_rect(x, y, w, h, color);
    
    // TODO: Add text rendering for label
    // For now, just draw the button background
    (void)label; // Suppress unused parameter warning
}

// =============================================================================
// TOUCH CONTROLLER FUNCTIONS
// =============================================================================

/**
 * Read raw value from XPT2046 touch controller
 */
static uint16_t xpt2046_read(uint8_t command)
{
    uint8_t tx_data[3] = {command, 0x00, 0x00};
    uint8_t rx_data[3] = {0};
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 24;  // 3 bytes = 24 bits
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;
    
    esp_err_t ret = spi_device_polling_transmit(touch_spi, &t);
    if (ret != ESP_OK) {
        return 0;
    }
    
    // XPT2046 returns 12-bit value in bits [14:3] of the response
    uint16_t value = ((uint16_t)rx_data[1] << 8) | rx_data[2];
    value = (value >> 3) & 0xFFF;
    
    return value;
}

/**
 * Read touch coordinates from XPT2046
 * Returns true if touch is detected and coordinates are valid
 */
static bool read_touch_coordinates(uint16_t *x, uint16_t *y)
{
    // XPT2046 commands:
    // 0xD0 = Read X position (12-bit, differential mode, power down between conversions)
    // 0x90 = Read Y position (12-bit, differential mode, power down between conversions)
    // 0xB0 = Read Z1 position (pressure indicator)
    
    // First check if screen is being touched by reading pressure
    uint16_t z1 = xpt2046_read(0xB1);
    
    // If pressure is too low, no valid touch
    if (z1 < 100) {
        return false;
    }
    
    // Read X and Y coordinates
    // We read twice and average to reduce noise
    uint16_t x1 = xpt2046_read(0xD0);
    uint16_t x2 = xpt2046_read(0xD0);
    uint16_t y1 = xpt2046_read(0x90);
    uint16_t y2 = xpt2046_read(0x90);
    
    // Average the readings
    *x = (x1 + x2) / 2;
    *y = (y1 + y2) / 2;
    
    return true;
}

/**
 * Read touch controller Z position (pressure)
 * Returns true if screen is being touched
 */
static bool check_touch_pressed(void)
{
    // Read Z1 position (pressure indicator)
    uint16_t z1 = xpt2046_read(0xB1);
    
    // If Z1 is above a threshold, touch is detected
    // Typical threshold is around 100-200 for touch detection
    return (z1 > 100);
}

// =============================================================================
// DISPLAY TEST SEQUENCE
// =============================================================================

/**
 * Run display test sequence
 * 
 * This function runs a series of graphical tests to verify display functionality.
 * Based on common LCD test patterns from: https://www.lcdwiki.com/2.8inch_ESP32-32E_Display
 * 
 * Tests include:
 * - Full screen color fills (Red, Green, Blue, White, Black)
 * - Color bars
 * - Checkerboard pattern
 * - Gradient patterns
 * 
 * The test runs continuously until the touchscreen is pressed.
 */
static void run_display_test(void)
{
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Starting Display Test Sequence");
    ESP_LOGI(TAG, "Touch screen to exit and start normal operation");
    ESP_LOGI(TAG, "========================================");
    
    bool test_running = true;
    int test_cycle = 0;
    
    while (test_running) {
        test_cycle++;
        ESP_LOGI(TAG, "Test cycle %d", test_cycle);
        
        // Test 1: Primary Colors
        ESP_LOGI(TAG, "Test 1: Red screen");
        ili9341_fill_screen(COLOR_RED);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        ESP_LOGI(TAG, "Test 1: Green screen");
        ili9341_fill_screen(COLOR_GREEN);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        ESP_LOGI(TAG, "Test 1: Blue screen");
        ili9341_fill_screen(COLOR_BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 2: Secondary Colors
        ESP_LOGI(TAG, "Test 2: Yellow screen");
        ili9341_fill_screen(COLOR_YELLOW);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        ESP_LOGI(TAG, "Test 2: Cyan screen");
        ili9341_fill_screen(COLOR_CYAN);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        ESP_LOGI(TAG, "Test 2: Magenta screen");
        ili9341_fill_screen(COLOR_MAGENTA);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 3: Grayscale
        ESP_LOGI(TAG, "Test 3: White screen");
        ili9341_fill_screen(COLOR_WHITE);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        ESP_LOGI(TAG, "Test 3: Gray screen");
        ili9341_fill_screen(COLOR_GRAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        ESP_LOGI(TAG, "Test 3: Black screen");
        ili9341_fill_screen(COLOR_BLACK);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 4: Color Bars (Vertical)
        ESP_LOGI(TAG, "Test 4: Vertical color bars");
        uint16_t bar_colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, 
                                  COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA, 
                                  COLOR_WHITE, COLOR_BLACK};
        int bar_width = SCREEN_WIDTH / 8;
        for (int i = 0; i < 8; i++) {
            ili9341_fill_rect(i * bar_width, 0, bar_width, SCREEN_HEIGHT, bar_colors[i]);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 5: Color Bars (Horizontal)
        ESP_LOGI(TAG, "Test 5: Horizontal color bars");
        int bar_height = SCREEN_HEIGHT / 8;
        for (int i = 0; i < 8; i++) {
            ili9341_fill_rect(0, i * bar_height, SCREEN_WIDTH, bar_height, bar_colors[i]);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 6: Checkerboard Pattern
        ESP_LOGI(TAG, "Test 6: Checkerboard pattern");
        int check_size = 40;
        for (int y = 0; y < SCREEN_HEIGHT; y += check_size) {
            for (int x = 0; x < SCREEN_WIDTH; x += check_size) {
                uint16_t color = ((x / check_size + y / check_size) % 2) ? COLOR_WHITE : COLOR_BLACK;
                ili9341_fill_rect(x, y, check_size, check_size, color);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 7: Corner rectangles
        ESP_LOGI(TAG, "Test 7: Corner rectangles");
        ili9341_fill_screen(COLOR_BLACK);
        ili9341_fill_rect(0, 0, 80, 60, COLOR_RED);                              // Top-left
        ili9341_fill_rect(SCREEN_WIDTH - 80, 0, 80, 60, COLOR_GREEN);           // Top-right
        ili9341_fill_rect(0, SCREEN_HEIGHT - 60, 80, 60, COLOR_BLUE);           // Bottom-left
        ili9341_fill_rect(SCREEN_WIDTH - 80, SCREEN_HEIGHT - 60, 80, 60, COLOR_YELLOW); // Bottom-right
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
        
        // Test 8: Center cross pattern
        ESP_LOGI(TAG, "Test 8: Center cross pattern");
        ili9341_fill_screen(COLOR_BLACK);
        ili9341_fill_rect(SCREEN_WIDTH / 2 - 5, 0, 10, SCREEN_HEIGHT, COLOR_WHITE); // Vertical
        ili9341_fill_rect(0, SCREEN_HEIGHT / 2 - 5, SCREEN_WIDTH, 10, COLOR_WHITE); // Horizontal
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (check_touch_pressed()) {
            test_running = false;
            break;
        }
    }
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Display Test Complete - Touch detected!");
    ESP_LOGI(TAG, "Starting normal operation...");
    ESP_LOGI(TAG, "========================================");
    
    // Wait for touch release
    while (check_touch_pressed()) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(200)); // Debounce delay
    
    // Clear screen before transitioning to normal mode
    ili9341_fill_screen(COLOR_BLACK);
}

/**
 * Draw the main playback screen
 */
static void draw_main_screen(void)
{
    ESP_LOGI(TAG, "Display: Drawing main screen...");
    ESP_LOGI(TAG, "Display: Main screen layout - 4 macro buttons + settings button");
    ESP_LOGI(TAG, "Display: Version: %s", KEYBOT_VERSION);
    
    // Clear screen to black background
    ili9341_fill_screen(COLOR_BLACK);
    
    // Define button layout (2x2 grid of macro buttons)
    // Screen is 320x240 in landscape mode
    // Leave some margin around buttons
    const uint16_t margin = BUTTON_MARGIN;
    const uint16_t button_width = (SCREEN_WIDTH - 3 * margin) / 2;  // Two buttons horizontally
    const uint16_t button_height = (SCREEN_HEIGHT - 3 * margin) / 2; // Two buttons vertically
    
    // Button positions (quadrant layout as described in requirements)
    // Store positions in app_state for touch detection
    
    // Top-left: Red (M1) - macro 0
    app_state.macro_buttons[0].x = margin;
    app_state.macro_buttons[0].y = margin;
    app_state.macro_buttons[0].width = button_width;
    app_state.macro_buttons[0].height = button_height;
    app_state.macro_buttons[0].color = COLOR_RED;
    app_state.macro_buttons[0].label = "M1";
    
    // Top-right: Green (M2) - macro 1
    app_state.macro_buttons[1].x = SCREEN_WIDTH / 2 + margin / 2;
    app_state.macro_buttons[1].y = margin;
    app_state.macro_buttons[1].width = button_width;
    app_state.macro_buttons[1].height = button_height;
    app_state.macro_buttons[1].color = COLOR_GREEN;
    app_state.macro_buttons[1].label = "M2";
    
    // Bottom-left: Blue (M3) - macro 2
    app_state.macro_buttons[2].x = margin;
    app_state.macro_buttons[2].y = SCREEN_HEIGHT / 2 + margin / 2;
    app_state.macro_buttons[2].width = button_width;
    app_state.macro_buttons[2].height = button_height;
    app_state.macro_buttons[2].color = COLOR_BLUE;
    app_state.macro_buttons[2].label = "M3";
    
    // Bottom-right: Yellow (M4) - macro 3
    app_state.macro_buttons[3].x = SCREEN_WIDTH / 2 + margin / 2;
    app_state.macro_buttons[3].y = SCREEN_HEIGHT / 2 + margin / 2;
    app_state.macro_buttons[3].width = button_width;
    app_state.macro_buttons[3].height = button_height;
    app_state.macro_buttons[3].color = COLOR_YELLOW;
    app_state.macro_buttons[3].label = "M4";
    
    // Draw the 4 macro buttons in their respective colors
    for (int i = 0; i < NUM_MACROS; i++) {
        // Skip drawing if this button will be replaced by confirm button
        if (app_state.selected_macro >= 0 && app_state.send_button_visible) {
            // Determine which button is replaced by confirm
            int replaced_idx = -1;
            switch (app_state.selected_macro) {
                case 0: replaced_idx = 3; break; // Top-left selected, bottom-right replaced
                case 1: replaced_idx = 2; break; // Top-right selected, bottom-left replaced
                case 2: replaced_idx = 1; break; // Bottom-left selected, top-right replaced
                case 3: replaced_idx = 0; break; // Bottom-right selected, top-left replaced
            }
            
            if (i == replaced_idx) {
                continue; // Skip this button, will be drawn as confirm
            }
            
            if (i == app_state.selected_macro) {
                // Dim the selected button slightly by drawing a semi-transparent overlay effect
                // For now, just draw it normally
                ESP_LOGI(TAG, "Drawing button %d (selected) at (%d, %d)", i, 
                        app_state.macro_buttons[i].x, app_state.macro_buttons[i].y);
            }
        }
        
        ESP_LOGI(TAG, "Drawing button %d (%s) at (%d, %d)", i, 
                app_state.macro_buttons[i].label,
                app_state.macro_buttons[i].x, app_state.macro_buttons[i].y);
        ili9341_draw_button(app_state.macro_buttons[i].x, app_state.macro_buttons[i].y,
                           app_state.macro_buttons[i].width, app_state.macro_buttons[i].height,
                           app_state.macro_buttons[i].color, app_state.macro_buttons[i].label);
    }
    
    // If a macro is selected, show confirm button in opposite quadrant
    if (app_state.selected_macro >= 0 && app_state.send_button_visible) {
        ESP_LOGI(TAG, "Drawing confirm button for selected macro %d", app_state.selected_macro);
        
        // Determine opposite quadrant for confirm button
        int opposite_idx = -1;
        switch (app_state.selected_macro) {
            case 0: opposite_idx = 3; break; // Top-left selected, confirm in bottom-right
            case 1: opposite_idx = 2; break; // Top-right selected, confirm in bottom-left
            case 2: opposite_idx = 1; break; // Bottom-left selected, confirm in top-right
            case 3: opposite_idx = 0; break; // Bottom-right selected, confirm in top-left
        }
        
        // Use the opposite button's position for confirm button
        app_state.confirm_button = app_state.macro_buttons[opposite_idx];
        app_state.confirm_button.color = COLOR_WHITE;
        app_state.confirm_button.label = "CONFIRM";
        
        // Draw confirm button
        ili9341_draw_button(app_state.confirm_button.x, app_state.confirm_button.y,
                           app_state.confirm_button.width, app_state.confirm_button.height,
                           app_state.confirm_button.color, app_state.confirm_button.label);
    }
    
    ESP_LOGI(TAG, "Display: Main screen drawn successfully");
}

/**
 * Draw the configuration screen
 */
static void draw_config_screen(void)
{
    ESP_LOGI(TAG, "Display: Drawing config screen...");
    ESP_LOGI(TAG, "Display: Config layout - 4 editable macro buttons + back button");
    
    // Clear screen to black
    ili9341_fill_screen(COLOR_BLACK);
    
    // Draw title area at top (optional, can add text rendering later)
    ili9341_fill_rect(0, 0, SCREEN_WIDTH, 30, COLOR_DARKBLUE);
    
    // Define button layout - same as main screen but with different behavior
    const uint16_t margin = BUTTON_MARGIN;
    const uint16_t top_margin = 35; // Leave space for title
    const uint16_t button_width = (SCREEN_WIDTH - 3 * margin) / 2;
    const uint16_t button_height = ((SCREEN_HEIGHT - top_margin - 3 * margin) / 2) - 10;
    
    // Store button positions
    // Top-left: Red (M1) - macro 0
    app_state.macro_buttons[0].x = margin;
    app_state.macro_buttons[0].y = top_margin;
    app_state.macro_buttons[0].width = button_width;
    app_state.macro_buttons[0].height = button_height;
    app_state.macro_buttons[0].color = COLOR_RED;
    
    // Top-right: Green (M2) - macro 1
    app_state.macro_buttons[1].x = SCREEN_WIDTH / 2 + margin / 2;
    app_state.macro_buttons[1].y = top_margin;
    app_state.macro_buttons[1].width = button_width;
    app_state.macro_buttons[1].height = button_height;
    app_state.macro_buttons[1].color = COLOR_GREEN;
    
    // Bottom-left: Blue (M3) - macro 2
    app_state.macro_buttons[2].x = margin;
    app_state.macro_buttons[2].y = top_margin + button_height + margin;
    app_state.macro_buttons[2].width = button_width;
    app_state.macro_buttons[2].height = button_height;
    app_state.macro_buttons[2].color = COLOR_BLUE;
    
    // Bottom-right: Yellow (M4) - macro 3
    app_state.macro_buttons[3].x = SCREEN_WIDTH / 2 + margin / 2;
    app_state.macro_buttons[3].y = top_margin + button_height + margin;
    app_state.macro_buttons[3].width = button_width;
    app_state.macro_buttons[3].height = button_height;
    app_state.macro_buttons[3].color = COLOR_YELLOW;
    
    // Draw the 4 macro buttons
    for (int i = 0; i < NUM_MACROS; i++) {
        ESP_LOGI(TAG, "Drawing config button %d at (%d, %d)", i, 
                app_state.macro_buttons[i].x, app_state.macro_buttons[i].y);
        ili9341_draw_button(app_state.macro_buttons[i].x, app_state.macro_buttons[i].y,
                           app_state.macro_buttons[i].width, app_state.macro_buttons[i].height,
                           app_state.macro_buttons[i].color, app_state.macros[i]);
    }
    
    // Draw back button at bottom
    uint16_t back_btn_width = 100;
    uint16_t back_btn_height = 30;
    uint16_t back_btn_x = (SCREEN_WIDTH - back_btn_width) / 2;
    uint16_t back_btn_y = SCREEN_HEIGHT - back_btn_height - 5;
    ili9341_draw_button(back_btn_x, back_btn_y, back_btn_width, back_btn_height, COLOR_GRAY, "BACK");
    
    ESP_LOGI(TAG, "Display: Config screen drawn successfully");
}

/**
 * Draw the on-screen keyboard
 */
static void draw_keyboard(void)
{
    ESP_LOGI(TAG, "Display: Drawing keyboard...");
    ESP_LOGI(TAG, "Display: Keyboard layout - QWERTY + special chars + controls");
    
    // Clear screen to black
    ili9341_fill_screen(COLOR_BLACK);
    
    // Draw title/text input area at top (showing what's been typed)
    ili9341_fill_rect(0, 0, SCREEN_WIDTH, 50, COLOR_DARKBLUE);
    // TODO: Add text rendering to show edit_buffer content
    
    // Draw preview of current text (just a placeholder for now)
    // Text rendering will be added later
    ESP_LOGI(TAG, "Current text: %.40s%s", app_state.edit_buffer, 
             strlen(app_state.edit_buffer) > 40 ? "..." : "");
    
    // Define keyboard layouts for different pages
    const char* kb_alpha_lower[3][10] = {
        {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
        {"a", "s", "d", "f", "g", "h", "j", "k", "l", ""},
        {"z", "x", "c", "v", "b", "n", "m", "", "", ""}
    };
    
    const char* kb_alpha_upper[3][10] = {
        {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
        {"A", "S", "D", "F", "G", "H", "J", "K", "L", ""},
        {"Z", "X", "C", "V", "B", "N", "M", "", "", ""}
    };
    
    const char* kb_numbers[3][10] = {
        {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
        {"-", "=", "[", "]", "\\", ";", "'", ",", ".", "/"},
        {"", "", "", "", "", "", "", "", "", ""}
    };
    
    const char* kb_symbols[3][10] = {
        {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"},
        {"_", "+", "{", "}", "|", ":", "\"", "<", ">", "?"},
        {"", "", "", "", "", "", "", "", "", ""}
    };
    
    // Select current keyboard layout
    const char* (*current_layout)[10] = NULL;
    switch (app_state.keyboard_page) {
        case KB_PAGE_ALPHA_LOWER:
            current_layout = kb_alpha_lower;
            break;
        case KB_PAGE_ALPHA_UPPER:
            current_layout = kb_alpha_upper;
            break;
        case KB_PAGE_NUMBERS:
            current_layout = kb_numbers;
            break;
        case KB_PAGE_SYMBOLS:
            current_layout = kb_symbols;
            break;
    }
    
    // Draw keyboard keys
    uint16_t y_pos = KEYBOARD_START_Y;
    for (int row = 0; row < KEYBOARD_ROWS; row++) {
        uint16_t x_pos = 10;
        
        for (int col = 0; col < KEYBOARD_MAX_COLS; col++) {
            if (strlen(current_layout[row][col]) > 0) {
                // Draw key
                ili9341_draw_button(x_pos, y_pos, KEY_WIDTH, KEY_HEIGHT, 
                                   COLOR_GRAY, current_layout[row][col]);
            }
            x_pos += KEY_WIDTH + KEY_MARGIN;
        }
        y_pos += KEY_HEIGHT + KEY_MARGIN;
    }
    
    // Draw control buttons at bottom
    uint16_t ctrl_y = KEYBOARD_START_Y + (KEY_HEIGHT + KEY_MARGIN) * KEYBOARD_ROWS + 5;
    
    // Page switch button (ABC/123/SYM)
    const char* page_label = "123";
    if (app_state.keyboard_page == KB_PAGE_NUMBERS) {
        page_label = "SYM";
    } else if (app_state.keyboard_page == KB_PAGE_SYMBOLS) {
        page_label = "ABC";
    }
    ili9341_draw_button(10, ctrl_y, 50, KEY_HEIGHT, COLOR_DARKBLUE, page_label);
    
    // Shift button (for uppercase/lowercase)
    if (app_state.keyboard_page == KB_PAGE_ALPHA_LOWER || 
        app_state.keyboard_page == KB_PAGE_ALPHA_UPPER) {
        const char* shift_label = app_state.keyboard_page == KB_PAGE_ALPHA_UPPER ? "abc" : "ABC";
        ili9341_draw_button(65, ctrl_y, 50, KEY_HEIGHT, COLOR_DARKBLUE, shift_label);
    }
    
    // Space bar
    ili9341_draw_button(120, ctrl_y, 80, KEY_HEIGHT, COLOR_DARKBLUE, "SPACE");
    
    // Backspace
    ili9341_draw_button(205, ctrl_y, 50, KEY_HEIGHT, COLOR_RED, "BKSP");
    
    // Save button
    ili9341_draw_button(260, ctrl_y, 50, KEY_HEIGHT, COLOR_GREEN, "SAVE");
    
    ESP_LOGI(TAG, "Display: Keyboard drawn successfully (page: %d)", app_state.keyboard_page);
}

/**
 * Draw the Bluetooth configuration screen
 */
static void draw_bt_config_screen(void)
{
    ESP_LOGI(TAG, "Display: Drawing Bluetooth config screen...");
    
    // Clear screen to black
    ili9341_fill_screen(COLOR_BLACK);
    
    // Draw title area
    ili9341_fill_rect(0, 0, SCREEN_WIDTH, 40, COLOR_DARKBLUE);
    // TODO: Add text "Bluetooth Settings"
    
    // Draw device name area
    ili9341_fill_rect(10, 50, SCREEN_WIDTH - 20, 40, COLOR_GRAY);
    // TODO: Display "Device: keybot"
    
    // Draw connection status area
    ili9341_fill_rect(10, 100, SCREEN_WIDTH - 20, 40, COLOR_GRAY);
    // TODO: Display connection status
    
    // Draw clear flash button (red, prominent)
    uint16_t clear_btn_width = 150;
    uint16_t clear_btn_height = 40;
    uint16_t clear_btn_x = (SCREEN_WIDTH - clear_btn_width) / 2;
    uint16_t clear_btn_y = 160;
    ili9341_draw_button(clear_btn_x, clear_btn_y, clear_btn_width, clear_btn_height, 
                       COLOR_RED, "CLEAR FLASH");
    
    // Draw back button
    uint16_t back_btn_width = 100;
    uint16_t back_btn_height = 30;
    uint16_t back_btn_x = (SCREEN_WIDTH - back_btn_width) / 2;
    uint16_t back_btn_y = SCREEN_HEIGHT - back_btn_height - 10;
    ili9341_draw_button(back_btn_x, back_btn_y, back_btn_width, back_btn_height, 
                       COLOR_GRAY, "BACK");
    
    ESP_LOGI(TAG, "Display: Bluetooth config screen drawn");
}

// =============================================================================
// BLUETOOTH FUNCTIONS (Stub implementations - to be completed)
// =============================================================================

/**
 * Initialize Bluetooth HID
 * 
 * Note: Full BLE HID implementation requires:
 * - Bluedroid stack initialization
 * - HID device profile setup
 * - Keyboard report descriptor
 * - GAP/GATT configuration
 * - Connection callbacks
 */
static void ble_init(void)
{
    ESP_LOGI(TAG, "Initializing Bluetooth HID...");
    
    // TODO: Initialize Bluedroid
    // TODO: Setup HID device profile
    // TODO: Set device name "keybot" (as per requirements)
    // TODO: Start advertising
    
    ESP_LOGI(TAG, "Bluetooth HID initialized (stub)");
    ESP_LOGI(TAG, "Device name will be: keybot");
}

/**
 * Send text via Bluetooth HID
 */
static void ble_send_text(const char *text)
{
    if (!app_state.ble_connected) {
        ESP_LOGW(TAG, "Bluetooth not connected, cannot send text");
        return;
    }
    
    ESP_LOGI(TAG, "Sending text via BLE: %.40s%s", text,
             strlen(text) > 40 ? "..." : "");
    
    // TODO: Convert text to HID keyboard reports
    // TODO: Send each character with proper key codes
    // TODO: Handle special characters and modifiers
}

// =============================================================================
// TOUCH HANDLING (Stub implementation - to be completed)
// =============================================================================

/**
 * Check if a point is within a button's bounds
 */
static bool is_point_in_button(uint16_t x, uint16_t y, const button_t *button)
{
    return (x >= button->x && x < (button->x + button->width) &&
            y >= button->y && y < (button->y + button->height));
}

/**
 * Get which macro button (0-3) is touched, or -1 if none
 */
static int get_touched_macro_button(uint16_t x, uint16_t y)
{
    for (int i = 0; i < NUM_MACROS; i++) {
        if (is_point_in_button(x, y, &app_state.macro_buttons[i])) {
            return i;
        }
    }
    return -1;
}

/**
 * Handle touch in playback mode
 */
static void handle_playback_touch(uint16_t x, uint16_t y, uint32_t press_duration)
{
    ESP_LOGI(TAG, "Playback touch at (%d, %d), duration: %lu ms", x, y, press_duration);
    
    // Check for long press (5 seconds for config, 10 seconds for BT config)
    if (press_duration >= BT_CONFIG_PRESS_MS) {
        ESP_LOGI(TAG, "Long press detected (>10s) - opening BT config");
        app_state.mode = MODE_BT_CONFIG;
        draw_bt_config_screen();
        return;
    } else if (press_duration >= CONFIG_PRESS_MS) {
        ESP_LOGI(TAG, "Long press detected (>5s) - opening config mode");
        app_state.mode = MODE_CONFIG;
        draw_config_screen();
        return;
    }
    
    // Short press handling
    if (press_duration < SHORT_PRESS_MS) {
        return; // Too short, ignore
    }
    
    // Check if confirm button was pressed
    if (app_state.send_button_visible && is_point_in_button(x, y, &app_state.confirm_button)) {
        ESP_LOGI(TAG, "Confirm button pressed - sending macro %d", app_state.selected_macro);
        // TODO: Send the macro via Bluetooth
        ble_send_text(app_state.macros[app_state.selected_macro]);
        
        // Reset selection
        app_state.selected_macro = -1;
        app_state.send_button_visible = false;
        draw_main_screen();
        return;
    }
    
    // Check which macro button was pressed
    int touched_button = get_touched_macro_button(x, y);
    
    if (touched_button >= 0) {
        ESP_LOGI(TAG, "Macro button %d pressed", touched_button);
        
        if (app_state.selected_macro == touched_button) {
            // Same button pressed again, cancel selection
            ESP_LOGI(TAG, "Same button pressed, canceling selection");
            app_state.selected_macro = -1;
            app_state.send_button_visible = false;
        } else {
            // New button selected, show confirm button
            ESP_LOGI(TAG, "New button selected: %d", touched_button);
            app_state.selected_macro = touched_button;
            app_state.send_button_visible = true;
            app_state.selection_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }
        
        draw_main_screen();
    } else {
        // Touch outside buttons, cancel selection
        if (app_state.selected_macro >= 0) {
            ESP_LOGI(TAG, "Touch outside buttons, canceling selection");
            app_state.selected_macro = -1;
            app_state.send_button_visible = false;
            draw_main_screen();
        }
    }
}

/**
 * Handle touch in config mode
 */
static void handle_config_touch(uint16_t x, uint16_t y)
{
    ESP_LOGI(TAG, "Config touch at (%d, %d)", x, y);
    
    // Check if back button was pressed (bottom center)
    uint16_t back_btn_width = 100;
    uint16_t back_btn_height = 30;
    uint16_t back_btn_x = (SCREEN_WIDTH - back_btn_width) / 2;
    uint16_t back_btn_y = SCREEN_HEIGHT - back_btn_height - 5;
    
    if (x >= back_btn_x && x < (back_btn_x + back_btn_width) &&
        y >= back_btn_y && y < (back_btn_y + back_btn_height)) {
        ESP_LOGI(TAG, "Back button pressed - returning to playback mode");
        app_state.mode = MODE_PLAYBACK;
        draw_main_screen();
        return;
    }
    
    // Check which macro button was pressed for editing
    int touched_button = get_touched_macro_button(x, y);
    
    if (touched_button >= 0) {
        ESP_LOGI(TAG, "Edit button %d pressed", touched_button);
        app_state.editing_macro = touched_button;
        app_state.mode = MODE_EDIT_KEYBOARD;
        app_state.keyboard_page = KB_PAGE_ALPHA_LOWER;
        
        // Copy current macro to edit buffer
        strncpy(app_state.edit_buffer, app_state.macros[touched_button], MAX_MACRO_LEN - 1);
        app_state.edit_buffer[MAX_MACRO_LEN - 1] = '\0';
        app_state.edit_buffer_len = strlen(app_state.edit_buffer);
        
        draw_keyboard();
    }
}

/**
 * Handle touch in keyboard mode
 */
static void handle_keyboard_touch(uint16_t x, uint16_t y)
{
    ESP_LOGI(TAG, "Keyboard touch at (%d, %d)", x, y);
    
    // Control buttons Y position
    uint16_t ctrl_y = KEYBOARD_START_Y + (KEY_HEIGHT + KEY_MARGIN) * KEYBOARD_ROWS + 5;
    
    // Check control buttons first
    // Page switch button (ABC/123/SYM)
    if (x >= 10 && x < 60 && y >= ctrl_y && y < (ctrl_y + KEY_HEIGHT)) {
        ESP_LOGI(TAG, "Page switch button pressed");
        // Cycle through pages
        switch (app_state.keyboard_page) {
            case KB_PAGE_ALPHA_LOWER:
            case KB_PAGE_ALPHA_UPPER:
                app_state.keyboard_page = KB_PAGE_NUMBERS;
                break;
            case KB_PAGE_NUMBERS:
                app_state.keyboard_page = KB_PAGE_SYMBOLS;
                break;
            case KB_PAGE_SYMBOLS:
                app_state.keyboard_page = KB_PAGE_ALPHA_LOWER;
                break;
        }
        draw_keyboard();
        return;
    }
    
    // Shift button (for uppercase/lowercase)
    if (x >= 65 && x < 115 && y >= ctrl_y && y < (ctrl_y + KEY_HEIGHT)) {
        ESP_LOGI(TAG, "Shift button pressed");
        if (app_state.keyboard_page == KB_PAGE_ALPHA_LOWER) {
            app_state.keyboard_page = KB_PAGE_ALPHA_UPPER;
        } else if (app_state.keyboard_page == KB_PAGE_ALPHA_UPPER) {
            app_state.keyboard_page = KB_PAGE_ALPHA_LOWER;
        }
        draw_keyboard();
        return;
    }
    
    // Space bar
    if (x >= 120 && x < 200 && y >= ctrl_y && y < (ctrl_y + KEY_HEIGHT)) {
        ESP_LOGI(TAG, "Space button pressed");
        if (app_state.edit_buffer_len < MAX_MACRO_LEN - 1) {
            app_state.edit_buffer[app_state.edit_buffer_len++] = ' ';
            app_state.edit_buffer[app_state.edit_buffer_len] = '\0';
            draw_keyboard();
        }
        return;
    }
    
    // Backspace
    if (x >= 205 && x < 255 && y >= ctrl_y && y < (ctrl_y + KEY_HEIGHT)) {
        ESP_LOGI(TAG, "Backspace button pressed");
        if (app_state.edit_buffer_len > 0) {
            app_state.edit_buffer[--app_state.edit_buffer_len] = '\0';
            draw_keyboard();
        }
        return;
    }
    
    // Save button
    if (x >= 260 && x < 310 && y >= ctrl_y && y < (ctrl_y + KEY_HEIGHT)) {
        ESP_LOGI(TAG, "Save button pressed");
        // Save the macro
        save_macro(app_state.editing_macro, app_state.edit_buffer);
        
        // Return to config screen
        app_state.mode = MODE_CONFIG;
        app_state.editing_macro = -1;
        memset(app_state.edit_buffer, 0, sizeof(app_state.edit_buffer));
        app_state.edit_buffer_len = 0;
        draw_config_screen();
        return;
    }
    
    // Check keyboard keys
    if (y >= KEYBOARD_START_Y && y < (KEYBOARD_START_Y + (KEY_HEIGHT + KEY_MARGIN) * KEYBOARD_ROWS)) {
        // Calculate which key was pressed
        int row = (y - KEYBOARD_START_Y) / (KEY_HEIGHT + KEY_MARGIN);
        int col = (x - 10) / (KEY_WIDTH + KEY_MARGIN);
        
        if (row >= 0 && row < KEYBOARD_ROWS && col >= 0 && col < KEYBOARD_MAX_COLS) {
            // Get the character for this key
            const char* ch = NULL;
            
            // Define keyboard layouts (same as in draw_keyboard)
            const char* kb_alpha_lower[3][10] = {
                {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
                {"a", "s", "d", "f", "g", "h", "j", "k", "l", ""},
                {"z", "x", "c", "v", "b", "n", "m", "", "", ""}
            };
            
            const char* kb_alpha_upper[3][10] = {
                {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
                {"A", "S", "D", "F", "G", "H", "J", "K", "L", ""},
                {"Z", "X", "C", "V", "B", "N", "M", "", "", ""}
            };
            
            const char* kb_numbers[3][10] = {
                {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
                {"-", "=", "[", "]", "\\", ";", "'", ",", ".", "/"},
                {"", "", "", "", "", "", "", "", "", ""}
            };
            
            const char* kb_symbols[3][10] = {
                {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"},
                {"_", "+", "{", "}", "|", ":", "\"", "<", ">", "?"},
                {"", "", "", "", "", "", "", "", "", ""}
            };
            
            // Select current keyboard layout
            switch (app_state.keyboard_page) {
                case KB_PAGE_ALPHA_LOWER:
                    ch = kb_alpha_lower[row][col];
                    break;
                case KB_PAGE_ALPHA_UPPER:
                    ch = kb_alpha_upper[row][col];
                    break;
                case KB_PAGE_NUMBERS:
                    ch = kb_numbers[row][col];
                    break;
                case KB_PAGE_SYMBOLS:
                    ch = kb_symbols[row][col];
                    break;
            }
            
            if (ch && strlen(ch) > 0 && app_state.edit_buffer_len < MAX_MACRO_LEN - 1) {
                ESP_LOGI(TAG, "Key pressed: '%s'", ch);
                app_state.edit_buffer[app_state.edit_buffer_len++] = ch[0];
                app_state.edit_buffer[app_state.edit_buffer_len] = '\0';
                draw_keyboard();
            }
        }
    }
}

/**
 * Handle touch in Bluetooth config mode
 */
static void handle_bt_config_touch(uint16_t x, uint16_t y)
{
    ESP_LOGI(TAG, "BT config touch at (%d, %d)", x, y);
    
    // Check if back button was pressed
    uint16_t back_btn_width = 100;
    uint16_t back_btn_height = 30;
    uint16_t back_btn_x = (SCREEN_WIDTH - back_btn_width) / 2;
    uint16_t back_btn_y = SCREEN_HEIGHT - back_btn_height - 10;
    
    if (x >= back_btn_x && x < (back_btn_x + back_btn_width) &&
        y >= back_btn_y && y < (back_btn_y + back_btn_height)) {
        ESP_LOGI(TAG, "Back button pressed - returning to playback mode");
        app_state.mode = MODE_PLAYBACK;
        draw_main_screen();
        return;
    }
    
    // Check if clear flash button was pressed
    uint16_t clear_btn_width = 150;
    uint16_t clear_btn_height = 40;
    uint16_t clear_btn_x = (SCREEN_WIDTH - clear_btn_width) / 2;
    uint16_t clear_btn_y = 160;
    
    if (x >= clear_btn_x && x < (clear_btn_x + clear_btn_width) &&
        y >= clear_btn_y && y < (clear_btn_y + clear_btn_height)) {
        ESP_LOGI(TAG, "Clear flash button pressed - erasing NVS");
        
        // Erase NVS namespace
        nvs_handle_t nvs_handle;
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
        if (err == ESP_OK) {
            nvs_erase_all(nvs_handle);
            nvs_commit(nvs_handle);
            nvs_close(nvs_handle);
            ESP_LOGI(TAG, "NVS erased successfully");
        } else {
            ESP_LOGE(TAG, "Failed to open NVS for erasing: %s", esp_err_to_name(err));
        }
        
        // Reload macros (will get defaults)
        load_macros();
        
        // Visual feedback - flash screen or show message
        ili9341_fill_screen(COLOR_RED);
        vTaskDelay(pdMS_TO_TICKS(500));
        
        // Return to main screen
        app_state.mode = MODE_PLAYBACK;
        draw_main_screen();
    }
}

/**
 * Touch handling task
 * 
 * Reads touch coordinates from XPT2046 and logs them for debugging.
 * Full UI interaction will be implemented later.
 */
static void handle_touch_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Touch task started");
    
    uint16_t last_x = 0, last_y = 0;
    bool was_touched = false;
    uint32_t touch_start_time = 0;
    
    while (1) {
        uint16_t raw_x, raw_y;
        
        // Read raw touch coordinates from XPT2046
        if (read_touch_coordinates(&raw_x, &raw_y)) {
            // Touch detected
            if (!was_touched) {
                // New touch started
                touch_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
                ESP_LOGI(TAG, "Touch started - Raw coordinates: X=%d, Y=%d", raw_x, raw_y);
                was_touched = true;
                last_x = raw_x;
                last_y = raw_y;
            } else {
                // Touch continuing
                // Only log if coordinates changed significantly
                if (abs((int)raw_x - (int)last_x) > 50 || abs((int)raw_y - (int)last_y) > 50) {
                    uint32_t duration = (xTaskGetTickCount() * portTICK_PERIOD_MS) - touch_start_time;
                    ESP_LOGI(TAG, "Touch moved - Raw: X=%d, Y=%d, Duration: %lu ms", raw_x, raw_y, duration);
                    last_x = raw_x;
                    last_y = raw_y;
                }
            }
        } else {
            // No touch detected
            if (was_touched) {
                // Touch just released
                uint32_t press_duration = (xTaskGetTickCount() * portTICK_PERIOD_MS) - touch_start_time;
                ESP_LOGI(TAG, "Touch released - Duration: %lu ms", press_duration);
                
                // Map raw coordinates to screen coordinates
                // XPT2046 typical range: 0-4095 for 12-bit ADC
                // Screen: 320x240 in landscape mode
                // Note: Calibration may be needed for specific hardware
                // For now, using a simple linear mapping
                // These values may need adjustment based on your specific display
                
                // Swap and invert coordinates for landscape mode
                // This assumes the touch controller's orientation relative to display
                uint16_t screen_x = (raw_y * SCREEN_WIDTH) / 4095;
                uint16_t screen_y = SCREEN_HEIGHT - ((raw_x * SCREEN_HEIGHT) / 4095);
                
                // Clamp to screen bounds
                if (screen_x >= SCREEN_WIDTH) screen_x = SCREEN_WIDTH - 1;
                if (screen_y >= SCREEN_HEIGHT) screen_y = SCREEN_HEIGHT - 1;
                
                ESP_LOGI(TAG, "Mapped touch to screen coordinates: X=%d, Y=%d", screen_x, screen_y);
                
                // Handle touch based on current mode
                switch (app_state.mode) {
                    case MODE_PLAYBACK:
                        handle_playback_touch(screen_x, screen_y, press_duration);
                        break;
                    
                    case MODE_CONFIG:
                        handle_config_touch(screen_x, screen_y);
                        break;
                    
                    case MODE_EDIT_KEYBOARD:
                        handle_keyboard_touch(screen_x, screen_y);
                        break;
                    
                    case MODE_BT_CONFIG:
                        handle_bt_config_touch(screen_x, screen_y);
                        break;
                    
                    default:
                        break;
                }
                
                was_touched = false;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(50)); // 20Hz polling
    }
}

// =============================================================================
// UI TASK
// =============================================================================

/**
 * Main UI task
 */
static void ui_task(void *pvParameters)
{
    ESP_LOGI(TAG, "UI task started");
    
    // Check if we should run display test
    if (app_state.mode == MODE_DISPLAY_TEST) {
        ESP_LOGI(TAG, "Running display test sequence...");
        run_display_test();
        
        // After test completes, switch to normal playback mode
        app_state.mode = MODE_PLAYBACK;
        ESP_LOGI(TAG, "Switched to playback mode");
    }
    
    // Draw initial screen
    draw_main_screen();
    
    uint32_t last_update = 0;
    
    while (1) {
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Check for selection timeout (5 seconds)
        if (app_state.selected_macro >= 0 && app_state.send_button_visible) {
            if (now - app_state.selection_time > SELECTION_TIMEOUT_MS) {
                ESP_LOGI(TAG, "Selection timeout, clearing");
                app_state.selected_macro = -1;
                app_state.send_button_visible = false;
                draw_main_screen();
            }
        }
        
        // Redraw screen periodically if needed
        if (now - last_update > 1000) {
            // Update BLE status or other dynamic elements if needed
            // For now, just update timestamp
            last_update = now;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz update rate
    }
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * Reset macro selection state
 */
static void reset_selection(void)
{
    app_state.selected_macro = -1;
    app_state.send_button_visible = false;
    app_state.selection_time = 0;
}

/*
 * =============================================================================
 * IMPLEMENTATION STATUS
 * =============================================================================
 * 
 * ✅ COMPLETED:
 * - NVS persistent storage for macros
 * - SPI bus initialization
 * - ILI9341 display initialization with full command sequence
 * - Hardware and software reset
 * - Display configuration (orientation, color depth, power control)
 * - Detailed console logging for debugging
 * - Basic framework structure
 * 
 * ⏳ TODO (Future Work):
 * 
 * 1. Display Driver Extensions:
 *    - Drawing primitives (pixels, lines, rectangles, text)
 *    - Font rendering
 *    - Touch controller (XPT2046) driver
 *    - Touch calibration routines
 * 
 * 2. Bluetooth HID (bluetooth.c):
 *    - Bluedroid stack setup
 *    - HID Device profile
 *    - Keyboard report descriptor
 *    - Key mapping for characters
 *    - Connection state management
 * 
 * 3. UI Implementation:
 *    - Main screen rendering
 *    - Configuration screen
 *    - On-screen keyboard
 *    - Touch event handling
 * 
 * 4. Additional Components (Optional):
 *    - lvgl (for advanced UI)
 *    - esp_lcd (for display abstraction)
 *    - esp_lvgl_port (for lvgl integration)
 * 
 * Reference:
 * - Display initialization: https://www.lcdwiki.com/2.8inch_ESP32-32E_Display
 * - ESP-IDF examples:
 *   * examples/bluetooth/bluedroid/ble/ble_hid_device_demo
 *   * examples/peripherals/lcd/i80_controller
 *   * examples/peripherals/spi_master
 * 
 * Build and Flash:
 * $ idf.py build
 * $ idf.py -p /dev/ttyUSB0 flash monitor
 * 
 * Expected Console Output on Startup:
 * - "Display: Starting ILI9341 initialization..."
 * - "Display: Configuring control pins..."
 * - "Display: Adding SPI device..."
 * - "Display: Performing hardware reset..."
 * - "Display: Sending software reset command..."
 * - "Display: Configuring power control..."
 * - "Display: Setting power levels..."
 * - "Display: Setting orientation (landscape)..."
 * - "Display: Setting pixel format (RGB565)..."
 * - "Display: Configuring frame rate..."
 * - "Display: Configuring gamma correction..."
 * - "Display: Clearing screen to black..."
 * - "Display: Turning on display..."
 * - "Display: ILI9341 initialization complete!"
 */
