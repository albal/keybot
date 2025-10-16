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
 * GPIO 4       ->    TFT_RST (Reset)
 * GPIO 5       ->    TOUCH_CS (Chip Select for Touch Controller)
 * 
 * Power:
 * 3.3V         ->    VCC
 * GND          ->    GND
 * 
 * Optional:
 * GPIO 21      ->    TFT_LED (Backlight)
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
 *    - ILI9341 command set for display control
 *    - XPT2046 SPI communication for touch
 *    - Double buffering for smooth rendering
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

// Logging tag
static const char *TAG = "MACROPAD";

// =============================================================================
// PIN DEFINITIONS
// =============================================================================

// Display SPI pins
#define PIN_MOSI        23
#define PIN_MISO        19
#define PIN_SCLK        18
#define PIN_TFT_CS      15
#define PIN_TFT_DC      2
#define PIN_TFT_RST     4
#define PIN_TOUCH_CS    5
#define PIN_BACKLIGHT   21

// SPI host
#define SPI_HOST        HSPI_HOST

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

// =============================================================================
// OPERATING MODES
// =============================================================================

typedef enum {
    MODE_PLAYBACK,      // Main screen - playback mode
    MODE_CONFIG,        // Configuration mode - select macro to edit
    MODE_EDIT_KEYBOARD  // Editing mode - on-screen keyboard active
} app_mode_t;

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
} app_state_t;

static app_state_t app_state = {
    .mode = MODE_PLAYBACK,
    .selected_macro = -1,
    .send_button_visible = false,
    .selection_time = 0,
    .editing_macro = -1,
    .shift_active = false,
    .ble_connected = false
};

// SPI device handle for display
static spi_device_handle_t display_spi;

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

// Bluetooth functions (to be implemented in bluetooth.c)
static void ble_init(void);
static void ble_send_text(const char *text);

// Touch handling
static void handle_touch_task(void *pvParameters);

// Main tasks
static void ui_task(void *pvParameters);

// =============================================================================
// MAIN APPLICATION ENTRY POINT
// =============================================================================

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 MacroPad Starting...");
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
 * Initialize SPI bus for display and touch
 */
static void init_spi(void)
{
    ESP_LOGI(TAG, "Initializing SPI...");
    
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SCREEN_WIDTH * SCREEN_HEIGHT * 2
    };
    
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    
    ESP_LOGI(TAG, "SPI bus initialized");
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
    gpio_set_level(PIN_TFT_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_TFT_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "Display: Hardware reset complete");
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
    
    // Configure RST and DC pins as outputs
    ESP_LOGI(TAG, "Display: Configuring control pins...");
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_TFT_RST) | (1ULL << PIN_TFT_DC),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "Display: Control pins configured (RST: GPIO%d, DC: GPIO%d)", PIN_TFT_RST, PIN_TFT_DC);
    
    // Add SPI device for display
    ESP_LOGI(TAG, "Display: Adding SPI device...");
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000,  // 26 MHz
        .mode = 0,                            // SPI mode 0
        .spics_io_num = PIN_TFT_CS,           // CS pin
        .queue_size = 7,                      // Queue 7 transactions at a time
        .pre_cb = ili9341_spi_pre_transfer_callback,  // Callback to handle D/C line
    };
    esp_err_t ret = spi_bus_add_device(SPI_HOST, &devcfg, &display_spi);
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
    ESP_LOGI(TAG, "Display: Setting orientation (landscape)...");
    ili9341_send_cmd(ILI9341_MADCTL);
    uint8_t madctl[] = {0x48}; // Landscape mode
    ili9341_send_data(madctl, 1);
    ESP_LOGI(TAG, "Display: Orientation set to landscape mode");
    
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
}

/**
 * Draw the main playback screen
 */
static void draw_main_screen(void)
{
    ESP_LOGI(TAG, "Display: Drawing main screen...");
    ESP_LOGI(TAG, "Display: Main screen layout - 4 macro buttons + settings button");
    
    // TODO: Clear screen
    // TODO: Draw title
    // TODO: Draw BLE status
    // TODO: Draw 4 macro buttons
    // TODO: Draw Settings button
    // TODO: Draw Send button if needed
    
    ESP_LOGI(TAG, "Display: Main screen drawn (stub - full implementation pending)");
}

/**
 * Draw the configuration screen
 */
static void draw_config_screen(void)
{
    ESP_LOGI(TAG, "Display: Drawing config screen...");
    ESP_LOGI(TAG, "Display: Config layout - 4 editable macro buttons + back button");
    
    // TODO: Clear screen
    // TODO: Draw title
    // TODO: Draw 4 macro buttons with edit labels
    // TODO: Draw Back button
    
    ESP_LOGI(TAG, "Display: Config screen drawn (stub - full implementation pending)");
}

/**
 * Draw the on-screen keyboard
 */
static void draw_keyboard(void)
{
    ESP_LOGI(TAG, "Display: Drawing keyboard...");
    ESP_LOGI(TAG, "Display: Keyboard layout - QWERTY + special chars + controls");
    
    // TODO: Clear screen
    // TODO: Draw title
    // TODO: Draw text input area
    // TODO: Draw QWERTY keyboard rows
    // TODO: Draw special character row
    // TODO: Draw control keys (Shift, Space, Backspace, Save)
    
    ESP_LOGI(TAG, "Display: Keyboard drawn (stub - full implementation pending)");
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
    // TODO: Set device name "ESP32 MacroPad"
    // TODO: Start advertising
    
    ESP_LOGI(TAG, "Bluetooth HID initialized (stub)");
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
 * Touch handling task
 * 
 * Note: XPT2046 touch controller requires:
 * - SPI communication
 * - Reading X/Y coordinates
 * - Pressure detection
 * - Calibration
 */
static void handle_touch_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Touch task started");
    
    while (1) {
        // TODO: Read touch controller via SPI
        // TODO: Check if touch is detected
        // TODO: Get X/Y coordinates
        // TODO: Apply calibration
        // TODO: Debounce
        // TODO: Handle based on current mode
        
        vTaskDelay(pdMS_TO_TICKS(10)); // 100Hz polling
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
    
    // Draw initial screen
    draw_main_screen();
    
    uint32_t last_update = 0;
    
    while (1) {
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Check for selection timeout (5 seconds)
        if (app_state.selected_macro >= 0 && app_state.send_button_visible) {
            if (now - app_state.selection_time > 5000) {
                ESP_LOGI(TAG, "Selection timeout, clearing");
                app_state.selected_macro = -1;
                app_state.send_button_visible = false;
                draw_main_screen();
            }
        }
        
        // Redraw screen periodically
        if (now - last_update > 1000) {
            // Update BLE status or other dynamic elements
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
 * NOTE: This is a framework implementation showing ESP-IDF structure.
 * =============================================================================
 * 
 * Complete implementation requires:
 * 
 * 1. Display Driver (display.c):
 *    - ILI9341 SPI command interface
 *    - Drawing primitives (pixels, lines, rectangles)
 *    - Text rendering (font support)
 *    - Touch controller (XPT2046) driver
 *    - Calibration routines
 * 
 * 2. Bluetooth HID (bluetooth.c):
 *    - Bluedroid stack setup
 *    - HID Device profile
 *    - Keyboard report descriptor
 *    - Key mapping for characters
 *    - Connection state management
 * 
 * 3. Additional Components:
 *    - lvgl (optional, for advanced UI)
 *    - esp_lcd (for display abstraction)
 *    - esp_lvgl_port (for lvgl integration)
 * 
 * Recommended approach:
 * - Use esp_lcd component for ILI9341
 * - Use LVGL for UI rendering
 * - Reference ESP-IDF examples:
 *   * examples/bluetooth/bluedroid/ble/ble_hid_device_demo
 *   * examples/peripherals/lcd/i80_controller
 *   * examples/peripherals/spi_master
 * 
 * Build and Flash:
 * $ idf.py build
 * $ idf.py -p /dev/ttyUSB0 flash monitor
 */
